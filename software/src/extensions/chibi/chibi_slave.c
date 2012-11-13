/* master-brick
 * Copyright (C) 2011-2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi_slave.c: chibi protocol implementation for slave (no PC connection)
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

#include "chibi_slave.h"
#include "chibi_low_level.h"
#include "chibi.h"

#include "config.h"
#include "bricklib/logging/logging.h"

#include "bricklib/bricklet/bricklet_config.h"
#include "bricklib/com/com_common.h"
#include "bricklib/com/com_messages.h"

#include "bricklib/free_rtos/include/FreeRTOS.h"
#include "bricklib/free_rtos/include/task.h"

extern uint8_t chibi_type;
//extern ComType com_ext[];

void chibi_slave_init(void) {
	logchibii("Configuring chibi extension as Slave\n\r");
	chibi_type = CHIBI_TYPE_SLAVE;

	chibi_low_level_init();

	xTaskCreate(chibi_slave_message_loop,
				(signed char *)"chs_ml",
				MESSAGE_LOOP_SIZE,
				NULL,
				1,
				(xTaskHandle *)NULL);
}

void chibi_slave_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = CHIBI_MAX_DATA_LENGTH;
	mlp.com_type    = COM_CHIBI;
	mlp.return_func = chibi_slave_message_loop_return;
	com_message_loop(&mlp);
}

void chibi_slave_message_loop_return(char *data, const uint16_t length) {
/*	const uint8_t stack_id = get_stack_id_from_data(data);

	if(stack_id == com_stack_id || stack_id == 0) {
		const ComMessage *com_message = get_com_from_data(data);
		if(com_message->reply_func != NULL) {
			com_message->reply_func(COM_CHIBI, (void*)data);
			return;
		}
	}
	for(uint8_t i = 0; i < BRICKLET_NUM; i++) {
		if(bs[i].stack_id == stack_id) {
			baddr[i].entry(BRICKLET_TYPE_INVOCATION, COM_CHIBI, (void*)data);
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
