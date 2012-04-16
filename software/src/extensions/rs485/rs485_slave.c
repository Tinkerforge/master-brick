/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rs485_slave.c: RS485 slave protocol implementation
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

#include "rs485_slave.h"

#include "config.h"
#include "rs485.h"
#include "rs485_config.h"

#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/pearson_hash.h"

#include "bricklib/bricklet/bricklet_config.h"

#include <FreeRTOS.h>
#include <task.h>

extern ComType com_current;

extern uint8_t com_stack_id;
extern uint8_t com_last_spi_stack_id;

extern BrickletSettings bs[];
extern const BrickletAddress baddr[];

extern uint8_t rs485_buffer_recv[];
extern uint8_t rs485_buffer_send[];
extern uint16_t rs485_buffer_size_send;
extern uint16_t rs485_buffer_size_recv;

extern uint8_t rs485_mode;
extern uint8_t rs485_type;

void rs485_slave_send(uint8_t address) {
	uint8_t checksum = 0;
    while((USART_RS485->US_CSR & US_CSR_TXEMPTY) == 0);
    USART_RS485->US_THR = 0xFF;
    while((USART_RS485->US_CSR & US_CSR_TXEMPTY) == 0);
    USART_RS485->US_THR = 0xFF;
    while((USART_RS485->US_CSR & US_CSR_TXEMPTY) == 0);
    USART_RS485->US_THR = 0xFF;

    while((USART_RS485->US_CSR & US_CSR_TXEMPTY) == 0);
    if(rs485_buffer_size_send == 0) {
    	USART_RS485->US_THR = address | (1 << 7);
    	PEARSON(checksum, (address | (1 << 7)));
    } else {
    	USART_RS485->US_THR = address;
    	PEARSON(checksum, address);
    }

    for(uint8_t i = 0; i < rs485_buffer_size_send; i++) {
        while((USART_RS485->US_CSR & US_CSR_TXEMPTY) == 0);
        USART_RS485->US_THR = rs485_buffer_send[i];
        PEARSON(checksum, rs485_buffer_send[i]);
    }

    while((USART_RS485->US_CSR & US_CSR_TXEMPTY) == 0);
    USART_RS485->US_THR = checksum;

    rs485_buffer_size_send = 0;
    rs485_set_mode_receive();
}

void rs485_slave_send_ack(void) {

}

void rs485_slave_init(void) {
	logrsi("Slave init\n\r");
    rs485_init();
    rs485_set_mode_receive();

	xTaskCreate(rs485_slave_state_machine_loop,
				(signed char *)"rs_sm",
				1000,
				NULL,
				1,
				(xTaskHandle *)NULL);

	xTaskCreate(rs485_slave_message_loop,
				(signed char *)"rss_ml",
				2000,
				NULL,
				1,
				(xTaskHandle *)NULL);

	rs485_type = RS485_TYPE_SLAVE;
}

void rs485_slave_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = RS485_MAX_DATA_LENGTH;
	mlp.com_type    = COM_RS485;
	mlp.return_func = rs485_slave_message_loop_return;
	com_message_loop(&mlp);
}

void rs485_slave_message_loop_return(char *data, uint16_t length) {
	const uint8_t stack_id = get_stack_id_from_data(data);

	if(stack_id == com_stack_id || stack_id == 0) {
		const ComMessage *com_message = get_com_from_data(data);
		if(com_message->reply_func != NULL) {
			com_message->reply_func(COM_RS485, (void*)data);
			return;
		}
	}
	for(uint8_t i = 0; i < BRICKLET_NUM; i++) {
		if(bs[i].stack_id == stack_id) {
			baddr[i].entry(BRICKLET_TYPE_INVOCATION, COM_RS485, (void*)data);
			return;
		}
	}

	if(stack_id <= com_last_spi_stack_id) {
		send_blocking_with_timeout(data, length, COM_SPI_STACK);
		return;
	}
}

void rs485_slave_state_machine_loop(void *arg) {
    while(true) {
    	if(rs485_mode == RS485_MODE_SEND) {
    		rs485_slave_send(1);
    	}
   		taskYIELD();
    }
}
