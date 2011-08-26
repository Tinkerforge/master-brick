/* master-brick
 * Copyright (C) 2010-2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: Implementation of Master-Brick specific messages
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

#include "communication.h"

#include "bricklib/drivers/adc/adc.h"
#include "bricklib/com/com_common.h"

void get_stack_voltage(uint8_t com, const GetStackVoltage *data) {
	GetStackVoltageReturn gsvr;

	gsvr.stack_address = data->stack_address;
	gsvr.type          = data->type;
	gsvr.length        = sizeof(GetStackVoltageReturn);

	const uint16_t value = adc_channel_get_data(STACK_VOLTAGE_CHANNEL);
	if(adc_channel_get_data(STACK_VOLTAGE_CHANNEL) < VOLTAGE_EPSILON) {
		gsvr.voltage = 0;
	} else {
		gsvr.voltage = value *
	                   STACK_VOLTAGE_REFERENCE *
	                   STACK_VOLTAGE_MULTIPLIER /
	                   VOLTAGE_MAX_VALUE;
	}

	send_blocking_with_timeout(&gsvr, sizeof(GetStackVoltageReturn), com);
}

void get_stack_current(uint8_t com, const GetStackCurrent *data) {
	GetStackCurrentReturn gscr;

	gscr.stack_address = data->stack_address;
	gscr.type          = data->type;
	gscr.length        = sizeof(GetStackCurrentReturn);

	if(adc_channel_get_data(STACK_VOLTAGE_CHANNEL) < VOLTAGE_EPSILON) {
		gscr.current = 0;
	} else {
		gscr.current = adc_channel_get_data(STACK_CURRENT_CHANNEL) *
		               STACK_CURRENT_REFERENCE *
		               STACK_CURRENT_MULTIPLIER /
		               VOLTAGE_MAX_VALUE;
	}

	send_blocking_with_timeout(&gscr, sizeof(GetStackCurrentReturn), com);
}
