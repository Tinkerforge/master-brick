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
#include "rs485_low_level.h"

#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/pearson_hash.h"
#include "bricklib/utility/util_definitions.h"

#include "bricklib/free_rtos/include/FreeRTOS.h"
#include "bricklib/free_rtos/include/task.h"

extern ComType com_current;

extern uint8_t rs485_buffer_send[];
extern uint16_t rs485_buffer_size_send;
extern uint16_t rs485_buffer_size_recv;

extern uint8_t rs485_mode;
extern uint8_t rs485_type;
extern uint8_t rs485_address;

extern uint8_t master_routing_table[];
extern uint8_t rs485_slave_address[];
extern uint8_t rs485_last_sequence_number;

uint16_t rs485_master_recv_counter = 3;
xTaskHandle rs485_handle_master_message_loop;
xTaskHandle rs485_handle_master_state_machine_loop;

bool rs485_master_send_empty = false;

void rs485_master_init(void) {
	logrsi("Master init\n\r");

	if(rs485_slave_address[0] == 0) {
		return;
	}

    rs485_init();

	xTaskCreate(rs485_master_state_machine_loop,
				(signed char *)"rs_sm",
				500,
				NULL,
				1,
				&rs485_handle_master_state_machine_loop);

	xTaskCreate(rs485_master_message_loop,
				(signed char *)"rsm_ml",
				MESSAGE_LOOP_SIZE,
				NULL,
				1,
				&rs485_handle_master_message_loop);

	rs485_type = RS485_TYPE_MASTER;
}

void rs485_master_state_machine_loop(void *arg) {
	if(rs485_slave_address[0] == 0) {
		return;
	}

	unsigned long last_wake_time = xTaskGetTickCount();
	uint8_t rs485_address_counter = 0;
    while(true) {
		// 1ms resolution
		unsigned long tick_count = xTaskGetTickCount();
		if(tick_count > last_wake_time + 2) {
			last_wake_time = tick_count - 1;
		}

    	vTaskDelayUntil(&last_wake_time, 1);

    	// If buffer full, don't ask for stuff. Otherwise we will just trigger
    	// unnecessary timeouts
    	if(rs485_buffer_size_recv != 0) {
    		continue;
    	}

    	if(rs485_master_recv_counter > 0) {
    		rs485_master_recv_counter--;
    	} else {
    		// Oooops, something went wrong, we were in recv mode for
    		// longer then 3ms.
    		rs485_master_send_empty = true;
    		rs485_low_level_set_mode_send_from_task();
    	}

    	if(rs485_mode == RS485_MODE_SEND) {
    		rs485_master_recv_counter = rs485_wait_time();
			// As long as receive buffer is not empty do nothing
			if(rs485_buffer_size_recv == 0) {
				// Nothing to send just ask for stuff
				if(rs485_buffer_size_send == 0 || rs485_master_send_empty) {
			    	uint8_t address = rs485_slave_address[rs485_address_counter];
			    	if(address == 0) {
			    		rs485_address_counter = 0;
			    		address = rs485_slave_address[0];
			    	}

			    	rs485_address = address;
					rs485_low_level_send(address, rs485_last_sequence_number+1, false);
					rs485_address_counter++;

				// Send message and ask for stuff
				} else {
					uint8_t address = master_routing_table[rs485_buffer_send[0]];
			    	rs485_address = address;
					rs485_low_level_send(address, rs485_last_sequence_number+1, false);
				}
			}
    	}
    }
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
