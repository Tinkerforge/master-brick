/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet.h: High Level Ethernet protocol implementation
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

#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>
#include <stdbool.h>

void ethernet_init_extension(uint8_t extension);
bool ethernet_init(void);
uint16_t ethernet_send(const void *data, const uint16_t length);
uint16_t ethernet_recv(void *data, const uint16_t length);
void ethernet_message_loop(void *parameters);
void ethernet_message_loop_return(char *data, uint16_t length);

#endif
