/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rs485_master.c: RS485 master protocol implementation
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

#include "rs485_master.h"

#include "config.h"
#include "rs485.h"
#include "rs485_config.h"

#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/pearson_hash.h"

#include <FreeRTOS.h>
#include <task.h>

extern ComType com_current;

extern uint8_t rs485_buffer_recv[];
extern uint8_t rs485_buffer_send[];
extern uint16_t rs485_buffer_size_send;
extern uint16_t rs485_buffer_size_recv;
extern uint8_t rs485_mode;
extern uint8_t rs485_type;

extern uint8_t rs485_slave_address[];

void rs485_master_init(void) {
	logrsi("Master init\n\r");
    rs485_init();
    rs485_set_mode_receive();

	xTaskCreate(rs485_master_state_machine_loop,
				(signed char *)"rs_sm",
				1000,
				NULL,
				1,
				(xTaskHandle *)NULL);

	xTaskCreate(rs485_master_message_loop,
				(signed char *)"rsm_ml",
				2000,
				NULL,
				1,
				(xTaskHandle *)NULL);

	rs485_slave_address[0] = 1;
	rs485_type = RS485_TYPE_MASTER;
}

void rs485_master_state_machine_loop(void *arg) {
	uint8_t rs485_address_counter = 1;
    while(true) {
    	if(rs485_mode == RS485_MODE_SEND) {
			// As long as receive buffer is not empty do nothing
			if(rs485_buffer_size_recv == 0) {
				// Nothing to send just ask for stuff
				if(rs485_buffer_size_send == 0) {
					rs485_master_send(rs485_address_counter);
				// Send message and ask for stuff
				} else {
					rs485_master_send(rs485_address_counter);
				}

				//rs485_address_counter++;
			}
    	}
    	taskYIELD();
    }
}

void rs485_master_send(uint8_t address) {
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

void rs485_master_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = RS485_MAX_DATA_LENGTH;
	mlp.com_type    = COM_RS485;
	mlp.return_func = rs485_master_message_loop_return;
	com_message_loop(&mlp);
}

void rs485_master_message_loop_return(char *data, uint16_t length) {
	send_blocking_with_timeout(data, length, com_current);
}
