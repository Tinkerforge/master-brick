/* master-brick
 * Copyright (C) 2013 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_ringbuffer.h: WIFI ringbuffer implementation
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

#ifndef WIFI_RINGBUFFER_H
#define WIFI_RINGBUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define WIFI_RINGBUFFER_SIZE 1500

uint16_t wifi_ringbuffer_get_free(void);
bool wifi_ringbuffer_is_empty(void);
bool wifi_ringbuffer_is_full(void);
bool wifi_ringbuffer_add(const char data);
bool wifi_ringbuffer_get(char *data);
void wifi_ringbuffer_print(void);

#endif
