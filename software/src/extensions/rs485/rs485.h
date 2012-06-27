/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rs485.h: High Level RS485 protocol implementation
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

#ifndef RS485_H
#define RS485_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint32_t speed;
	char parity;
	uint8_t stopbits;
} __attribute__((__packed__)) RS485Config;

void rs485_init_masterslave(uint8_t extension);
bool rs485_init(void);
uint16_t rs485_send(const void *data, const uint16_t length);
uint16_t rs485_recv(void *data, const uint16_t length);
uint8_t rs485_get_receiver_address(uint8_t stack_id);

#endif
