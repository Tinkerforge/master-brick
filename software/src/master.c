/* master-brick
 * Copyright (C) 2010-2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * master.c: Master specific functions
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

#include "master.h"

#include <stdio.h>

#include "bricklib/bricklet/bricklet_init.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/com/spi/spi_stack/spi_stack_master.h"
#include "bricklib/com/spi/spi_stack/spi_stack_common.h"
#include "bricklib/com/spi/spi_stack/spi_stack_select.h"
#include "bricklib/drivers/adc/adc.h"
#include "bricklib/logging/logging.h"
#include "bricklib/utility/util_definitions.h"

#include "extensions/chibi/chibi_master.h"
#include "extensions/chibi/chibi_slave.h"

#include "config.h"

extern uint8_t com_stack_id;
extern uint8_t com_last_stack_address;
extern uint16_t spi_stack_buffer_size_send;
extern uint8_t com_last_spi_stack_id;
uint8_t master_routing_table[MAX_STACK_IDS] = {0};

void master_init(void) {
	Pin power_pins[] = {PIN_STACK_VOLTAGE, PIN_STACK_CURRENT};
	PIO_Configure(power_pins, PIO_LISTSIZE(power_pins));

	adc_channel_enable(STACK_VOLTAGE_CHANNEL);
	adc_channel_enable(STACK_CURRENT_CHANNEL);
	/*if(usb_is_connected()) {
		chibi_master_init();
	} else {
		chibi_slave_init();
	}*/
}

void master_create_routing_table(void) {
	uint8_t stack_address = 0;
	uint8_t tries = 0;
	while(stack_address <= 8) {
		StackEnumerate se = {
			0,
			TYPE_STACK_ENUMERATE,
			sizeof(StackEnumerate),
			com_last_spi_stack_id + 1
		};

		if(spi_stack_send(&se, sizeof(StackEnumerate)) != 0) {
			spi_stack_select(stack_address + 1);

			tries = 0;
			while(!spi_stack_master_transceive() && tries < 10) {
				tries++;
			}
			if(tries == 10) {
				break;
			}

			spi_stack_deselect();
		}

		StackEnumerateReturn ser;
		tries = 0;
		while(tries < 10) {
			SLEEP_MS(50);
			spi_stack_select(stack_address + 1);
			spi_stack_master_transceive();
			spi_stack_deselect();
			if(spi_stack_recv(&ser, sizeof(StackEnumerateReturn))) {
				break;
			}
			tries++;
		}

		if(tries == 10) {
			logspise("Did not receive answer for Stack Enumerate\n\r");
			break;
		}

		stack_address++;
		for(uint8_t i = com_last_spi_stack_id + 1; i <= ser.stack_id_upto; i++) {
			master_routing_table[i] = stack_address;
		}
		com_last_spi_stack_id = ser.stack_id_upto;
	}

	spi_stack_buffer_size_send = 0;
	com_last_stack_address = stack_address;
}

void tick_task(void) {

}
