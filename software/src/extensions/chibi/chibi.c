/* master-brick
 * Copyright (C) 2011-2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi.c: High-level chibi protocol implementation
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

#include "chibi.h"
#include "chibi_low_level.h"
#include "routing.h"

#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/drivers/pio/pio_it.h"

#include "bricklib/com/com_messages.h"

#include "bricklib/utility/util_definitions.h"
#include "bricklib/logging/logging.h"

#include "bricklib/free_rtos/include/FreeRTOS.h"
#include "bricklib/free_rtos/include/task.h"

#include <string.h>
#include <stdbool.h>

extern uint8_t chibi_buffer_recv[];
extern uint8_t chibi_buffer_size_recv;

static uint8_t chibi_sequence_number = 0;
uint8_t chibi_address = 0;
uint8_t chibi_slave_address[CHIBI_NUM_SLAVE_ADDRESS] = {0};
uint8_t chibi_master_address = 0;
extern uint8_t chibi_type;
extern uint32_t led_rxtx;

bool chibi_init(void) {
	// Enable peripheral clock
	PMC->PMC_PCER0 = 1 << ID_USART0;

	// Configure the USART0 as SPI
	USART_Configure(USART0,
					US_MR_USART_MODE_SPI_MASTER |
					US_MR_USCLKS_MCK            |
					US_MR_CHRL_8_BIT            |
					US_MR_PAR_NO                |
					US_MR_CHMODE_NORMAL         |
					US_MR_CLKO                  |
					US_SPI_CPOL_0               |
					US_SPI_CPHA_1,
					CHIBI_USART_SPI_CLOCK,
					BOARD_MCK);

	// Enable receiver and transmitter
	USART0->US_CR = US_CR_TXEN;
	USART0->US_CR = US_CR_RXEN;

	return true;
}

uint16_t chibi_send_broadcast(const void *data, const uint16_t length) {
	if(chibi_type != CHIBI_TYPE_MASTER) {
		return length;
	}

	for(uint8_t i = 0; i < CHIBI_NUM_SLAVE_ADDRESS; i++) {
		if(chibi_slave_address[i] == 0) {
			return length;
		}
		uint32_t options = chibi_slave_address[i];
		send_blocking_with_timeout_options(data, length, COM_CHIBI, &options);
	}

	return length;
}

uint16_t chibi_send(const void *data, const uint16_t length, uint32_t *options) {
	uint8_t send_length = MIN(length, CHIBI_MAX_DATA_LENGTH);

	uint8_t receiver_address = 0;
	if(chibi_type == CHIBI_TYPE_MASTER) {
		if(((MessageHeader*)data)->uid == 0) {
			if(options == NULL) {
				return chibi_send_broadcast(data, length);
			}
		}

		if(options != NULL) {
			receiver_address = *options;
		} else {
			const uint32_t uid = ((MessageHeader*)data)->uid;
			RouteTo route_to = routing_route_extension_to(uid);
			if(route_to.option == 0) {
				return chibi_send_broadcast(data, length);
			} else {
				receiver_address = route_to.option;
			}
		}
	} else if(chibi_type == CHIBI_TYPE_SLAVE) {
		receiver_address =  chibi_master_address;
	}

	if(receiver_address != 0) {
		ChibiHeaderMPDU ch = {
			CHIBI_MDPU_DATA               |
			CHIBI_MDPU_ACK_REQUEST        |
			CHIBI_MDPU_PAN_COMPRESSION    |
			CHIBI_MDPU_DEST_SHORT_ADDRESS |
			CHIBI_MDPU_FRAME_VERSION_06   |
			CHIBI_MDPU_SRC_SHORT_ADDRESS,
			chibi_sequence_number++,
			CHIBI_PAN_ID,
			receiver_address,
			chibi_address
		};

		uint8_t chibi_ret = chibi_transfer((uint8_t*)&ch, data, send_length);
		if(chibi_ret == CHIBI_ERROR_OK) {
			led_rxtx++;
			return send_length;
		} else {
			led_rxtx++;
			return 0;
		}
	} else {
		return send_length;
	}
}

uint16_t chibi_recv(void *data, const uint16_t length, uint32_t *options) {
	if(chibi_buffer_size_recv == 0) {
		return 0;
	}

	static uint16_t recv_pointer = 0;

	uint16_t recv_length = MIN(length, chibi_buffer_size_recv);

	memcpy(data, chibi_buffer_recv + recv_pointer, recv_length);

	if(chibi_buffer_size_recv - recv_length == 0) {
		recv_pointer = 0;
	} else {
		recv_pointer += recv_length;
	}

	chibi_buffer_size_recv -= recv_length;

	led_rxtx++;

	return recv_length;
}

bool chibi_add_enumerate_connected_request(void) {
	__disable_irq();
	if(chibi_buffer_size_recv != 0) {
		__enable_irq();
		return false;
	}

	com_make_default_header(chibi_buffer_recv, 0, sizeof(Enumerate), FID_CREATE_ENUMERATE_CONNECTED);
	chibi_buffer_size_recv = sizeof(Enumerate);
	__enable_irq();

	return true;
}
