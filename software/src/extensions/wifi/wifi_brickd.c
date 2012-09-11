/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_brickd.c: Simplistic reimplementation of brickd for WIFI Extension
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

#include "wifi_brickd.h"

#include "wifi.h"
#include "wifi_data.h"
#include "config.h"
#include "wifi_config.h"

#include <stddef.h>

WifiRouting wifi_brickd_routing_table[WIFI_BRICKD_ROUTING_TABLE_SIZE];

uint32_t wifi_brickd_counter = 0;

void wifi_brickd_init(void) {
	for(uint8_t i = 0; i < WIFI_BRICKD_ROUTING_TABLE_SIZE; i++) {
		wifi_brickd_routing_table[i].stack_id = 0;
		wifi_brickd_routing_table[i].func_id = 0;
		wifi_brickd_routing_table[i].cid = -1;
		wifi_brickd_routing_table[i].counter = 0;
	}
}

uint32_t wifi_brickd_counter_diff(uint32_t new, uint32_t old) {
	if(new > old) {
		return new - old;
	}

	return new + (0xFFFFFFFF - old);
}

void wifi_brickd_route_from(const uint8_t *data, const uint8_t cid) {
	// Broadcast
	if(data[0] == 0) {
		return;
	}

	WifiRouting *smallest = &wifi_brickd_routing_table[0];
	wifi_brickd_counter++;
	uint32_t diff = 0;

	for(uint8_t i = 0; i < 10; i++) {
		if(wifi_brickd_routing_table[i].cid == -1) {
			wifi_brickd_routing_table[i].stack_id = data[0];
			wifi_brickd_routing_table[i].func_id = data[1];
			wifi_brickd_routing_table[i].cid = cid;
			wifi_brickd_routing_table[i].counter = wifi_brickd_counter;
			return;
		} else {
			uint32_t new_diff = wifi_brickd_counter_diff((*smallest).counter, wifi_brickd_routing_table[i].counter);
			if(new_diff > diff) {
				smallest = &wifi_brickd_routing_table[i];
				diff = new_diff;
			}
		}
	}

	smallest->stack_id = data[0];
	smallest->func_id = data[1];
	smallest->cid = cid;
	smallest->counter = wifi_brickd_counter;
}

int8_t wifi_brickd_route_to(const uint8_t *data) {
	WifiRouting *current_match = NULL;
	uint32_t current_diff = 0;

	for(uint8_t i = 0; i < WIFI_BRICKD_ROUTING_TABLE_SIZE; i++) {
		if(wifi_brickd_routing_table[i].stack_id == data[0] &&
		   wifi_brickd_routing_table[i].func_id == data[1]) {
			uint32_t new_diff = wifi_brickd_counter_diff(current_match->counter, wifi_brickd_counter);
			if(new_diff > current_diff) {
				new_diff = current_diff;
				current_match = &wifi_brickd_routing_table[i];
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

void wifi_brickd_disconnect(uint8_t cid) {
	if(cid > 0 && cid < 16) {
		for(uint8_t i = 1; i < WIFI_BRICKD_ROUTING_TABLE_SIZE; i++) {
			if(wifi_brickd_routing_table[i].cid == cid) {
				wifi_brickd_routing_table[i].cid = -1;
				wifi_brickd_routing_table[i].func_id = 0;
				wifi_brickd_routing_table[i].stack_id = 0;
			}
		}
	}
}
