/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_low_level.h: Low level WIFI protocol implementation
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


#ifndef WIFI_LOW_LEVEL_H
#define WIFI_LOW_LEVEL_H

#include <stdint.h>
#include <stdbool.h>

#include "wifi_config.h"

#define WIFI_LOW_LEVEL_SPI_ESC_CHAR               0xFB
#define WIFI_LOW_LEVEL_SPI_IDLE_CHAR              0xF5
#define WIFI_LOW_LEVEL_SPI_XOFF_CHAR              0xFA
#define WIFI_LOW_LEVEL_SPI_XON_CHAR               0xFD
#define WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ONE   0xFF
#define WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ZERO  0x00
#define WIFI_LOW_LEVEL_SPI_LINK_READY             0xF3

bool wifi_low_level_is_byte_stuffing(char value);
char wifi_low_level_write_byte(char b);
uint8_t wifi_low_level_write_byte_stuffing(char b, char *ret1, char *ret2);
char wifi_low_level_read_byte(void);
void wifi_low_level_read_buffer(char *buffer, const uint8_t length);
void wifi_low_level_write_buffer(const char *buffer, const uint8_t length);
void wifi_low_level_select(void);
void wifi_low_level_deselect(void);

#endif
