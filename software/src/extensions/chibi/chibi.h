/* master-brick
 * Copyright (C) 2011-2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi.h: High-level chibi protocol implementation
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

#ifndef CHIBI_H
#define CHIBI_H

#include <stdint.h>
#include <stdbool.h>

uint16_t chibi_send_broadcast(const void *data, const uint16_t length);
bool chibi_init(void);
uint16_t chibi_send(const void *data, const uint16_t length, uint32_t *options);
uint16_t chibi_recv(void *data, const uint16_t length, uint32_t *options);

#endif
