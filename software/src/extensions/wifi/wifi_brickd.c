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
#include "config.h"
#include "wifi_config.h"


WifiRouting *wifi_brickd_routing_table_start;
WifiRouting wifi_brickd_routing_table[WIFI_BRICKD_ROUTING_TABLE_SIZE];

uint32_t wifi_brickd_counter = 0;

void wifi_brickd_init(void) {
	for(uint8_t i = 0; i < WIFI_BRICKD_ROUTING_TABLE_SIZE; i++) {
		wifi_brickd_routing_table[i].stack_id = 0;
		wifi_brickd_routing_table[i].cid = -1;
		wifi_brickd_routing_table[i].counter = 0;
	}

/*	wifi_brickd_routing_table_start = &wifi_brickd_routing_table[0];
	for(uint8_t i = 1; i < WIFI_BRICKD_ROUTING_TABLE_SIZE; i++) {
		wifi_brickd_routing_table[i-1].wifi_routing = &wifi_brickd_routing_table[i];
	}*/
}

uint32_t wifi_brickd_counter_diff(uint32_t new, uint32_t old) {
	if(new > old) {
		return new - old;
	}

	return new + (0xFFFFFFFF - old);
}

// TODO: same stackid/cid two times possible, use smallest counter one!
void wifi_brickd_route_from(const uint8_t *data, const uint8_t cid) {
	WifiRouting *smallest;
	wifi_brickd_counter++;
	uint32_t diff = 0;

	smallest = &wifi_brickd_routing_table[0];
	for(uint8_t i = 0; i < 10; i++) {
		if(wifi_brickd_routing_table[i].cid == -1) {
			wifi_brickd_routing_table[i].stack_id = data[0];
			wifi_brickd_routing_table[i].cid = cid;
			wifi_brickd_routing_table[i].counter = wifi_brickd_counter;
			return;
		} else {
			uint32_t new_diff = wifi_brickd_counter_diff((*smallest).counter, wifi_brickd_routing_table[i].counter);
			if(new_diff > diff) {
				smallest = & wifi_brickd_routing_table[i];
				diff = new_diff;
			}
		}
	}

	(*smallest).stack_id = data[0];
	(*smallest).cid = cid;
	(*smallest).counter = wifi_brickd_counter;
	return;
}

int8_t wifi_brickd_route_to(const uint8_t *data) {
	for(uint8_t i = 0; i < WIFI_BRICKD_ROUTING_TABLE_SIZE; i++) {
		if(wifi_brickd_routing_table[i].stack_id == data[0]) {
			int8_t cid = wifi_brickd_routing_table[i].cid;
			wifi_brickd_routing_table[i].cid = 0;
			return cid;
		}
	}

	return -1;
}
