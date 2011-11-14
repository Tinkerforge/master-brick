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
#include <stdbool.h>

#define TYPE_GET_STACK_VOLTAGE 1
#define TYPE_GET_STACK_CURRENT 2
#define TYPE_SET_EXTENSION_TYPE 3
#define TYPE_GET_EXTENSION_TYPE 4
#define TYPE_IS_CHIBI_PRESENT 5
#define TYPE_SET_CHIBI_ADDRESS 6
#define TYPE_GET_CHIBI_ADDRESS 7
#define TYPE_SET_CHIBI_RECEIVER_ADDRESS 8
#define TYPE_GET_CHIBI_RECEIVER_ADDRESS 9

#define COM_MESSAGES_USER \
	{TYPE_GET_STACK_VOLTAGE, (message_handler_func_t)get_stack_voltage}, \
	{TYPE_GET_STACK_CURRENT, (message_handler_func_t)get_stack_current}, \
	{TYPE_SET_EXTENSION_TYPE, (message_handler_func_t)set_extension_type}, \
	{TYPE_GET_EXTENSION_TYPE, (message_handler_func_t)get_extension_type}, \
	{TYPE_IS_CHIBI_PRESENT, (message_handler_func_t)is_chibi_present}, \
	{TYPE_SET_CHIBI_ADDRESS, (message_handler_func_t)set_chibi_address}, \
	{TYPE_GET_CHIBI_ADDRESS, (message_handler_func_t)get_chibi_address}, \
	{TYPE_SET_CHIBI_RECEIVER_ADDRESS, (message_handler_func_t)set_chibi_receiver_address}, \
	{TYPE_GET_CHIBI_RECEIVER_ADDRESS, (message_handler_func_t)get_chibi_receiver_address}, \


typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetStackVoltage;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t voltage;
} __attribute__((__packed__)) GetStackVoltageReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetStackCurrent;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t current;
} __attribute__((__packed__)) GetStackCurrentReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t extension;
	uint32_t exttype;
} __attribute__((__packed__)) SetExtensionType;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t extension;
} __attribute__((__packed__)) GetExtensionType;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint32_t exttype;
} __attribute__((__packed__)) GetExtensionTypeReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) IsChibiPresent;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	bool present;
} __attribute__((__packed__)) IsChibiPresentReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) SetChibiAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetChibiAddressReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t num;
	uint8_t address;
} __attribute__((__packed__)) SetChibiReceiverAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t num;
} __attribute__((__packed__)) GetChibiReceiverAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetChibiReceiverAddressReturn;

void get_stack_voltage(uint8_t com, const GetStackVoltage *data);
void get_stack_current(uint8_t com, const GetStackCurrent *data);

void set_extension_type(uint8_t com, const SetExtensionType *data);
void get_extension_type(uint8_t com, const GetExtensionType *data);
void is_chibi_present(uint8_t com, const IsChibiPresent *data);
void set_chibi_address(uint8_t com, const SetChibiAddress *data);
void get_chibi_address(uint8_t com, const GetChibiAddress *data);
void set_chibi_receiver_address(uint8_t com, const SetChibiReceiverAddress *data);
void get_chibi_receiver_address(uint8_t com, const GetChibiReceiverAddress *data);

#endif
