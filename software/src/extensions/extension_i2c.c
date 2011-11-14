/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * extension_i2c.c: Implementation of Extension I2C protocol
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

#include "extension_i2c.h"

#include "bricklib/com/i2c/i2c_eeprom/i2c_eeprom_master.h"
#include "bricklib/com/i2c/i2c_eeprom/i2c_eeprom_common.h"
#include "bricklib/com/i2c/i2c_clear_bus.h"

#include <twi/twi.h>
#include <twi/twid.h>

Pin extension_select[] = {PIN_EXT_0_SELECT, PIN_EXT_1_SELECT};

void extension_i2c_init(void) {
	const Pin twi_pins[] = {PINS_TWI_STACK};

	// Configure TWI pins
	PIO_Configure(twi_pins, PIO_LISTSIZE(twi_pins));

	// Enable TWI peripheral clock
	PMC->PMC_PCER0 = 1 << ID_TWI1;

	// Configure TWI interrupts
	NVIC_DisableIRQ(TWI1_IRQn);
	NVIC_ClearPendingIRQ(TWI1_IRQn);
	NVIC_SetPriority(TWI1_IRQn, PRIORITY_EEPROM_MASTER_TWI1);
	NVIC_EnableIRQ(TWI1_IRQn);

    // Configure TWI as master
    TWI_ConfigureMaster(TWI_STACK, I2C_EEPROM_CLOCK, BOARD_MCK);
}

void extension_i2c_write_zeros(const uint8_t extension) {
	char data[32] = {0};

	for(uint8_t i = 0; i < 40; i++) {
		extension_i2c_select(extension);
		i2c_eeprom_master_write(TWI_STACK, i*32, data, 32);
		extension_i2c_deselect(extension);
	}
}

void extension_i2c_select(const uint8_t extension) {
	PIO_Clear(&extension_select[extension == 0 ? 1 : 1]);
	PIO_Set(&extension_select[extension]);
}

void extension_i2c_deselect(const uint8_t extension) {
	PIO_Clear(&extension_select[extension == 0 ? 1 : 1]);
	PIO_Clear(&extension_select[extension]);
}

bool extension_i2c_read(const uint8_t extension,
                        const uint16_t address,
                        char *data,
                        const uint16_t length) {
	extension_i2c_select(extension);
	bool ret = i2c_eeprom_master_read(TWI_STACK,
	                                  address,
	                                  data,
	                                  length);
	extension_i2c_deselect(extension);
	return ret;
}

bool extension_i2c_write(const uint8_t extension,
                         const uint16_t address,
                         char *data,
                         const uint16_t length) {
	extension_i2c_select(extension);
	bool ret = i2c_eeprom_master_write(TWI_STACK,
	                                   address,
	                                   data,
	                                   length);
	extension_i2c_deselect(extension);
	return ret;
}

void extension_i2c_clear_eeproms(void) {
	Pin sda = PIN_TWI_TWD_STACK;
	Pin scl = PIN_TWI_TWCK_STACK;
	for(uint8_t i = 0; i < 2; i++) {
		extension_i2c_select(i);
		i2c_clear_bus(&sda, &scl);
		extension_i2c_deselect(i);
	}
}
