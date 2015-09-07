/* master-brick
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi2_bootloader.h: WIFI2 bootloader communication with ESP8266
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

#ifndef WIFI2_BOOTLOADER_H
#define WIFI2_BOOTLOADER_H

#include <stdint.h>
#include <stdbool.h>

int8_t wifi2_bootloader_start(void);
int8_t wifi2_bootloader_write(uint8_t *data, uint8_t length);
uint8_t wifi2_bootloader_read(uint8_t *data, uint8_t length);

#endif
