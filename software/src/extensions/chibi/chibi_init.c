/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi_init.c: Chibi initialization
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
#include "chibi_master.h"
#include "chibi_config.h"
#include "extensions/extension_init.h"
#include "extensions/extension_i2c.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/logging/logging.h"

extern uint8_t chibi_address;
extern uint8_t chibi_slave_address[];
extern uint8_t chibi_master_address;
extern uint8_t chibi_frequency_mode;
extern uint8_t chibi_channel;

void chibi_init_masterslave(uint8_t extension) {
	Pin chibi_select_pin = PIN_CHIBI_SELECT;
	PIO_Clear(&chibi_select_pin);

	extension_i2c_read(extension,
	                   CHIBI_ADDRESS_FREQUENCY,
	                   (char*)&chibi_frequency_mode,
	                   1);
	extension_i2c_read(extension,
	                   CHIBI_ADDRESS_CHANNEL,
	                   (char*)&chibi_channel,
	                   1);
	if(chibi_frequency_mode > 3) {
		chibi_frequency_mode = 0;
	}

	if(chibi_channel > 10) {
		chibi_channel = 0;
	}

	chibi_address = extension_get_address(extension);
	chibi_master_address = extension_get_master_address(extension);
	for(uint8_t i = 0; i < CHIBI_NUM_SLAVE_ADDRESS; i++) {
		chibi_slave_address[i] = extension_get_slave_address(extension, i);
		logi("chibi: For slave %d found address %d\n\r", i, chibi_slave_address[i]);
		if(chibi_slave_address[i] == 0) {
			break;
		}
	}

	// TODO: initialize chibi pins according to extension

	if(chibi_address == chibi_master_address) {
		chibi_master_init();
	} else {
		chibi_slave_init();
	}

}
