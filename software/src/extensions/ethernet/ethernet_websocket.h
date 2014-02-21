/* master-brick
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_websocket.h: Ethernet websocket implementation
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

#ifndef ETHERNET_WEBSOCKET_H
#define ETHERNET_WEBSOCKET_H

#include <stdint.h>

void ethernet_websocket_init(const uint8_t socket);
uint8_t ethernet_websocket_read_data_tcp(const uint8_t socket, uint8_t *buffer, const uint8_t length);
uint8_t ethernet_websocket_write_data_tcp(const uint8_t socket, const uint8_t *buffer, const uint8_t length);

#endif
