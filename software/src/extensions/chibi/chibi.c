/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
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

#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/drivers/pio/pio_it.h"

#include "bricklib/utility/util_definitions.h"
#include "bricklib/logging/logging.h"

#include <FreeRTOS.h>
#include <task.h>

#include <string.h>
#include <stdbool.h>

extern uint8_t chibi_buffer_recv[];
extern uint8_t chibi_buffer_size_recv;
extern bool chibi_transfer_end;
//extern uint8_t master_routing_table[];

static uint8_t chibi_sequence_number = 0;
uint8_t chibi_address = 0;
uint8_t chibi_slave_address[CHIBI_NUM_SLAVE_ADDRESS] = {0};
uint8_t chibi_master_address = 0;
extern uint8_t chibi_type;

uint8_t chibi_get_receiver_address(uint8_t stack_id) {
	if(chibi_type == CHIBI_TYPE_MASTER) {
		// TODO Protocol V2.0
		return 0;
//		return master_routing_table[stack_id];
	} else if(chibi_type == CHIBI_TYPE_SLAVE) {
		return chibi_master_address;
	}

	return 0;
}

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

uint16_t chibi_send(const void *data, const uint16_t length) {
	uint8_t send_length = MIN(length, CHIBI_MAX_DATA_LENGTH);

	const uint8_t stack_id = ((uint8_t*)data)[0];
	uint8_t receiver_address = chibi_get_receiver_address(stack_id);

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

	if(chibi_transfer((uint8_t*)&ch, data, send_length) == CHIBI_ERROR_OK) {
		return send_length;
	} else {
		return 0;
	}
}

uint16_t chibi_recv(void *data, const uint16_t length) {
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

	return recv_length;
}
