/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_brickd.h: Simplistic reimplementation of brickd for WIFI Extension
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

#ifndef BRICKD_H
#define BRICKD_H

#include <stdint.h>
#include <stdbool.h>

#define BRICKD_ROUTING_TABLE_SIZE 10

typedef struct BrickdRouting BrickdRouting;

struct BrickdRouting {
	uint32_t uid;
	uint32_t counter;
	uint8_t func_id;
	uint8_t seqence_number;
	int8_t cid;
};

void brickd_init(void);
uint32_t brickd_counter_diff(const uint32_t new, const uint32_t old);
void brickd_route_from(const void *data, const uint8_t cid);
int8_t brickd_route_to(const void *data);
void brickd_disconnect(const uint8_t cid);

#endif
