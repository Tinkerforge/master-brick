/* master-brick
 * Copyright (C) 2010-2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * master.h: Master specific functions
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

#ifndef MASTER_H
#define MASTER_H

#include <stdint.h>

#define MAX_STACK_IDS 256

#define MASTER_MODE_NONE 0
#define MASTER_MODE_SLAVE 1
#define MASTER_MODE_MASTER 2

#define MASTER_CALLBACK_VALUE_AVERAGE 100
#define MASTER_STACK_VOLTAGE_MULT 725340  // 3300*1099   / 5
#define MASTER_STACK_VOLTAGE_DIV  81081   // 4095*99     / 5

#define MASTER_CALLBACK_VALUE_DEFAULT 0
#define MASTER_CALLBACK_PERIOD_DEFAULT 0
#define MASTER_CALLBACK_OPTION_DEFAULT 'x'
#define MASTER_CALLBACK_MIN_DEFAULT 0
#define MASTER_CALLBACK_MAX_DEFAULT 0
#define MASTER_CALLBACK_COUNTER_DEFAULT 0
#define MASTER_CALLBACK_DEBOUNCE_PERIOD_DEFAULT 100

typedef struct {
	uint32_t stack_current_sum;
	uint32_t stack_voltage_sum;
	uint32_t usb_voltage_sum;
	uint16_t stack_current;
	uint16_t stack_voltage;
	uint16_t usb_voltage;
	uint16_t stack_current_last;
	uint16_t stack_voltage_last;
	uint16_t usb_voltage_last;
	uint32_t period_stack_current;
	uint32_t period_stack_voltage;
	uint32_t period_usb_voltage;
	uint32_t period_stack_current_counter;
	uint32_t period_stack_voltage_counter;
	uint32_t period_usb_voltage_counter;
	char option_stack_current;
	uint16_t min_stack_current;
	uint16_t max_stack_current;
	char option_stack_voltage;
	uint16_t min_stack_voltage;
	uint16_t max_stack_voltage;
	char option_usb_voltage;
	uint16_t min_usb_voltage;
	uint16_t max_usb_voltage;
	char option_stack_current_save;
	uint16_t min_stack_current_save;
	uint16_t max_stack_current_save;
	char option_stack_voltage_save;
	uint16_t min_stack_voltage_save;
	uint16_t max_stack_voltage_save;
	char option_usb_voltage_save;
	uint16_t min_usb_voltage_save;
	uint16_t max_usb_voltage_save;
	uint32_t threshold_period_current_stack_current;
	uint32_t threshold_period_current_stack_voltage;
	uint32_t threshold_period_current_usb_voltage;
	uint32_t debounce_period;
	uint8_t sum_counter;
} MasterCallback;

uint8_t master_get_hardware_version(void);
void master_init(void);
void tick_task(const uint8_t tick_type);

#endif
