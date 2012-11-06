/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * routing.c: Master Brick specific routing functions
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

#include "routing.h"

#include "bricklib/com/spi/spi_stack/spi_stack_common.h"
#include "bricklib/com/spi/spi_stack/spi_stack_select.h"
#include "bricklib/com/spi/spi_stack/spi_stack_master.h"
#include "bricklib/com/com_messages.h"
#include "bricklib/utility/util_definitions.h"

extern uint16_t spi_stack_buffer_size_recv;
extern uint16_t spi_stack_buffer_size_send;
extern uint8_t master_mode;
extern ComType com_ext[];

RoutingTable routing_table[ROUTING_TABLE_SIZE] = {{0, 0}};
uint8_t routing_table_max = 0;
uint8_t routing_stack_address_max = 0;


void routing_table_create_stack(void) {
	uint8_t stack_address = 0;
	uint8_t tries = 0;
	while(stack_address <= 8) {
		StackEnumerate se;
		com_make_default_header(&se, 0, FID_STACK_ENUMERATE, sizeof(StackEnumerate));

		if(spi_stack_send(&se, sizeof(StackEnumerate)) != 0) {
			spi_stack_select(stack_address + 1);

			tries = 0;
			while(!spi_stack_master_transceive() && tries < 10) {
				SLEEP_MS(50);
				tries++;
			}
			if(tries == 10) {
				break;
			}

			spi_stack_deselect();

			spi_stack_buffer_size_recv = 0;
		}

		StackEnumerateReturn ser;
		tries = 0;
		while(tries < 10) {
			SLEEP_MS(50);
			spi_stack_select(stack_address + 1);
			spi_stack_master_transceive();
			spi_stack_deselect();
			if(spi_stack_recv(&ser, sizeof(StackEnumerateReturn))) {
				break;
			}
			tries++;
		}

		if(tries == 10) {
			logspise("Did not receive answer for Stack Enumerate\n\r");
			break;
		}

		stack_address++;
		routing_stack_address_max = stack_address;

		for(uint8_t i = 0; i < STACK_ENUMERATE_MAX_UIDS; i++) {
			if(ser.uids[i] != 0) {
				if(routing_table_max >= ROUTING_TABLE_SIZE) {
					break;
				}
				routing_table[routing_table_max].uid = ser.uids[i];
				routing_table[routing_table_max].route_to = stack_address;
				routing_table_max++;
				logspisi("New Stack participant (sa, uid): %d, %d\n\r", stack_address, ser.uids[i]);
			}
		}
	}

	spi_stack_buffer_size_send = 0;
}

uint8_t routing_route_to(uint32_t uid) {
	for(uint8_t i = 0; i < routing_table_max; i++) {
		if(routing_table[i].uid == uid) {
			return routing_table[i].route_to;
		}
	}

	return 0;
}

void routing_master_from_pc(char *data, uint16_t length) {
	uint32_t uid = ((MessageHeader*)data)->uid;
	// Broadcast
	if(uid == 0) {
		for(uint8_t i = 0; i < routing_stack_address_max; i++) {
			send_blocking_with_timeout(data, length, COM_SPI_STACK);
		}
		send_blocking_with_timeout(data, length, com_ext[0]);
		send_blocking_with_timeout(data, length, com_ext[1]);
	// Discover Route
	} else {
		uint8_t route_to = routing_route_to(uid);
		if(route_to >= ROUTING_STACK_MIN && route_to <= ROUTING_STACK_MAX) {
			send_blocking_with_timeout(data, length, COM_SPI_STACK);
		} else if(master_mode & MASTER_MODE_MASTER) {
			if(route_to == ROUTING_EXTENSION_1) {
				send_blocking_with_timeout(data, length, com_ext[0]);
			} else if(route_to == ROUTING_EXTENSION_2) {
				send_blocking_with_timeout(data, length, com_ext[1]);
			} else {
				send_blocking_with_timeout(data, length, com_ext[0]);
				send_blocking_with_timeout(data, length, com_ext[1]);
			}
		}
	}
}
