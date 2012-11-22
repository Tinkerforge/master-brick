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
#define RS485_NO_MESSAGE_BUFFER_SIZE         13

#define RS485_BUFFER_NO_ACK 0
#define RS485_BUFFER_WAIT_FOR_ACK 1
#define RS485_BUFFER_NO_DATA 2

#define RS485_WAIT_BEFORE_SEND() \
	do { \
		uint32_t i = 4*BOARD_MCK/rs485_config.speed; \
		__ASM volatile ( \
			"PUSH {R0}\n" \
			"MOV R0, %0\n" \
			"1:\n" \
			"SUBS R0, #1\n" \
			"BNE.N 1b\n" \
			"POP {R0}\n" \
			:: "r" (i) \
		); \
	} while(0)

void rs485_low_level_begin_read(void);
void rs485_low_level_set_mode_receive(void);
void rs485_low_level_set_mode_send(void);
void rs485_low_level_set_mode_send_from_task(void);
void rs485_low_level_send(const uint8_t address, const uint8_t sequence_number, const bool nodata);
void rs485_low_level_recv(void);
void rs485_low_level_handle_out_of_sync(const bool first);
bool rs485_low_level_message_complete(const uint8_t *data);
void rs485_low_level_handle_message(const uint8_t *data);
uint8_t rs485_low_level_get_length_from_message(const uint8_t *data);
uint16_t rs485_low_level_get_crc_from_message(const uint8_t *data);
uint32_t rs485_low_level_get_uid_from_message(const uint8_t *data);
uint8_t rs485_low_level_get_fid_from_message(const uint8_t *data);
uint16_t rs485_low_level_crc16(const uint8_t *data, const uint8_t length);
void rs485_low_level_read_buffer(void *buffer, const uint32_t size);
void rs485_low_level_resync(void);
void rs485_low_level_insert_uid(void* data);

#endif
