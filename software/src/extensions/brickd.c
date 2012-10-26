/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * brickd.c: Simplistic reimplementation of brickd for WIFI Extension
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

#include "brickd.h"

#include "config.h"

#include <stddef.h>

BrickdRouting brickd_routing_table[BRICKD_ROUTING_TABLE_SIZE];

uint32_t brickd_counter = 0;

void brickd_init(void) {
	for(uint8_t i = 0; i < BRICKD_ROUTING_TABLE_SIZE; i++) {
		brickd_routing_table[i].stack_id = 0;
		brickd_routing_table[i].func_id = 0;
		brickd_routing_table[i].cid = -1;
		brickd_routing_table[i].counter = 0;
	}
}

uint32_t brickd_counter_diff(uint32_t new, uint32_t old) {
	if(new > old) {
		return new - old;
	}

	return new + (0xFFFFFFFF - old);
}

void brickd_route_from(const uint8_t *data, const uint8_t cid) {
	// Broadcast
	if(data[0] == 0) {
		return;
	}

	BrickdRouting *smallest = &brickd_routing_table[0];
	brickd_counter++;
	uint32_t diff = 0;

	for(uint8_t i = 0; i < 10; i++) {
		if(brickd_routing_table[i].cid == -1) {
			brickd_routing_table[i].stack_id = data[0];
			brickd_routing_table[i].func_id = data[1];
			brickd_routing_table[i].cid = cid;
			brickd_routing_table[i].counter = brickd_counter;
			return;
		} else {
			uint32_t new_diff = brickd_counter_diff((*smallest).counter, brickd_routing_table[i].counter);
			if(new_diff > diff) {
				smallest = &brickd_routing_table[i];
				diff = new_diff;
			}
		}
	}

	smallest->stack_id = data[0];
	smallest->func_id = data[1];
	smallest->cid = cid;
	smallest->counter = brickd_counter;
}

int8_t brickd_route_to(const uint8_t *data) {
	BrickdRouting *current_match = NULL;
	uint32_t current_diff = 0;

	for(uint8_t i = 0; i < BRICKD_ROUTING_TABLE_SIZE; i++) {
		if(brickd_routing_table[i].stack_id == data[0] &&
		   brickd_routing_table[i].func_id == data[1]) {
			uint32_t new_diff = brickd_counter_diff(current_match->counter, brickd_counter);
			if(new_diff > current_diff) {
				new_diff = current_diff;
				current_match = &brickd_routing_table[i];
			}
		}
	}

	if(current_match != NULL) {
		int8_t cid = current_match->cid;
		current_match->stack_id = 0;
		current_match->func_id = 0;
		current_match->cid = -1;

		return cid;
	}

	return -1;
}

void brickd_disconnect(uint8_t cid) {
	if(cid > 0 && cid < 16) {
		for(uint8_t i = 1; i < BRICKD_ROUTING_TABLE_SIZE; i++) {
			if(brickd_routing_table[i].cid == cid) {
				brickd_routing_table[i].cid = -1;
				brickd_routing_table[i].func_id = 0;
				brickd_routing_table[i].stack_id = 0;
			}
		}
	}
}
