/* master-brick
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_websocket.h: Websocket implementation
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

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdint.h>

typedef void (*websocket_answer_callback_t)(char*, uint8_t, int32_t);
typedef void (*websocket_answer_error_t)(int32_t);

#define MAX_WEBSOCKET_LINE_LENGTH 100

#define WEBSOCKET_CLIENT_KEY_STR "Sec-WebSocket-Key:"
#define WEBSOCKET_SERVER_KEY "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

#define WEBSOCKET_ANSWER_STRING_1 "HTTP/1.1 101 Switching Protocols\r\nAccess-Control-Allow-Origin: *\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: "
#define WEBSOCKET_ANSWER_STRING_2 "\r\nSec-WebSocket-Protocol: tfp\r\n\r\n"

#define WEBSOCKET_KEY_LEN 38 // It should be max 36
#define WEBSOCKET_CONCATKEY_LEN 66  // It should be max 36 (server key) + 28 (base64 hash) = 64
#define MAX_WEBSOCKET_ANSWER_LENGTH 256

#define WEBSOCKET_OPCODE_CONTINUATION_FRAME  0
#define WEBSOCKET_OPCODE_TEXT_FRAME          1
#define WEBSOCKET_OPCODE_BINARY_FRAME        2
#define WEBSOCKET_OPCODE_CLOSE_FRAME         8
#define WEBSOCKET_OPCODE_PING_FRAME          9
#define WEBSCOKET_OPCODE_PONG_FRAME         10

#define WEBSOCKET_MASK_LENGTH 4

typedef struct {
	uint8_t opcode : 4;
	uint8_t rsv1 : 1;
	uint8_t rsv2 : 1;
	uint8_t rsv3 : 1;
	uint8_t fin : 1;

	uint8_t payload_length : 7;
	uint8_t mask : 1; // mask is 0, no masking key
} __attribute__((__packed__)) WebsocketFrameClientToServer;

typedef struct {
	uint8_t opcode : 4;
	uint8_t rsv1 : 1;
	uint8_t rsv2 : 1;
	uint8_t rsv3 : 1;
	uint8_t fin : 1;

	uint8_t payload_length : 7;
	uint8_t mask : 1;

	uint8_t masking_key[WEBSOCKET_MASK_LENGTH]; // only used if mask = 1
} __attribute__((__packed__)) WebsocketFrame;

// Extended is used if payload_length = 126
typedef struct {
	uint8_t opcode : 4;
	uint8_t rsv1 : 1;
	uint8_t rsv2 : 1;
	uint8_t rsv3 : 1;
	uint8_t fin : 1;

	uint8_t payload_length : 7;
	uint8_t mask : 1;

	uint16_t payload_length_extended;

	uint8_t masking_key[WEBSOCKET_MASK_LENGTH]; // only used if mask = 1
} __attribute__((__packed__)) WebsocketFrameExtended;

// Extended2 is used if payload_length = 127
typedef struct {
	uint8_t opcode : 4;
	uint8_t rsv1 : 1;
	uint8_t rsv2 : 1;
	uint8_t rsv3 : 1;
	uint8_t fin : 1;

	uint8_t payload_length : 7;
	uint8_t mask : 1;

	uint64_t payload_length_extended;

	uint8_t masking_key[WEBSOCKET_MASK_LENGTH]; // only used if mask = 1
} __attribute__((__packed__)) WebsocketFrameExtended2;

void websocket_parse_handshake(char *header_part, uint8_t length, websocket_answer_callback_t websocket_answer_callback, websocket_answer_error_t websocket_answer_error, int32_t user_data);
void websocket_debug_header(const WebsocketFrame *wsf);

#endif
