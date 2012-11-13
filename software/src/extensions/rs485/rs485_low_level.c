/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rs485_low_level.c: Low level RS485 protocol implementation
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

#include "rs485_low_level.h"

#include <string.h>

#include "config.h"
#include "rs485.h"
#include "rs485_config.h"

#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "bricklib/utility/pearson_hash.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"

extern uint8_t rs485_buffer_recv[];
extern uint8_t rs485_buffer_send[];
extern uint16_t rs485_buffer_size_send;
extern uint16_t rs485_buffer_size_recv;
extern uint8_t rs485_mode;
extern uint8_t rs485_type;
extern uint8_t rs485_address;
extern RS485Config rs485_config;
extern bool rs485_master_send_empty;

uint8_t rs485_low_level_buffer_send[128];
uint8_t rs485_low_level_buffer_recv[128];
uint8_t rs485_low_level_buffer_ack = RS485_BUFFER_NO_ACK;

uint16_t rs485_error_crc = 0;

volatile uint8_t rs485_low_level_buffer_to_transfer = 0;

uint8_t rs485_state = RS485_STATE_NONE;
uint8_t rs485_last_sequence_number = 0;

extern Pin extension_pins[];
extern uint8_t RS485_RECV;

static const uint8_t crc_lookup_high[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const uint8_t crc_lookup_low[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

void rs485_low_level_send(uint8_t address, uint8_t sequence_number, bool nodata) {
	rs485_low_level_buffer_send[0] = address;
	rs485_low_level_buffer_send[1] = RS485_MODBUS_FUNCTION_CODE;
	rs485_low_level_buffer_send[2] = sequence_number;

	if(rs485_buffer_size_send == 0 || nodata || rs485_master_send_empty) {
		memset(&rs485_low_level_buffer_send[3], 0, 4);

		uint16_t crc16 = rs485_low_level_crc16(rs485_low_level_buffer_send, 7);

		memcpy(&rs485_low_level_buffer_send[7], &crc16, 2);

		rs485_low_level_buffer_to_transfer = RS485_NO_MESSAGE_BUFFER_SIZE;
	} else {
		memcpy(&rs485_low_level_buffer_send[3],
		       rs485_buffer_send,
		       rs485_buffer_size_send);

		uint16_t crc16 = rs485_low_level_crc16(rs485_low_level_buffer_send,
		                                       rs485_buffer_size_send + 3);

		memcpy(&rs485_low_level_buffer_send[rs485_buffer_size_send+3],
		       &crc16,
		       2);

		rs485_low_level_buffer_to_transfer = rs485_buffer_size_send + 5;
	}

	rs485_last_sequence_number = sequence_number;

	if(nodata) {
		rs485_state = RS485_STATE_SEND_NO_DATA;
		rs485_low_level_buffer_ack = RS485_BUFFER_NO_ACK;
	} else {
		rs485_state = RS485_STATE_SEND_DATA;
		rs485_low_level_buffer_ack = RS485_BUFFER_WAIT_FOR_ACK;
		if(rs485_buffer_size_send == 0) {
			rs485_low_level_buffer_ack |= RS485_BUFFER_NO_DATA;
		}
	}

	PIO_Set(&extension_pins[RS485_RECV]);
	USART_WriteBuffer(USART_RS485,
	                  rs485_low_level_buffer_send,
	                  rs485_low_level_buffer_to_transfer);

	rs485_low_level_set_mode_send();
}

void rs485_low_level_recv(void) {
	rs485_state = RS485_STATE_RECV_DATA_BEGIN;

	rs485_low_level_read_buffer(&rs485_low_level_buffer_recv,
	                            RS485_NO_MESSAGE_BUFFER_SIZE);
	rs485_low_level_set_mode_receive();
}

void rs485_low_level_set_mode_receive(void) {
    rs485_mode = RS485_MODE_RECEIVE;
	USART1->US_IDR = 0xFFFFFFFF;

	PIO_Clear(&extension_pins[RS485_RECV]);
	USART_EnableIt(USART_RS485, US_IER_ENDRX);
}

void rs485_low_level_set_mode_send(void) {
	rs485_mode = RS485_MODE_NONE;
	USART1->US_IDR = 0xFFFFFFFF;

	PIO_Set(&extension_pins[RS485_RECV]);

	__attribute__((unused)) volatile uint8_t state =  USART_RS485->US_CSR;
    USART_EnableIt(USART_RS485, US_IER_TXEMPTY);
}

void rs485_low_level_set_mode_send_from_task(void) {
	PIO_Set(&extension_pins[RS485_RECV]);
    rs485_mode = RS485_MODE_SEND;
}

void rs485_low_level_begin_read(void) {
	rs485_state = RS485_STATE_RECV_DATA_BEGIN;
	rs485_low_level_read_buffer(rs485_low_level_buffer_recv, 8);
	rs485_low_level_set_mode_receive();
}

void rs485_low_level_empty_read_buffer(void) {
	while((USART_RS485->US_CSR & US_CSR_RXRDY)) {
		USART_RS485->US_RHR;
	}
}

uint16_t rs485_low_level_get_crc_from_message(const uint8_t *data) {
	const uint16_t length = rs485_low_level_get_length_from_message(data);
	return *((uint16_t*)&data[length+3]);
}

uint16_t rs485_low_level_get_length_from_message(const uint8_t *data) {
	const uint16_t length = *((uint16_t*)&data[5]);
	if(length == 0) {
		return 4;
	}

	return length;
}

bool rs485_low_level_message_complete(uint8_t *data) {
	return rs485_low_level_get_length_from_message(data) < 5;
}

void rs485_low_level_handle_message(uint8_t *data) {
	const uint16_t length = rs485_low_level_get_length_from_message(data);
	const uint16_t crc16 = rs485_low_level_crc16(data, length+3);
	const uint8_t address = data[0];
	const uint8_t sequence_number = data[2];

	// If the CRC is wrong we force timeout
	if(crc16 != rs485_low_level_get_crc_from_message(data)) {
		rs485_error_crc++;
		logrse("CRC Error: %d\n\r", rs485_error_crc);
		rs485_low_level_resync();
		return;
	}

	// If the address is wrong we force timeout
	if(address != rs485_address) {
		rs485_low_level_recv();
		return;
	}

	// If data[3] == data[4] == 0: The message is an ack or polling for the
	// slave
	if(data[3] == 0 && data[4] == 0) {
		// If sequence number not the same, it is polling
		if(sequence_number != rs485_last_sequence_number) {
			if(rs485_type == RS485_TYPE_MASTER) {
				RS485_WAIT_BEFORE_SEND();
				rs485_low_level_send(rs485_address, sequence_number + 1, true);
			} else {
				RS485_WAIT_BEFORE_SEND();
				rs485_low_level_send(rs485_address, sequence_number, false);
			}
		// Sequence number is the same, so the message is an ack
		} else {
			// If we waited for an ack, we can set the send buffer size back to 0
			if(rs485_low_level_buffer_ack & RS485_BUFFER_WAIT_FOR_ACK) {
				if(!(rs485_low_level_buffer_ack & RS485_BUFFER_NO_DATA)) {
					if(!rs485_master_send_empty) {
						rs485_buffer_size_send = 0;
					}
				}
				rs485_low_level_buffer_ack = RS485_BUFFER_NO_ACK;

				if(rs485_type == RS485_TYPE_MASTER) {
					rs485_low_level_set_mode_send_from_task();
				} else {
					rs485_low_level_recv();
				}
			// We did not wait for an ack, that probably means we accidentally
			// got polled with the same sequence number as before?
			} else {
				if(rs485_type == RS485_TYPE_MASTER) {
					rs485_low_level_set_mode_send_from_task();
				} else {
					RS485_WAIT_BEFORE_SEND();
					rs485_low_level_send(rs485_address, sequence_number, false);
				}
			}
		}

		rs485_master_send_empty = false;
		return;
	}

	if(rs485_buffer_size_recv == 0) {
		if(rs485_low_level_buffer_ack & RS485_BUFFER_WAIT_FOR_ACK) {
			if(!(rs485_low_level_buffer_ack & RS485_BUFFER_NO_DATA)) {
				if(!rs485_master_send_empty) {
					rs485_buffer_size_send = 0;
				}
			}
			rs485_low_level_buffer_ack = RS485_BUFFER_NO_ACK;
		}

		if(rs485_type == RS485_TYPE_MASTER) {
			// If the sequence number is the same, it means that the message
			// is an answer for the last request
			if(sequence_number == rs485_last_sequence_number) {
				memcpy(rs485_buffer_recv,
					   &rs485_low_level_buffer_recv[3],
					   length);
				rs485_buffer_size_recv = length;
			}

			// The master always sends an ack without data, otherwise
			// other rs485 participants might not have there turn
			RS485_WAIT_BEFORE_SEND();
			rs485_low_level_send(rs485_address, sequence_number, true);
		} else {
			// If the sequence number is the same, it means that an ack was not
			// received. We already handled this message
			if(sequence_number != rs485_last_sequence_number) {
				memcpy(rs485_buffer_recv,
					   &rs485_low_level_buffer_recv[3],
					   length);
				rs485_buffer_size_recv = length;
			}

			// The slave can send ack with or without data, with old
			// sequence number
			RS485_WAIT_BEFORE_SEND();
			rs485_low_level_send(rs485_address, sequence_number, false);
		}
	// If the recv buffer is full we force a timeout
	} else {
		if(rs485_type == RS485_TYPE_SLAVE) {
			rs485_low_level_recv();
		}
	}
	rs485_master_send_empty = false;
}

void USART1_IrqHandler() {
	USART1->US_IDR = 0xFFFFFFFF;

	uint32_t state = USART_RS485->US_CSR;
    if((state & US_CSR_TXEMPTY) == US_CSR_TXEMPTY) {
    	switch(rs485_state) {
			case RS485_STATE_SEND_DATA: {
				rs485_low_level_recv();
				return;
			}

			case RS485_STATE_SEND_NO_DATA: {
				rs485_low_level_set_mode_send_from_task();
				return;
			}
    	}
    }

    if((state & US_CSR_ENDRX) == US_CSR_ENDRX) {
    	switch(rs485_state) {
			case RS485_STATE_RECV_DATA_BEGIN: {
				if(rs485_low_level_message_complete(rs485_low_level_buffer_recv)) {
					PIO_Set(&extension_pins[RS485_RECV]);
					rs485_low_level_handle_message(rs485_low_level_buffer_recv);
				} else {
					rs485_state = RS485_STATE_RECV_DATA_END;
					const uint16_t length = rs485_low_level_get_length_from_message(rs485_low_level_buffer_recv);
					// TODO: What is the right thing to do here? Something is definitely wrong!
					if(length > RS485_MAX_DATA_LENGTH) {
						rs485_low_level_resync();
						return;
					}
					const uint16_t length_ro_read = length -9 +3 +2;
					rs485_low_level_read_buffer(&rs485_low_level_buffer_recv[9],
					                            length_ro_read);
					rs485_low_level_set_mode_receive();
				}
				return;
			}

			case RS485_STATE_RECV_DATA_END: {
				rs485_low_level_handle_message(rs485_low_level_buffer_recv);

				return;
			}
		}
    }
}

void rs485_low_level_read_buffer(void *buffer, uint32_t size) {
        USART_RS485->US_RPR = (uint32_t)buffer;
        USART_RS485->US_RCR = size;
        USART_RS485->US_PTCR = US_PTCR_RXTEN;
}

uint16_t rs485_low_level_crc16(uint8_t *data, uint8_t length) {
    uint16_t high = 0xFF;
    uint16_t low  = 0xFF;
    uint32_t index;

    while(length--) {
    	index = low ^ *(data++);
        low   = (uint8_t)(high ^ crc_lookup_high[index]);
        high  = crc_lookup_low[index];
    }

    return (uint16_t)(high << 8 | low);
}

void rs485_low_level_resync(void) {
	USART_RS485->US_IDR = 0xFFFFFFFF;
	PIO_Clear(&extension_pins[RS485_RECV]);

	__attribute__((unused)) volatile uint8_t data;

	while(true) {
		uint32_t counter = 2*8*BOARD_MCK/rs485_config.speed;
		while(((USART_RS485->US_CSR & US_CSR_RXRDY) != US_CSR_RXRDY) && --counter);
		if(counter == 0) {
			if(rs485_type == RS485_TYPE_MASTER) {
				rs485_low_level_set_mode_send_from_task();
			} else {
				rs485_low_level_recv();
			}
			return;
		}

		data = USART1->US_RHR;
	}
}
