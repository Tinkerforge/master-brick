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

#include "routing.h"

#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "bricklib/com/com_messages.h"
#include "bricklib/utility/pearson_hash.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/drivers/tc/tc.h"
#include "bricklib/utility/system_timer.h"


#define RS485_TC_CHANNEL_NUM 0
#define RS485_TC_CHANNEL (TC0->TC_CHANNEL[RS485_TC_CHANNEL_NUM])
#define RS485_COUNTER RS485_TC_CHANNEL.TC_RC


extern uint8_t rs485_buffer_recv[];
extern uint8_t rs485_buffer_send[];
extern uint16_t rs485_buffer_size_send;
extern uint16_t rs485_buffer_size_recv;
extern uint8_t rs485_mode;
extern uint8_t rs485_type;
extern uint8_t rs485_address;
extern RS485Config rs485_config;
extern bool rs485_master_send_empty;
extern uint8_t rs485_send_address;
extern uint8_t rs485_send_tries;

uint8_t rs485_low_level_buffer_send[RS485_MAX_DATA_LENGTH+RS485_MESSAGE_OVERHEAD];
uint8_t rs485_low_level_buffer_recv[RS485_MAX_DATA_LENGTH+RS485_MESSAGE_OVERHEAD];
uint8_t rs485_low_level_buffer_ack = RS485_BUFFER_NO_ACK;

uint16_t rs485_error_crc = 0;

volatile uint8_t rs485_low_level_buffer_to_transfer = 0;

uint8_t rs485_state = RS485_STATE_NONE;
uint8_t rs485_last_sequence_number = 0;

uint8_t rs485_first_message = 0;
uint32_t rs485_first_message_time = 0;

extern Pin extension_pins[];
extern uint8_t RS485_RECV;
extern ComInfo com_info;

static const uint32_t crc_lookup[] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

uint8_t rs485_low_level_irq_address = 0;
uint8_t rs485_low_level_irq_sequence_number = 0;
uint8_t rs485_low_level_irq_nodata = false;

// Baudrate: 500000 -> t=25us
// Baudrate: 250000 -> t=50us
// -> t_us = 12500000/baudrate
void TC0_IrqHandler(void) {
	// acknowledge interrupt
	tc_channel_stop(&RS485_TC_CHANNEL);
	tc_channel_interrupt_ack(&RS485_TC_CHANNEL);

	rs485_low_level_send(rs485_low_level_irq_address, rs485_low_level_irq_sequence_number, rs485_low_level_irq_nodata);
}

void rs485_low_level_start_tc(uint32_t baudrate) {
	RS485_COUNTER = (12500000/2) / baudrate;
	tc_channel_start(&RS485_TC_CHANNEL);
}

void rs485_low_level_init(void) {
    // Enable peripheral clock for TC
    PMC->PMC_PCER0 = 1 << ID_TC0;

    // Configure and enable TC interrupts
	NVIC_DisableIRQ(TC0_IRQn);
	NVIC_ClearPendingIRQ(TC0_IRQn);
	NVIC_SetPriority(TC0_IRQn, 0);
	NVIC_EnableIRQ(TC0_IRQn);

	// CLOCK4 = MCK/128 == 2us per tick
	tc_channel_init(&RS485_TC_CHANNEL, TC_CMR_TCCLKS_TIMER_CLOCK4 | TC_CMR_CPCTRG);

    // Interrupt in compare
    tc_channel_interrupt_set(&RS485_TC_CHANNEL, TC_IER_CPCS);
}

