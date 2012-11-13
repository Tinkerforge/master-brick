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
#include "rs485_low_level.h"

#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/pearson_hash.h"
#include "bricklib/utility/util_definitions.h"

#include "bricklib/bricklet/bricklet_config.h"

#include "bricklib/free_rtos/include/FreeRTOS.h"
#include "bricklib/free_rtos/include/task.h"

extern uint8_t rs485_type;

void rs485_slave_init(void) {
	logrsi("Slave init\n\r");
    rs485_init();

	xTaskCreate(rs485_slave_message_loop,
				(signed char *)"rss_ml",
				1000,
				NULL,
				1,
				(xTaskHandle *)NULL);

	rs485_type = RS485_TYPE_SLAVE;
    rs485_low_level_recv();
}

void rs485_slave_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = RS485_MAX_DATA_LENGTH;
	mlp.com_type    = COM_RS485;
	mlp.return_func = rs485_slave_message_loop_return;
	com_message_loop(&mlp);
}

void rs485_slave_message_loop_return(char *data, uint16_t length) {
/*	const uint8_t stack_id = get_stack_id_from_data(data);

	if(stack_id == com_stack_id || stack_id == 0) {
		const ComMessage *com_message = get_com_from_data(data);
		if(com_message->reply_func != NULL) {
			com_message->reply_func(COM_RS485, (void*)data);
		}

		return;
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

	if(stack_id <= com_last_ext_id[0]) {
		send_blocking_with_timeout(data, length, com_ext[0]);
		return;
	}

	if(stack_id <= com_last_ext_id[1]) {
		send_blocking_with_timeout(data, length, com_ext[1]);
		return;
	}*/
}
