/* master-brick
 * Copyright (C) 2010-2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: Implementation of Master-Brick specific messages
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


#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>

#define TYPE_GET_STACK_VOLTAGE 1
#define TYPE_GET_STACK_CURRENT 2

#define COM_MESSAGES_USER \
	{TYPE_GET_STACK_VOLTAGE, (message_handler_func_t)get_stack_voltage}, \
	{TYPE_GET_STACK_CURRENT, (message_handler_func_t)get_stack_current}, \

typedef struct {
	uint8_t stack_address;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetStackVoltage;

typedef struct {
	uint8_t stack_address;
	uint8_t type;
	uint16_t length;
	uint16_t voltage;
} __attribute__((__packed__)) GetStackVoltageReturn;

typedef struct {
	uint8_t stack_address;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetStackCurrent;

typedef struct {
	uint8_t stack_address;
	uint8_t type;
	uint16_t length;
	uint16_t current;
} __attribute__((__packed__)) GetStackCurrentReturn;

void get_stack_voltage(uint8_t com, const GetStackVoltage *data);
void get_stack_current(uint8_t com, const GetStackCurrent *data);

#endif
