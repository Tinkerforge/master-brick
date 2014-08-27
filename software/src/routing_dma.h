/* master-brick
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * routing_dma.h: Master Brick specific routing functions (with DMA support)
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

#ifndef ROUTING_H
#define ROUTING_H

#include <stdint.h>
#include "bricklib/com/com.h"

#define ROUTING_TABLE_MAX_SIZE  64

#define ROUTING_STACK       1
#define ROUTING_EXTENSION_1 2
#define ROUTING_EXTENSION_2 3

typedef struct {
	uint8_t to;
	uint8_t option;
} RouteTo;

typedef struct {
	uint32_t uid;
	RouteTo route_to;
} RoutingTable;

RouteTo routing_route_to_fromto(const uint32_t uid, const uint8_t from, const uint8_t to);
RouteTo routing_route_to(const uint32_t uid);
RouteTo routing_route_stack_to(const uint32_t uid);
RouteTo routing_route_extension_to(const uint32_t uid);
void routing_add_route(const uint32_t uid, const RouteTo route_to);
void routing_table_create_stack(void);
void routing_master_from_pc(const char *data, const uint16_t length, const ComType com);

#endif
