/* master-brick
 * Copyright (C) 2011-2012 Olaf Lüke <olaf@tinkerforge.com>
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

#include "extensions/rs485/rs485.h"
#include "extensions/rs485/rs485_config.h"
#include "extensions/rs485/rs485_master.h"
#include "extensions/rs485/rs485_slave.h"

#include "extensions/wifi/wifi.h"
#include "extensions/ethernet/ethernet.h"

#include <stdio.h>

extern ComType com_ext[];

/*extern uint8_t chibi_slave_address[];
extern uint8_t chibi_type;
extern uint8_t rs485_slave_address[];
extern uint8_t rs485_type;*/

Pin extension_pins[] = {PIN_EXT_0_SELECT,
		                PIN_EXT_0_GP_0,
		                PIN_EXT_0_GP_1,
		                PIN_EXT_0_GP_2,
		                PIN_EXT_1_SELECT,
					    PIN_EXT_1_GP_0,
					    PIN_EXT_1_GP_1,
					    PIN_EXT_1_GP_2,
					    PIN_EXT_RXD,
					    PIN_EXT_TXD,
					    PIN_EXT_SCK};

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
	PIO_Configure(extension_pins, PIO_LISTSIZE(extension_pins));

	extension_i2c_init();
	for(uint8_t i = 0; i < 2; i++) {
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
					rs485_init_masterslave(i);

					break;
				}

				case EXTENSION_TYPE_WIFI: {
					com_ext[i] = COM_WIFI;
					wifi_init_extension(i);

					break;
				}

				case EXTENSION_TYPE_ETHERNET: {
					com_ext[i] = COM_ETHERNET;
					ethernet_init_extension(i);

					break;
				}

				case EXTENSION_TYPE_NONE:
				default: {
					com_ext[i] = COM_NONE;
					break;
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

void extension_enumerate_chibi(uint8_t com, const Enumerate *data, uint8_t com_num) {
/*	if(!(chibi_type & CHIBI_TYPE_MASTER)) {
		return;
	}

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
		logchibii("Extension Enumerate id %d %d\n\r", id, com_last_ext_id[com_num]);

		if(id <= com_last_ext_id[com_num]) {
			Enumerate e = {
				id,
				FID_ENUMERATE,
				sizeof(Enumerate),
			};
			send_blocking_with_timeout(&e, sizeof(Enumerate), com_ext[com_num]);
		}
	}*/
}

void extension_enumerate_rs485(uint8_t com, const Enumerate *data, uint8_t com_num) {
/*	if(!(rs485_type & RS485_TYPE_MASTER)) {
		return;
	}

	for(uint8_t i = 0; i < RS485_NUM_SLAVE_ADDRESS; i++) {
		uint8_t slave_address = rs485_slave_address[i];
		logrsi("Extension Enumerate %d\n\r", slave_address);
		if(slave_address == 0) {
			return;
		}

		uint16_t id;
		for(id = com_last_spi_stack_id+1; id <= com_last_ext_id[com_num]; id++) {
			if(master_routing_table[id] == slave_address) {
				break;
			}
		}
		logrsi("Extension Enumerate id %d %d\n\r", id, com_last_ext_id[com_num]);

		if(id <= com_last_ext_id[com_num]) {
			Enumerate e = {
				id,
				FID_ENUMERATE,
				sizeof(Enumerate),
			};
			send_blocking_with_timeout(&e, sizeof(Enumerate), com_ext[com_num]);
		}
	}*/
}

void extension_enumerate(uint8_t com, const Enumerate *data) {
	for(uint8_t com_num = 0; com_num < 2; com_num++) {
		switch(com_ext[com_num]) {
			case COM_CHIBI: {
				extension_enumerate_chibi(com, data, com_num);
				break;
			}
			case COM_RS485: {
				extension_enumerate_rs485(com, data, com_num);
				break;
			}
			default: {
				break;
			}
		}
	}
}
