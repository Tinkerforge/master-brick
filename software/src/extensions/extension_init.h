/* master-brick
 * Copyright (C) 2011-2013 Olaf Lüke <olaf@tinkerforge.com>
 *
 * extension_init.h: Implementation of Extension initialization
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


#ifndef EXTENSION_INIT_H
#define EXTENSION_INIT_H

#include <stdint.h>
#include <stdbool.h>
#include "bricklib/com/com_messages.h"

uint8_t extension_get_slave_address(const uint8_t extension,
                                       const uint8_t num);
void extension_set_slave_address(const uint8_t extension,
                                    const uint8_t num,
                                    uint8_t address);
uint8_t extension_get_master_address(const uint8_t extension);
void extension_set_master_address(const uint8_t extension, uint8_t address);
uint8_t extension_get_address(const uint8_t extension);
void extension_set_address(const uint8_t extension, uint8_t address);

uint32_t extension_get_type(const uint8_t extension);
void extension_set_type(const uint8_t extension, uint32_t type);
bool extension_is_present(const uint8_t extension);
void extension_init(void);

void extension_enumerate(uint8_t com, const Enumerate *data);

#endif
