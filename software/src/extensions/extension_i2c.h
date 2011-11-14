/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * extension_i2c.h: Implementation of Extension I2C protocol
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

#ifndef EXTENSION_I2C_H
#define EXTENSION_I2C_H

#include <stdint.h>
#include <stdbool.h>

void extension_i2c_init(void);
void extension_i2c_select(const uint8_t extension);
void extension_i2c_deselect(const uint8_t extension);
bool extension_i2c_read(const uint8_t extension,
                        const uint16_t address,
                        char *data,
                        const uint16_t length);
bool extension_i2c_write(const uint8_t extension,
                         const uint16_t address,
                         char *data,
                         const uint16_t length);
void extension_i2c_write_zeros(const uint8_t extension);
void extension_i2c_clear_eeproms(void);

#endif