void rs485_low_level_send(const uint8_t address, const uint8_t sequence_number, const bool nodata) {
	rs485_low_level_buffer_send[0] = address;
	rs485_low_level_buffer_send[1] = RS485_MODBUS_FUNCTION_CODE;
	rs485_low_level_buffer_send[2] = sequence_number;

	if(rs485_buffer_size_send == 0 || nodata || rs485_master_send_empty) {
		memset(&rs485_low_level_buffer_send[RS485_MESSAGE_HEADER_SIZE], 0, sizeof(MessageHeader));
		rs485_low_level_buffer_send[RS485_MESSAGE_LENGTH] = sizeof(MessageHeader);
		uint16_t crc16 = rs485_low_level_crc16(rs485_low_level_buffer_send, RS485_MESSAGE_HEADER_SIZE + sizeof(MessageHeader));

		memcpy(&rs485_low_level_buffer_send[RS485_MESSAGE_HEADER_SIZE + sizeof(MessageHeader)], &crc16, RS485_MESSAGE_FOOTER_SIZE);

		rs485_low_level_buffer_to_transfer = RS485_NO_MESSAGE_BUFFER_SIZE;
	} else {
		memcpy(&rs485_low_level_buffer_send[RS485_MESSAGE_HEADER_SIZE],
		       rs485_buffer_send,
		       rs485_buffer_size_send);

		uint16_t crc16 = rs485_low_level_crc16(rs485_low_level_buffer_send,
		                                       rs485_buffer_size_send + RS485_MESSAGE_HEADER_SIZE);

		memcpy(&rs485_low_level_buffer_send[rs485_buffer_size_send + RS485_MESSAGE_HEADER_SIZE],
		       &crc16,
		       RS485_MESSAGE_FOOTER_SIZE);

		rs485_low_level_buffer_to_transfer = rs485_buffer_size_send + RS485_MESSAGE_OVERHEAD;
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
	rs485_low_level_read_buffer(rs485_low_level_buffer_recv, RS485_NO_MESSAGE_BUFFER_SIZE);
	rs485_low_level_set_mode_receive();
}

void rs485_low_level_empty_read_buffer(void) {
	while((USART_RS485->US_CSR & US_CSR_RXRDY)) {
		USART_RS485->US_RHR;
	}
}

uint16_t rs485_low_level_get_crc_from_message(const uint8_t *data) {
	const uint16_t length = rs485_low_level_get_length_from_message(data);
	return *((uint16_t*)&data[length+RS485_MESSAGE_HEADER_SIZE]);
}

uint32_t rs485_low_level_get_uid_from_message(const uint8_t *data) {
	return data[RS485_MESSAGE_UID] |
           (data[RS485_MESSAGE_UID+1] << 8) |
           (data[RS485_MESSAGE_UID+2] << 16) |
           (data[RS485_MESSAGE_UID+3] << 24);
}

uint8_t rs485_low_level_get_fid_from_message(const uint8_t *data) {
	return data[RS485_MESSGAE_FID];
}

uint8_t rs485_low_level_get_length_from_message(const uint8_t *data) {
	return data[RS485_MESSAGE_LENGTH];
}

bool rs485_low_level_message_complete(const uint8_t *data) {
	return rs485_low_level_get_length_from_message(data) < RS485_MESSAGE_OVERHEAD;
}

void rs485_low_level_handle_message(const uint8_t *data) {
	const uint16_t length = rs485_low_level_get_length_from_message(data);
	const uint16_t crc16 = rs485_low_level_crc16(data, length+RS485_MESSAGE_HEADER_SIZE);

	const uint8_t address = data[RS485_MESSAGE_MODBUS_ADDRESS];
	const uint8_t sequence_number = data[RS485_MESSAGE_MODBUS_SEQUENCE_NUMBER];

	// If the CRC is wrong we force timeout
	if(crc16 != rs485_low_level_get_crc_from_message(data)) {
		rs485_error_crc++;
		logrse("CRC Error (%d vs %d) counter: %d\n\r", crc16,
		                                               rs485_low_level_get_crc_from_message(data),
		                                               rs485_error_crc);
		rs485_low_level_resync();
		return;
	}

	// If the address is wrong we force timeout
	if(address != rs485_address) {
		rs485_low_level_recv();
		return;
	}

	// If fid == uid == 0: The message is an ack or polling for the
	// slave
	const uint8_t fid = rs485_low_level_get_fid_from_message(data);
	const uint32_t uid = rs485_low_level_get_uid_from_message(data);
	if(fid == 0 && uid == 0) {
		// If sequence number not the same, it is polling
		if(sequence_number != rs485_last_sequence_number) {
			if(rs485_type == RS485_TYPE_MASTER) {
				rs485_low_level_irq_address = rs485_address;
				rs485_low_level_irq_sequence_number = sequence_number + 1;
				rs485_low_level_irq_nodata = true;
				rs485_low_level_start_tc(rs485_config.speed);
			} else {
				if(rs485_first_message == 0) {
					rs485_first_message = 1;
					rs485_first_message_time = system_timer_get_ms();
				}

				rs485_low_level_irq_address = rs485_address;
				rs485_low_level_irq_sequence_number = sequence_number;
				rs485_low_level_irq_nodata = false;
				rs485_low_level_start_tc(rs485_config.speed);
			}
		// Sequence number is the same, so the message is an ack
		} else {
			// If we waited for an ack, we can set the send buffer size back to 0
			if(rs485_low_level_buffer_ack & RS485_BUFFER_WAIT_FOR_ACK) {
				if(!(rs485_low_level_buffer_ack & RS485_BUFFER_NO_DATA)) {
					if(!rs485_master_send_empty) {
						rs485_buffer_size_send = 0;
						rs485_send_tries = 0;
						rs485_send_address = 0;
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
					rs485_low_level_irq_address = rs485_address;
					rs485_low_level_irq_sequence_number = sequence_number;
					rs485_low_level_irq_nodata = false;
					rs485_low_level_start_tc(rs485_config.speed);
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
					rs485_send_tries = 0;
					rs485_send_address = 0;
				}
			}
			rs485_low_level_buffer_ack = RS485_BUFFER_NO_ACK;
		}

		if(rs485_type == RS485_TYPE_MASTER) {
			// If the sequence number is the same, it means that the message
			// is an answer for the last request
			if(sequence_number == rs485_last_sequence_number) {
				memcpy(rs485_buffer_recv,
					   &rs485_low_level_buffer_recv[RS485_MESSAGE_HEADER_SIZE],
					   length);
				rs485_buffer_size_recv = length;

				if(uid != 0) {
					RouteTo route_to = routing_route_extension_to(uid);
					if(route_to.to == 0 && route_to.option == 0) {
						uint8_t to = 0;
						if(com_info.ext[0] == COM_RS485) {
							to = ROUTING_EXTENSION_1;
						} else if(com_info.ext[0] == COM_RS485) {
							to = ROUTING_EXTENSION_2;
						}

						RouteTo new_route = {to, address};
						routing_add_route(uid, new_route);
					}
				}

				rs485_low_level_insert_uid(rs485_buffer_recv);
			}

			// The master always sends an ack without data, otherwise
			// other rs485 participants might not have their turn
			rs485_low_level_irq_address = rs485_address;
			rs485_low_level_irq_sequence_number = sequence_number;
			rs485_low_level_irq_nodata = true;
			rs485_low_level_start_tc(rs485_config.speed);
		} else {
			// If the sequence number is the same, it means that an ack was not
			// received. We already handled this message
			if(sequence_number != rs485_last_sequence_number) {
				memcpy(rs485_buffer_recv,
					   &rs485_low_level_buffer_recv[RS485_MESSAGE_HEADER_SIZE],
					   length);
				rs485_buffer_size_recv = length;
			}

			// The slave can send ack with or without data, with old
			// sequence number
			rs485_low_level_irq_address = rs485_address;
			rs485_low_level_irq_sequence_number = sequence_number;
			rs485_low_level_irq_nodata = false;
			rs485_low_level_start_tc(rs485_config.speed);
		}
	// If the recv buffer is full we force a timeout
	} else {
		if(rs485_type == RS485_TYPE_SLAVE) {
			rs485_low_level_recv();
		}
	}
	rs485_master_send_empty = false;
}

void rs485_low_level_insert_uid(void* data) {
	if(rs485_buffer_size_recv > sizeof(MessageHeader)) {
		EnumerateCallback *enum_cb =  (EnumerateCallback*)data;
		if(enum_cb->header.fid == FID_ENUMERATE_CALLBACK || enum_cb->header.fid == FID_GET_IDENTITY) {
			if(enum_cb->position == '0' && enum_cb->connected_uid[1] == '\0') {
				uid_to_serial_number(com_info.uid, enum_cb->connected_uid);
			}
		}
	}
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
					const int16_t length_ro_read = length
					                               -RS485_NO_MESSAGE_BUFFER_SIZE
					                               +RS485_MESSAGE_HEADER_SIZE
					                               +RS485_MESSAGE_FOOTER_SIZE;
					rs485_low_level_read_buffer(&rs485_low_level_buffer_recv[RS485_NO_MESSAGE_BUFFER_SIZE],
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

void rs485_low_level_read_buffer(void *buffer, const uint32_t size) {
        USART_RS485->US_RPR = (uint32_t)buffer;
        USART_RS485->US_RCR = size;
        USART_RS485->US_PTCR = US_PTCR_RXTEN;
}

uint16_t rs485_low_level_crc16(const uint8_t *data, const uint8_t length) {
    uint16_t crc = 0xFFFF;
    uint8_t index;

    uint8_t counter = length;
    while(counter--) {
    	index = crc ^ *(data++);
        crc = (crc >> 8) ^ crc_lookup[index];
    }

    return crc;
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
