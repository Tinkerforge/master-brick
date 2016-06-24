/* master-brick
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi2.c: High Level WIFI2 protocol implementation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "wifi2.h"

#include <string.h>

#include "bricklib/free_rtos/include/FreeRTOS.h"
#include "bricklib/free_rtos/include/task.h"

#include "config.h"
#include "wifi2_config.h"
#include "wifi2_uart.h"

#include "bricklib/com/com.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/init.h"
#include "bricklib/utility/pearson_hash.h"

#include "bricklib/drivers/usart/usart.h"
#include "bricklib/drivers/pio/pio.h"


extern ComInfo com_info;
extern Pin extension_pins[];
extern Wifi2 *w2;

bool wifi2_init_extension_done[2] = { false, false };

bool wifi2_init(void) {
    const uint32_t mode = US_MR_USCLKS_MCK       |
                          US_MR_CHRL_8_BIT       |
                          US_MR_CHMODE_NORMAL    |
                          US_MR_NBSTOP_1_BIT     |
                          US_MR_PAR_NO;

    PMC->PMC_PCER0 = 1 << ID_WIFI2;
    USART_Configure(USART_WIFI2, mode, WIFI2_BAUDRATE, BOARD_MCK);
    USART_WIFI2->US_IDR = 0xFFFFFFFF;
    NVIC_EnableIRQ(IRQ_WIFI2);

    USART_SetReceiverEnabled(USART_WIFI2, 1);
    USART_SetTransmitterEnabled(USART_WIFI2, 1);

	xTaskCreate(wifi2_message_loop,
				(signed char *)"wi2_ml",
				MESSAGE_LOOP_SIZE,
				NULL,
				1,
				(xTaskHandle *)NULL);

    USART_WIFI2->US_RPR = (uint32_t)w2->recv_dma_buffer[0];
    USART_WIFI2->US_RCR = WIFI2_UART_BUFFER_SIZE;
    USART_WIFI2->US_RNPR = (uint32_t)w2->recv_dma_buffer[1];
    USART_WIFI2->US_RNCR = WIFI2_UART_BUFFER_SIZE;
    USART_WIFI2->US_PTCR = US_PTCR_RXTEN;


	w2->recv_dma_last_rcr = WIFI2_UART_BUFFER_SIZE;
	w2->recv_dma_index = 0;

	logwifi2d("WIFI2 initialized\n\r");

	return true;
}

void wifi2_init_extension(const uint8_t extension) {
	logwifi2d("WIFI2 init extension\n\r");

	wifi2_uart_init(extension);
	wifi2_init();

	wifi2_init_extension_done[extension] = true;
}


void wifi2_tick(const uint8_t tick_type) {
	wifi2_uart_tick(tick_type);
	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		return;
	}
}

uint16_t wifi2_send(const void *data, const uint16_t length, uint32_t *options) {
	if(w2->send_buffer_message_length != 0) {
		return 0;
	}

	const uint8_t send_length = MIN(length, WIFI2_UART_BUFFER_SIZE);
	memcpy(w2->send_buffer, data, send_length);
	w2->send_buffer_message_length = send_length;

	return send_length;
}

uint16_t wifi2_recv(void *data, const uint16_t length, uint32_t *options) {
	static uint16_t recv_pointer = 0;

	if(w2->recv_buffer_tfp_length == 0) {
		return 0;
	}

	uint16_t recv_length = MIN(length, w2->recv_buffer_tfp_length);

	memcpy(data, w2->recv_buffer_tfp + recv_pointer, recv_length);


	if(w2->recv_buffer_tfp_length - recv_length == 0) {
		recv_pointer = 0;
	} else {
		recv_pointer += recv_length;
	}

	w2->recv_buffer_tfp_length -= recv_length;

	if(w2->recv_buffer_tfp_length == 0) {
		ack_last_message();
	}

	return recv_length;
}

void wifi2_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = WIFI2_MAX_DATA_LENGTH;
	mlp.com_type    = COM_WIFI2;
	mlp.return_func = wifi2_message_loop_return;
	com_message_loop(&mlp);
}

void wifi2_message_loop_return(const char *data, const uint16_t length) {
	com_route_message_from_pc(data, length, COM_WIFI2);
}
