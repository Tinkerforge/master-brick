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
#define TYPE_SET_CHIBI_MASTER_ADDRESS 8
#define TYPE_GET_CHIBI_MASTER_ADDRESS 9
#define TYPE_SET_CHIBI_SLAVE_ADDRESS 10
#define TYPE_GET_CHIBI_SLAVE_ADDRESS 11
#define TYPE_GET_CHIBI_SIGNAL_STRENGTH 12
#define TYPE_GET_CHIBI_ERROR_LOG 13
#define TYPE_SET_CHIBI_FREQUENCY 14
#define TYPE_GET_CHIBI_FREQUENCY 15
#define TYPE_SET_CHIBI_CHANNEL 16
#define TYPE_GET_CHIBI_CHANNEL 17

#define COM_MESSAGES_USER \
	{TYPE_GET_STACK_VOLTAGE, (message_handler_func_t)get_stack_voltage}, \
	{TYPE_GET_STACK_CURRENT, (message_handler_func_t)get_stack_current}, \
	{TYPE_SET_EXTENSION_TYPE, (message_handler_func_t)set_extension_type}, \
	{TYPE_GET_EXTENSION_TYPE, (message_handler_func_t)get_extension_type}, \
	{TYPE_IS_CHIBI_PRESENT, (message_handler_func_t)is_chibi_present}, \
	{TYPE_SET_CHIBI_ADDRESS, (message_handler_func_t)set_chibi_address}, \
	{TYPE_GET_CHIBI_ADDRESS, (message_handler_func_t)get_chibi_address}, \
	{TYPE_SET_CHIBI_MASTER_ADDRESS, (message_handler_func_t)set_chibi_master_address}, \
	{TYPE_GET_CHIBI_MASTER_ADDRESS, (message_handler_func_t)get_chibi_master_address}, \
	{TYPE_SET_CHIBI_SLAVE_ADDRESS, (message_handler_func_t)set_chibi_slave_address}, \
	{TYPE_GET_CHIBI_SLAVE_ADDRESS, (message_handler_func_t)get_chibi_slave_address}, \
	{TYPE_GET_CHIBI_SIGNAL_STRENGTH, (message_handler_func_t)get_chibi_signal_strength}, \
	{TYPE_GET_CHIBI_ERROR_LOG, (message_handler_func_t)get_chibi_error_log}, \
	{TYPE_SET_CHIBI_FREQUENCY, (message_handler_func_t)set_chibi_frequency}, \
	{TYPE_GET_CHIBI_FREQUENCY, (message_handler_func_t)get_chibi_frequency}, \
	{TYPE_SET_CHIBI_CHANNEL, (message_handler_func_t)set_chibi_channel}, \
	{TYPE_GET_CHIBI_CHANNEL, (message_handler_func_t)get_chibi_channel}, \


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
	uint8_t address;
} __attribute__((__packed__)) SetChibiMasterAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiMasterAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetChibiMasterAddressReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t num;
	uint8_t address;
} __attribute__((__packed__)) SetChibiSlaveAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t num;
} __attribute__((__packed__)) GetChibiSlaveAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetChibiSlaveAddressReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiSignalStrength;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t signal_strength;
} __attribute__((__packed__)) GetChibiSignalStrengthReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiErrorLog;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t underrun;
	uint16_t crc_error;
	uint16_t no_ack;
	uint16_t overflow;
} __attribute__((__packed__)) GetChibiErrorLogReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t frequency;
} __attribute__((__packed__)) SetChibiFrequency;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiFrequency;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t frequency;
} __attribute__((__packed__)) GetChibiFrequencyReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t channel;
} __attribute__((__packed__)) SetChibiChannel;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiChannel;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t channel;
} __attribute__((__packed__)) GetChibiChannelReturn;

void get_stack_voltage(uint8_t com, const GetStackVoltage *data);
void get_stack_current(uint8_t com, const GetStackCurrent *data);
void set_extension_type(uint8_t com, const SetExtensionType *data);
void get_extension_type(uint8_t com, const GetExtensionType *data);
void is_chibi_present(uint8_t com, const IsChibiPresent *data);
void set_chibi_address(uint8_t com, const SetChibiAddress *data);
void get_chibi_address(uint8_t com, const GetChibiAddress *data);
void set_chibi_master_address(uint8_t com, const SetChibiMasterAddress *data);
void get_chibi_master_address(uint8_t com, const GetChibiMasterAddress *data);
void set_chibi_slave_address(uint8_t com, const SetChibiSlaveAddress *data);
void get_chibi_slave_address(uint8_t com, const GetChibiSlaveAddress *data);
void get_chibi_signal_strength(uint8_t com, const GetChibiSignalStrength *data);
void get_chibi_error_log(uint8_t com, const GetChibiErrorLog *data);
void set_chibi_frequency(uint8_t com, const SetChibiFrequency *data);
void get_chibi_frequency(uint8_t com, const GetChibiFrequency *data);
void set_chibi_channel(uint8_t com, const SetChibiChannel *data);
void get_chibi_channel(uint8_t com, const GetChibiChannel *data);

#endif
