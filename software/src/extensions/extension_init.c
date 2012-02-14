/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * extension_init: Implementation of Extension initialization
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

#include "extension_init.h"
#include "extension_config.h"
#include "extension_i2c.h"

#include "bricklib/com/i2c/i2c_eeprom/i2c_eeprom_master.h"
#include "bricklib/com/i2c/i2c_eeprom/i2c_eeprom_common.h"
#include "bricklib/com/i2c/i2c_clear_bus.h"
#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "extensions/chibi/chibi_init.h"
#include "extensions/chibi/chibi_config.h"
#include "extensions/chibi/chibi_low_level.h"

#include <stdio.h>

extern ComType com_ext[];

uint8_t extension_get_slave_address(const uint8_t extension,
                                    const uint8_t num) {
	uint8_t address;
	if(extension_i2c_read(extension,
	                      EXTENSION_POS_SLAVE_ADDRESS_START + num*4,
	                      (char*)&address, 1)) {
		return address;
	}

	return 0;
}

void extension_set_slave_address(const uint8_t extension,
                                 const uint8_t num,
                                 uint8_t address) {
	extension_i2c_write(extension,
	                    EXTENSION_POS_SLAVE_ADDRESS_START + num*4,
	                    (char*)&address, 1);
}

uint8_t extension_get_master_address(const uint8_t extension) {
	uint8_t address;
	if(extension_i2c_read(extension,
	                      EXTENSION_POS_MASTER_ADDRESS,
	                      (char*)&address, 1)) {
		return address;
	}

	return 0;
}

void extension_set_master_address(const uint8_t extension, uint8_t address) {
	extension_i2c_write(extension,
	                    EXTENSION_POS_MASTER_ADDRESS,
	                    (char*)&address, 1);
}

uint8_t extension_get_address(const uint8_t extension) {
	uint8_t address;
	if(extension_i2c_read(extension,
	                      EXTENSION_POS_ADDRESS,
	                      (char*)&address, 1)) {
		return address;
	}

	return 0;
}

void extension_set_address(const uint8_t extension, uint8_t address) {
	extension_i2c_write(extension, EXTENSION_POS_ADDRESS, (char*)&address, 1);
}

uint32_t extension_get_type(const uint8_t extension) {
	uint32_t type;
	if(extension_i2c_read(extension, EXTENSION_POS_TYPE, (char*)&type, 4)) {
		return type;
	}

	return 0;
}

void extension_set_type(const uint8_t extension, uint32_t type) {
	extension_i2c_write_zeros(extension);
	extension_i2c_write(extension, EXTENSION_POS_TYPE, (char*)&type, 4);
}

uint8_t extension_init(void) {
	logexti("Start extension initialization\n\r");
	Pin extension_pins [] = {PIN_EXT_0_SELECT,
			                 PIN_EXT_0_GP_0,
			                 PIN_EXT_0_GP_1,
			                 PIN_EXT_0_GP_2,
			                 PIN_EXT_1_SELECT,
						     PIN_EXT_1_GP_0,
						     PIN_EXT_1_GP_1,
						     PIN_EXT_1_GP_2};

	PIO_Configure(extension_pins, PIO_LISTSIZE(extension_pins));

	extension_i2c_init();
	for(uint8_t i = 0; i < 1; i++) {
		if(extension_is_present(i)) {
			uint8_t type = extension_get_type(i);
			logexti("Extension %d present: %d\n\r", i, type);

			switch(type) {
				case EXTENSION_TYPE_CHIBI: {
					com_ext[i] = COM_CHIBI;
					chibi_init_masterslave(i);

					break;
				}

				case EXTENSION_TYPE_RS485: {
					com_ext[i] = COM_RS485;

					break;
				}

				case EXTENSION_TYPE_NONE:
				default: {
					com_ext[i] = COM_NONE;
				}
			}
		} else {
			logexti("Extension %d not present\n\r", i);
		}
	}
	return EXTENSION_TYPE_CHIBI;
}

bool extension_is_present(const uint8_t extension) {
	char tmp;
	return extension_i2c_read(extension, 0, &tmp, 1);
}

extern uint8_t com_last_ext_id[];
extern uint8_t com_last_spi_stack_id;
extern uint8_t master_routing_table[];
extern uint8_t chibi_slave_address[];
extern uint8_t chibi_type;

void extension_enumerate(uint8_t com, const Enumerate *data) {
	if(!(chibi_type & CHIBI_TYPE_MASTER)) {
		return;
	}

	for(uint8_t com_num = 0; com_num < 2; com_num++) {
		if(com_ext[com_num] != COM_NONE) {
			// TODO other extension than chibi
			for(uint8_t i = 0; i < CHIBI_NUM_SLAVE_ADDRESS; i++) {
				uint8_t slave_address = chibi_slave_address[i];
				logchibii("Extension Enumerate %d\n\r", slave_address);
				if(slave_address == 0) {
					return;
				}

				uint16_t id;
				for(id = com_last_spi_stack_id+1; id <= com_last_ext_id[com_num]; id++) {
					if(master_routing_table[id] == slave_address) {
						break;
					}
				}
				logchibii("Extension Enumerate id %d %d\n\r", id, com_last_ext_id[com_num]+1);

				if(id <= com_last_ext_id[com_num]) {
					Enumerate e = {
						id,
						TYPE_ENUMERATE,
						sizeof(Enumerate),
					};
					send_blocking_with_timeout(&e, sizeof(Enumerate), com_ext[com_num]);
				}
			}
		}
	}
}

void extension_stack_id(uint8_t com, const GetStackID *data) {
	if(!(chibi_type & CHIBI_TYPE_MASTER)) {
		return;
	}

	for(uint8_t com_num = 0; com_num < 2; com_num++) {
		if(com_ext[com_num] != COM_NONE) {
			for(uint8_t i = 0; i < CHIBI_NUM_SLAVE_ADDRESS; i++) {
				uint8_t slave_address = chibi_slave_address[i];
				if(slave_address == 0) {
					return;
				}

				uint16_t id;
				// TODO: if com_num = 2: from com_last_ext_id[1] to com_last_ext_id[2]
				for(id = com_last_spi_stack_id+1; id <= com_last_ext_id[com_num]; id++) {
					if(master_routing_table[id] == slave_address) {
						break;
					}
				}

				if(id <= com_last_ext_id[com_num]) {
					GetStackID gsid = {
						id,
						TYPE_GET_STACK_ID,
						sizeof(GetStackID),
						data->uid
					};
					send_blocking_with_timeout(&gsid, sizeof(GetStackID), com_ext[com_num]);
				}
			}
		}
	}
}
