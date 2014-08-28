/* master-brick
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * routing_dma.c: Master Brick specific routing functions (with DMA support)
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

#include "bricklib/com/spi/spi_stack/spi_stack_common_dma.h"
#include "bricklib/com/spi/spi_stack/spi_stack_select.h"
#include "bricklib/com/spi/spi_stack/spi_stack_master_dma.h"
#include "bricklib/com/com_messages.h"
#include "bricklib/com/com.h"
#include "extensions/brickd.h"
#include "bricklib/utility/util_definitions.h"

extern uint16_t spi_stack_buffer_size_recv;
extern uint16_t spi_stack_buffer_size_send;
extern uint8_t spi_stack_buffer_recv[SPI_STACK_MAX_MESSAGE_LENGTH];
extern uint8_t spi_stack_buffer_send[SPI_STACK_MAX_MESSAGE_LENGTH];
extern uint8_t master_mode;
extern ComInfo com_info;
extern SPIStackMasterSlaveStatus slave_status[];
extern SPIStackMasterTransceiveState transceive_state;

RoutingTable routing_table[ROUTING_TABLE_MAX_SIZE] = {{0, {0, 0}}};
uint8_t routing_table_size = 0;
uint8_t routing_table_last_stack = 0;

uint8_t routing_table_stack_enumerate(uint8_t stack_address) {
	StackEnumerate se;
	com_make_default_header(&se, 0, sizeof(StackEnumerate), FID_STACK_ENUMERATE);
	bool transceive_ok = false;

	uint8_t tries;
	for(tries = 0; tries < 10; tries++) {
		SPIStackMasterTransceiveInfo ti;
		if(!transceive_ok) {
			ti = spi_stack_master_start_transceive((uint8_t *)&se, sizeof(StackEnumerate), stack_address);
			//spi_stack_buffer_size_recv = 0; // We don't care if we received something, throw it away
		} else {
			ti = spi_stack_master_start_transceive(NULL, 0, stack_address);
		}
		SLEEP_MS(50);

		switch(ti) {
			case TRANSCEIVE_INFO_SEND_OK:
				return tries;

				break;

			case TRANSCEIVE_INFO_SEND_EMPTY_MESSAGE:
			case TRANSCEIVE_INFO_SEND_NOTHING_BUSY:
			case TRANSCEIVE_INFO_SEND_ERROR:
			default:
				break;
		}

	}

	return tries;
}

uint8_t routing_table_stack_enumerate_return(uint8_t stack_address, StackEnumerateReturn *ser) {
	uint8_t tries;
	for(tries = 0; tries < 10; tries++) {
		if(spi_stack_buffer_size_recv > 0) {
			spi_stack_buffer_size_recv = 0;
			for(uint8_t i = 0; i < sizeof(StackEnumerateReturn); i++) {
				((uint8_t*)ser)[i] = spi_stack_buffer_recv[i];
			}

			// Do a sanity check to find out if it can be the right message
			if(ser->header.length == sizeof(StackEnumerateReturn)) {
				return tries;
			}
		}
		spi_stack_master_start_transceive(NULL, 0, stack_address);
		SLEEP_MS(50);
	}

	return tries;
}

void routing_table_create_stack(void) {
	uint8_t stack_address = 0;
	uint8_t tries = 0;
	while(stack_address < SPI_ADDRESS_MAX) {
		// Lets assume the next slave is available and see if we get an answer
		slave_status[stack_address] = SLAVE_STATUS_AVAILABLE;

		tries = routing_table_stack_enumerate(stack_address+1);
		if(tries == 10) {
			// We couldn't send data to this stack participant, we are done
			// with searching for more slaves
			break;
		}

		StackEnumerateReturn ser;
		tries = routing_table_stack_enumerate_return(stack_address+1, &ser);

		if(tries == 10) {
			slave_status[stack_address] = SLAVE_STATUS_ABSENT;
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

	if(stack_address <= SPI_ADDRESS_MAX) {
		slave_status[stack_address] = SLAVE_STATUS_ABSENT;
	}

	// Make sure that there is no valid data and that nothing ought to be send
	spi_stack_buffer_size_recv = 0;
	spi_stack_buffer_size_send = 0;
	spi_stack_buffer_recv[0] = 0;
	spi_stack_buffer_send[0] = 0;
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
