/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi_master.c: chibi protocol implementation for master (PC connection)
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

#include "chibi_master.h"
#include "chibi_low_level.h"
#include "chibi.h"

#include "config.h"
#include "bricklib/logging/logging.h"

#include "bricklib/com/com_common.h"

#include <FreeRTOS.h>
#include <task.h>

extern uint8_t chibi_type;
extern ComType com_current;

void chibi_master_init(void) {
	chibi_type = CHIBI_TYPE_MASTER;
	logchibii("Configuring chibi extension as Master\n\r");

    chibi_init();

	xTaskCreate(chibi_master_message_loop,
				(signed char *)"chm_ml",
				2000,
				NULL,
				1,
				(xTaskHandle *)NULL);
}

void chibi_master_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = CHIBI_MAX_DATA_LENGTH;
	mlp.com_type    = COM_CHIBI;
	mlp.return_func = chibi_master_message_loop_return;
	com_message_loop(&mlp);
}

void chibi_master_message_loop_return(char *data, uint16_t length) {
	send_blocking_with_timeout(data, length, com_current);
}
