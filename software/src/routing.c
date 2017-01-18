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
#include "bricklib/com/com.h"
#include "extensions/brickd.h"
#include "bricklib/utility/util_definitions.h"

extern uint16_t spi_stack_buffer_size_recv;
extern uint16_t spi_stack_buffer_size_send;
extern uint8_t master_mode;
extern ComInfo com_info;

RoutingTable routing_table[ROUTING_TABLE_MAX_SIZE] = {{0, {0, 0}}};
uint8_t routing_table_size = 0;
uint8_t routing_table_last_stack = 0;

void routing_table_create_stack(void) {
	uint8_t stack_address = 0;
	uint8_t tries = 0;
	while(stack_address < SPI_ADDRESS_MAX) {
		StackEnumerate se;
		com_make_default_header(&se, 0, sizeof(StackEnumerate), FID_STACK_ENUMERATE);

		uint32_t options = stack_address + 1;
		if(spi_stack_send(&se, sizeof(StackEnumerate), &options) != 0) {
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
			if(spi_stack_recv(&ser, sizeof(StackEnumerateReturn), NULL)) {
				break;
			}
			tries++;
		}

		if(tries == 10) {
			logspise("Did not receive answer for Stack Enumerate\n\r");
			break;
		}

		stack_address++;
		com_info.last_stack_address = stack_address;

		for(uint8_t i = 0; i < STACK_ENUMERATE_MAX_UIDS; i++) {
			if(ser.uids[i] != 0) {
				if(routing_table_size >= ROUTING_TABLE_MAX_SIZE) {
					break;
				}
				routing_table[routing_table_size].uid = ser.uids[i];
				routing_table[routing_table_size].route_to.to = ROUTING_STACK;
				routing_table[routing_table_size].route_to.option = stack_address;
				routing_table_last_stack = routing_table_size;
				routing_table_size++;
				logspisi("New Stack participant (sa, uid): %d, %lu\n\r", stack_address, ser.uids[i]);
			}
		}
	}

	spi_stack_buffer_size_send = 0;
}

RouteTo routing_route_to_fromto(const uint32_t uid, const uint8_t from, const uint8_t to) {
	for(uint8_t i = from; i < MIN(to, ROUTING_TABLE_MAX_SIZE); i++) {
		if(routing_table[i].uid == uid) {
			return routing_table[i].route_to;
		}
	}

	RouteTo no_route = {0, 0};

	return no_route;
}

RouteTo routing_route_to(const uint32_t uid) {
	return routing_route_to_fromto(uid, 0, routing_table_size);
}

RouteTo routing_route_stack_to(const uint32_t uid) {
	return routing_route_to_fromto(uid, 0, routing_table_last_stack);
}

RouteTo routing_route_extension_to(const uint32_t uid) {
	return routing_route_to_fromto(uid, routing_table_last_stack, routing_table_size);
}

void routing_add_route(const uint32_t uid, const RouteTo route_to) {
	if(routing_table_size < ROUTING_TABLE_MAX_SIZE) {
		routing_table[routing_table_size].uid = uid;
		routing_table[routing_table_size].route_to = route_to;
		logi("Add to route %d: %lu -> (%d, %d)\n\r", routing_table_size, uid, route_to.to, route_to.option);
		routing_table_size++;
	}
}

void routing_master_from_pc(const char *data, const uint16_t length, const ComType com) {
	uint32_t uid = ((MessageHeader*)data)->uid;

	// Broadcast
	if(uid == 0) {
		for(uint8_t i = 1; i <= com_info.last_stack_address; i++) {
			uint32_t options = i;
			send_blocking_with_timeout_options(data, length, COM_SPI_STACK, &options);
		}

		if(com_info.ext_type[0] == COM_TYPE_MASTER) {
			send_blocking_with_timeout(data, length, com_info.ext[0]);
		}
		if(com_info.ext_type[1] == COM_TYPE_MASTER) {
			send_blocking_with_timeout(data, length, com_info.ext[1]);
		}
	// Message for brickd
	} else if(uid == 1) {
		if(com == COM_ETHERNET || com == COM_WIFI) {
			MessageHeader *header = (MessageHeader*)data;
			switch(header->fid) {
				case BRICKD_FID_GET_AUTHENTICATION_NONCE: {
					brickd_get_authentication_nonce(com, (GetAuthenticationNonce*)data);
					break;
				}

				case BRICKD_FID_AUTHENTICATE: {
					brickd_authenticate(com, (Authenticate*)data);
					break;
				}

				default: {
					com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com);
					break;
				}
			}
		}
	// Discover Route
	} else {
		const RouteTo route_to = routing_route_to(uid);
		if(route_to.to == ROUTING_STACK) {
			uint32_t options = route_to.option;
			send_blocking_with_timeout_options(data, length, COM_SPI_STACK, &options);
		} else if(master_mode & MASTER_MODE_MASTER) {
			if(route_to.to == ROUTING_EXTENSION_1) {
				if(com_info.ext_type[0] == COM_TYPE_MASTER) {
					send_blocking_with_timeout(data, length, com_info.ext[0]);
				}
			} else if(route_to.to == ROUTING_EXTENSION_2) {
				if(com_info.ext_type[1] == COM_TYPE_MASTER) {
					send_blocking_with_timeout(data, length, com_info.ext[1]);
				}
			} else {
				if(com_info.ext_type[0] == COM_TYPE_MASTER) {
					send_blocking_with_timeout(data, length, com_info.ext[0]);
				}
				if(com_info.ext_type[1] == COM_TYPE_MASTER) {
					send_blocking_with_timeout(data, length, com_info.ext[1]);
				}
			}
		}
	}
}
