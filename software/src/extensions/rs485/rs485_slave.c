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

void rs485_slave_message_loop_return(const char *data, const uint16_t length) {
	com_route_message_from_pc(data, length, COM_RS485);
}
