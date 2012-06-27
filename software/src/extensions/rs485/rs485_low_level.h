/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rs485_low_level.h: Low level RS485 protocol implementation
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


#ifndef RS485_LOW_LEVEL_H
#define RS485_LOW_LEVEL_H

#include <stdint.h>
#include <stdbool.h>

#define RS485_STATE_NONE                      0
#define RS485_STATE_SEND_DATA                 1
#define RS485_STATE_SEND_NO_DATA              2
#define RS485_STATE_RECV_ACK                  2
#define RS485_STATE_RECV_DATA_BEGIN           3
#define RS485_STATE_RECV_DATA_END             4
#define RS485_STATE_RECV_DATA_BUFFER_FULL     5
#define RS485_STATE_RECV_DATA_NOT_FOR_ME      6
#define RS485_STATE_RECV_DATA_NOT_FOR_ME_ACK  7
#define RS485_STATE_SEND_DATA_OK              8
#define RS485_STATE_SEND_DATA_NOK             9
#define RS485_STATE_SEND_DATA_NOK_SEND       10
#define RS485_STATE_SEND_DATA_BEGIN          11
#define RS485_STATE_SEND_DATA_END            12
#define RS485_STATE_RECV_OUT_OF_SYNC         13

#define RS485_MODBUS_FUNCTION_CODE          100
#define RS485_NO_MESSAGE_BUFFER_SIZE          9
#define RS485_WAIT_BEFORE_SEND_US            10

#define RS485_BUFFER_NO_ACK 0
#define RS485_BUFFER_WAIT_FOR_ACK 1
#define RS485_BUFFER_NO_DATA 2

#define RS485_SEND_BYTE_NO_CHECKSUM(data) do { \
    while((USART_RS485->US_CSR & US_CSR_TXEMPTY) == 0); \
    USART_RS485->US_THR = data; \
} while(0)

#define RS485_SEND_BYTE_CHECKSUM(data, checksum) do { \
    while((USART_RS485->US_CSR & US_CSR_TXEMPTY) == 0); \
    USART_RS485->US_THR = data; \
    PEARSON(checksum, data); \
} while(0)

#define RS485_RECV_BYTE_NO_CHECKSUM(data) do { \
	uint16_t counter = RS485_WAIT; \
	while(((USART_RS485->US_CSR & US_CSR_RXRDY) != US_CSR_RXRDY) && --counter); \
	if(counter == 0) { \
		if(rs485_type == RS485_TYPE_SLAVE) { \
			USART1->US_IER = US_IER_RXRDY; \
		} else { \
			rs485_set_mode_send(); \
		} \
		__enable_irq(); \
		return; \
	} \
	data = USART1->US_RHR; \
} while(0)

#define RS485_RECV_BYTE_CHECKSUM(data, rs485_checksum) do { \
	uint16_t counter = RS485_WAIT; \
	while(((USART_RS485->US_CSR & US_CSR_RXRDY) != US_CSR_RXRDY) && --counter); \
	data = USART1->US_RHR; \
	PEARSON(rs485_checksum, data); \
} while(0)

void rs485_low_level_begin_read(void);
void rs485_low_level_set_mode_receive(void);
void rs485_low_level_set_mode_send(void);
void rs485_low_level_set_mode_send_from_task(void);
void rs485_low_level_send(uint8_t address, uint8_t sequence_number, bool nodata);
void rs485_low_level_recv(void);
void rs485_low_level_handle_out_of_sync(bool first);
bool rs485_low_level_message_complete(uint8_t *data);
void rs485_low_level_handle_message(uint8_t *data);
uint16_t rs485_low_level_get_length_from_message(const uint8_t *data);
uint16_t rs485_low_level_get_crc_from_message(const uint8_t *data);
uint16_t rs485_low_level_crc16(uint8_t *data, uint8_t length);
void rs485_low_level_read_buffer(void *buffer, uint32_t size);

#endif
