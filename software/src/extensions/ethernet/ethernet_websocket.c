/* master-brick
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_websocket.c: Ethernet websocket implementation
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

#include "ethernet_websocket.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "config.h"
#include "ethernet_config.h"
#include "ethernet_low_level.h"
#include "extensions/websocket.h"
#include "extensions/brickd.h"

extern ComInfo com_info;

bool ethernet_websocket_after_handshake[ETHERNET_MAX_SOCKETS] = {false, false, false, false, false, false, false};
uint8_t ethernet_websocket_to_read[ETHERNET_MAX_SOCKETS] = {0, 0, 0, 0, 0, 0, 0};
uint8_t ethernet_websocket_mask[ETHERNET_MAX_SOCKETS][WEBSOCKET_MASK_LENGTH] = {
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
};

uint8_t ethernet_websocket_mask_mod[ETHERNET_MAX_SOCKETS] = {0, 0, 0, 0, 0, 0, 0};

#define ETHERNET_WEBSOCKET_ERROR_ANSWER_1 "HTTP/1.1 200 OK\r\nContent-Length: 276\r\nContent-Type: text/html\r\n\r\n"
#define ETHERNET_WEBSOCKET_ERROR_ANSWER_2 "<html><head><title>This is a Websocket</title></head><body>Dear Sir or Madam,<br/><br/>I regret to inform you that there is no webserver "
#define ETHERNET_WEBSOCKET_ERROR_ANSWER_3 "here.<br/>This port is exclusively used for Websockets.<br/><br/>Yours faithfully,<blockquote>Ethernet Extension</blockquote></body></html>"

void ethernet_websocket_init(const uint8_t socket) {
	ethernet_websocket_after_handshake[socket] = false;
	ethernet_websocket_to_read[socket] = 0;
	ethernet_websocket_mask[socket][0] = 0;
	ethernet_websocket_mask[socket][1] = 0;
	ethernet_websocket_mask[socket][2] = 0;
	ethernet_websocket_mask[socket][3] = 0;
}

void ethernet_websocket_answer_error(int32_t user_data) {
	if(user_data > 7) {
		return;
	}

	ethernet_low_level_write_data_tcp_blocking(user_data, (uint8_t*)ETHERNET_WEBSOCKET_ERROR_ANSWER_1, strlen(ETHERNET_WEBSOCKET_ERROR_ANSWER_1));
	ethernet_low_level_write_data_tcp_blocking(user_data, (uint8_t*)ETHERNET_WEBSOCKET_ERROR_ANSWER_2, strlen(ETHERNET_WEBSOCKET_ERROR_ANSWER_2));
	ethernet_low_level_write_data_tcp_blocking(user_data, (uint8_t*)ETHERNET_WEBSOCKET_ERROR_ANSWER_3, strlen(ETHERNET_WEBSOCKET_ERROR_ANSWER_3));
}

void ethernet_websocket_answer_callback(char *answer, uint8_t length, int32_t user_data) {
	if(user_data > 7) {
		return;
	}

	ethernet_low_level_write_data_tcp_blocking(user_data, (uint8_t*)WEBSOCKET_ANSWER_STRING_1, strlen(WEBSOCKET_ANSWER_STRING_1));
	ethernet_low_level_write_data_tcp_blocking(user_data, (uint8_t*)answer, length);
	ethernet_low_level_write_data_tcp_blocking(user_data, (uint8_t*)WEBSOCKET_ANSWER_STRING_2, strlen(WEBSOCKET_ANSWER_STRING_2));
	ethernet_websocket_after_handshake[user_data] = true;

	com_info.current = COM_ETHERNET;
}

uint8_t ethernet_websocket_write_data_tcp(const uint8_t socket, const uint8_t *buffer, const uint8_t length) {
	WebsocketFrameClientToServer wfcts;
	wfcts.fin = 1;
	wfcts.rsv1 = 0;
	wfcts.rsv2 = 0;
	wfcts.rsv3 = 0;
	wfcts.opcode = 2;
	wfcts.mask = 0;
	wfcts.payload_length = length;

	uint8_t length_to_send = sizeof(WebsocketFrameClientToServer) + length;
	uint8_t websocket_data[length_to_send];
	memcpy((void*)websocket_data, (void*)&wfcts, sizeof(WebsocketFrameClientToServer));
	memcpy((void*)(websocket_data + sizeof(WebsocketFrameClientToServer)), buffer, length);

	// For websockets we have to send the whole data package, otherwise
	// we may send the header more then once
	ethernet_low_level_write_data_tcp_blocking(socket, websocket_data, length_to_send);

	com_debug_message((MessageHeader*)(websocket_data+2));

	return length;
}

void ethernet_websocket_close_frame(const WebsocketFrame *wsf, const uint8_t socket) {
	uint8_t length = wsf->payload_length;
	uint8_t tmp_buffer[length];
	while(length > 0) {
		length -= ethernet_low_level_read_data_tcp(socket, tmp_buffer, length);
	}
	ethernet_low_level_disconnect(socket);
	brickd_disconnect(socket);

	ethernet_low_level_socket_init(socket);
	ethernet_low_level_socket_listen(socket);
	brickd_disconnect(socket);
}

uint8_t ethernet_websocket_read_data_tcp(const uint8_t socket, uint8_t *buffer, const uint8_t length) {
	uint8_t read_length = 0;
	if(ethernet_websocket_after_handshake[socket]) {
		if(ethernet_websocket_to_read[socket] == 0) { // We need to read websocket header
			// buffer with max possible size of websocket header
			uint8_t ethernet_header_data[sizeof(WebsocketFrameExtended2)];
			uint8_t ethernet_header_length = sizeof(WebsocketFrame);

			// Nothing to read
			ethernet_header_length -= ethernet_low_level_read_data_tcp(socket, ethernet_header_data, ethernet_header_length);
			if(ethernet_header_length == sizeof(WebsocketFrame)) {
				return 0;
			}

			while(ethernet_header_length > 0) {
				ethernet_header_length -= ethernet_low_level_read_data_tcp(socket, ethernet_header_data, ethernet_header_length);
			}

			WebsocketFrame *wsf = (WebsocketFrame*)&ethernet_header_data;

			if(wsf->mask == 1) {
				memcpy((void*)ethernet_websocket_mask[socket], (void*)wsf->masking_key, WEBSOCKET_MASK_LENGTH);
			} else {
				// mask = 0 from browser is not allowed!
				logethe("Mask=0 not allowed\n\r");
				ethernet_low_level_emergency_disconnect(socket);
				return 0;
			}

			// We currently don't support 16 bit or 64 bit length
			if(wsf->payload_length == 126 || wsf->payload_length == 127) {
				ethernet_low_level_emergency_disconnect(socket);
				return 0;
			}

			switch(wsf->opcode) {
				case WEBSOCKET_OPCODE_CONTINUATION_FRAME:
				case WEBSOCKET_OPCODE_TEXT_FRAME: {
					// Continuation and text frame not allowed
					websocket_debug_header(wsf);
					logethe("Opcode not supported: %d\n\r", wsf->opcode);
					ethernet_low_level_emergency_disconnect(socket);
					return 0;
				}

				case WEBSOCKET_OPCODE_BINARY_FRAME: {
					ethernet_websocket_mask_mod[socket] = 0;
					ethernet_websocket_to_read[socket] = wsf->payload_length;
					break;
				}

				case WEBSOCKET_OPCODE_CLOSE_FRAME: {
					ethernet_websocket_close_frame(wsf, socket);
					break;
				}

				case WEBSOCKET_OPCODE_PING_FRAME: {
					logethe("Opcode not supported: %d\n\r", wsf->opcode);
					ethernet_low_level_emergency_disconnect(socket);
					break;
				}

				case WEBSCOKET_OPCODE_PONG_FRAME: {
					logethe("Opcode not supported: %d\n\r", wsf->opcode);
					ethernet_low_level_emergency_disconnect(socket);
					break;
				}
			}
		} else {
			// Websocket header is done, we can just read the data
			read_length = ethernet_low_level_read_data_tcp(socket, buffer, MIN(length, ethernet_websocket_to_read[socket]));

			for(uint8_t i = 0; i < read_length; i++) {
				buffer[i] ^= ethernet_websocket_mask[socket][ethernet_websocket_mask_mod[socket]];
				ethernet_websocket_mask_mod[socket]++;
				if(ethernet_websocket_mask_mod[socket] >= WEBSOCKET_MASK_LENGTH) {
					ethernet_websocket_mask_mod[socket] = 0;
				}
			}
			ethernet_websocket_to_read[socket] -= read_length;
		}
	} else {
		uint8_t data[100];
		uint8_t handshake_length = ethernet_low_level_read_data_tcp(socket, data, 100);
		if(handshake_length != 0) {
			websocket_parse_handshake((char*)data, handshake_length, ethernet_websocket_answer_callback, ethernet_websocket_answer_error, socket);
		}
	}

	return read_length;
}
