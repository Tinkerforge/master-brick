/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi.c: High Level WIFI protocol implementation
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


#include "wifi.h"

#include <string.h>

#include "config.h"
#include "wifi_config.h"
#include "wifi_command.h"
#include "wifi_data.h"
#include "wifi_low_level.h"

#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/led.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/pearson_hash.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/bricklet/bricklet_config.h"
#include "extensions/extension_init.h"
#include "extensions/extension_i2c.h"

extern uint32_t led_rxtx;

uint8_t wifi_state = WIFI_STATE_COMMAND_IDLE;
char wifi_ssid[] =    "TinkerforgeWLAN";
char wifi_wpa_key[] = "3164429505265866";
uint8_t wifi_ssid_length =    15;
uint8_t wifi_wpa_key_length = 16;

uint8_t wifi_buffer_recv[WIFI_BUFFER_SIZE] = {0};
uint16_t wifi_buffer_size_recv = 0;

extern ComType com_current;

extern uint8_t com_stack_id;
extern uint8_t com_last_spi_stack_id;

extern BrickletSettings bs[];
extern const BrickletAddress baddr[];

bool wifi_init(void) {
    Pin pins_wifi_spi[] = {PINS_WIFI_SPI};
    PIO_Configure(pins_wifi_spi, PIO_LISTSIZE(pins_wifi_spi));
    pins_wifi_spi[4].type = PIO_INPUT;

    wifi_low_level_deselect();

    uint32_t mode = US_MR_USART_MODE_SPI_MASTER |
                    US_MR_USCLKS_MCK |
                    US_MR_CHRL_8_BIT |
                    US_MR_PAR_NO |
                    US_MR_CHMODE_NORMAL |
                    US_MR_CLKO |
                    US_SPI_BPMODE_1;

    PMC->PMC_PCER0 = 1 << ID_WIFI_SPI;
    USART_Configure(USART_WIFI_SPI, mode, WIFI_SPI_CLOCK, BOARD_MCK);
    NVIC_EnableIRQ(IRQ_WIFI_SPI);


    USART_SetTransmitterEnabled(USART_WIFI_SPI, 1);
    USART_SetReceiverEnabled(USART_WIFI_SPI, 1);

    wifi_command_flush();
    wifi_command_send(WIFI_COMMAND_ID_AT_ATE0);
    wifi_command_flush();

    wifi_command_send(WIFI_COMMAND_ID_AT_ATV0);
    wifi_command_flush();

	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT);
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WD);
//	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXPS_OFF);
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXACTIVE_ON);
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_BDATA_ON);
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WWPA);
//	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_ATC_ON);
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NDHCP_ON);
//	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WAUTO);
//	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NAUTO);
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WA);
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NSTCP);
//	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_ATA);

	wifi_command_flush();

	xTaskCreate(wifi_message_loop,
				(signed char *)"wif_ml",
				MESSAGE_LOOP_SIZE,
				NULL,
				1,
				(xTaskHandle *)NULL);

    logwifii("WIFI initialized\n\r");
    return true;
}

void wifi_init_extension(uint8_t extension) {
	wifi_init();
}

uint16_t wifi_send(const void *data, const uint16_t length) {
/*	if(wifi_buffer_size_send > 0) {
		return 0;
	}*/

	led_rxtx++;

	uint16_t send_length = MIN(length, WIFI_BUFFER_SIZE);

	wifi_data_send_escape(data, send_length);

	return send_length;
}

uint16_t wifi_recv(void *data, const uint16_t length) {
	if(wifi_buffer_size_recv == 0) {
		wifi_data_poll();
		return 0;
	}
	led_rxtx++;

	static uint16_t recv_pointer = 0;

	uint16_t recv_length = MIN(length, wifi_buffer_size_recv);

	memcpy(data, wifi_buffer_recv + recv_pointer, recv_length);

	if(wifi_buffer_size_recv - recv_length == 0) {
		recv_pointer = 0;
	} else {
		recv_pointer += recv_length;
	}

	wifi_buffer_size_recv -= recv_length;

	return recv_length;
}

void wifi_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = WIFI_MAX_DATA_LENGTH;
	mlp.com_type    = COM_WIFI;
	mlp.return_func = wifi_message_loop_return;
	com_message_loop(&mlp);
}

void wifi_message_loop_return(char *data, uint16_t length) {
	const uint8_t stack_id = get_stack_id_from_data(data);

	if(stack_id == com_stack_id || stack_id == 0) {
		const ComMessage *com_message = get_com_from_data(data);
		if(com_message->reply_func != NULL) {
			com_message->reply_func(COM_WIFI, (void*)data);
		}

		return;
	}
	for(uint8_t i = 0; i < BRICKLET_NUM; i++) {
		if(bs[i].stack_id == stack_id) {
			baddr[i].entry(BRICKLET_TYPE_INVOCATION, COM_WIFI, (void*)data);
			return;
		}
	}

	if(stack_id <= com_last_spi_stack_id) {
		send_blocking_with_timeout(data, length, COM_SPI_STACK);
		return;
	}
}
