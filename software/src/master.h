/* master-brick
 * Copyright (C) 2010-2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * master.h: Master specific functions
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

#ifndef MASTER_H
#define MASTER_H

#include <stdint.h>

#define MAX_STACK_IDS 256

#define MASTER_MODE_NONE 0
#define MASTER_MODE_SLAVE 1
#define MASTER_MODE_MASTER 2

void master_create_routing_table_rs485(const uint8_t extension);
uint8_t master_get_hardware_version(void);
void master_create_routing_table_chibi(const uint8_t extension);
void master_create_routing_table_extensions(void);
void master_init(void);
void new_connection(void);
void spi_connection_established(void);
void tick_task(const uint8_t tick_type);

#endif
