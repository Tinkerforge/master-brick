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

uint8_t chibi_first_message = 0;

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

	chibi_first_message = 1;
}

void chibi_slave_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = CHIBI_MAX_DATA_LENGTH;
	mlp.com_type    = COM_CHIBI;
	mlp.return_func = chibi_slave_message_loop_return;
	com_message_loop(&mlp);
}

void chibi_slave_message_loop_return(const char *data, const uint16_t length) {
	com_route_message_from_pc(data, length, COM_CHIBI);
}
