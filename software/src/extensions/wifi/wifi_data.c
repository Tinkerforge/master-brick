/* master-brick
 * Copyright (C) 2012-2013 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_data.c: Data mode functionality for WIFI Extension
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

#include "wifi_data.h"

#include "wifi_low_level.h"
#include "wifi_ringbuffer.h"
#include "wifi_command.h"

#include "bricklib/com/com_messages.h"
#include "bricklib/utility/led.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/logging/logging.h"

#include "extensions/brickd.h"

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

//#define WIFI_DEBUG_PRINT_DATA

WIFIDataState wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;
uint16_t wifi_data_recv_length_desired = 0;
uint16_t wifi_data_recv_length = 0;
uint16_t wifi_buffer_size_counter = 0;
int8_t  wifi_data_current_cid = -1;
char wifi_data_current_async_type = '0';

char wifi_data_length_buffer[4] = {0};
extern uint8_t wifi_buffer_recv[];
extern uint16_t wifi_buffer_size_recv;
extern Pin extension_pins[];
extern WifiStatus wifi_status;

extern uint8_t WIFI_DATA_RDY;
extern int8_t wifi_new_cid;
extern uint32_t wifi_ringbuffer_overflow;
extern int8_t wifi_force_ack_counter[WIFI_DATA_MAX_CID];

static const uint16_t wifi_data_lenght_mul[4] = {1000, 100, 10, 1};

bool wifi_data_currently_stuffing = false;
bool wifi_xoff = false;

bool wifi_data_cid_present[WIFI_DATA_MAX_CID] = {false};
int8_t wifi_data_expecting_new_cid = -1;

char wifi_buffer_command[WIFI_COMMAND_BUFFER_SIZE];
uint8_t wifi_buffer_command_length = 0;

bool wifi_data_next_handle_ringbuffer(char *ndata, bool transceive) {
	if(transceive) {
		if(wifi_buffer_size_recv != 0) {
			if(PIO_Get(&extension_pins[WIFI_DATA_RDY])) {
				if(!wifi_ringbuffer_is_full()) {
					*ndata = wifi_low_level_write_byte(WIFI_LOW_LEVEL_SPI_IDLE_CHAR);
					wifi_ringbuffer_add(*ndata);
				}
			}
			return false;
		} else {
			if(wifi_ringbuffer_is_empty()) {
				if(PIO_Get(&extension_pins[WIFI_DATA_RDY])) {
					*ndata = wifi_low_level_write_byte(WIFI_LOW_LEVEL_SPI_IDLE_CHAR);
					return true;
				} else {
					return false;
				}
			} else {
				wifi_ringbuffer_get(ndata);
				return true;
			}
		}
	} else {
		if(wifi_buffer_size_recv != 0) {
			// What should we do if ringbuffer is full here?
			if(wifi_ringbuffer_is_full()) {
				logwifie("Ringbuffer full, possibly overflow\n\r");
			}
			wifi_ringbuffer_add(*ndata);
			return false;
		}

		// We can handle new data now, but we first have to use up ringbuffer
		if(!wifi_ringbuffer_is_empty()) {
			char tmp;
			wifi_ringbuffer_get(&tmp);
			wifi_ringbuffer_add(*ndata);
			*ndata = tmp;
		}
		return true;
	}
	return true;
}

bool wifi_data_next_handle_stuffing(char *ndata) {
	if(wifi_data_currently_stuffing) {
		*ndata ^= 0x20;
		wifi_data_currently_stuffing = false;
	} else {
		switch(*ndata) {
			case WIFI_LOW_LEVEL_SPI_ESC_CHAR: {
				wifi_data_currently_stuffing = true;
				return false;
			}

			case WIFI_LOW_LEVEL_SPI_XOFF_CHAR: {
				wifi_xoff = true;
				return false;
			}

			case WIFI_LOW_LEVEL_SPI_XON_CHAR: {
				wifi_xoff = false;
				return false;
			}

			case WIFI_LOW_LEVEL_SPI_IDLE_CHAR:
			case WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ONE:
			case WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ZERO:
			case WIFI_LOW_LEVEL_SPI_LINK_READY: {
				return false;
			}
		}

		return true;
	}

	return true;
}

void wifi_data_next_handle_wait_for_esc(char ndata) {
	if(ndata == WIFI_DATA_CHAR_ESC) {
#ifdef WIFI_DEBUG_PRINT_DATA
		logwifid("Data Recv: <ESC>");
#endif
		wifi_data_state = WIFI_DATA_WAIT_FOR_ESC_CHAR;
	} else {
		if(ndata == '\r' || ndata == '\n') {
			wifi_data_state = WIFI_DATA_WAIT_FOR_COMMAND_START;
			return;
		}

		if(ndata >= WIFI_DATA_ASCII_START && ndata <= WIFI_DATA_ASCII_END) {
			wifi_buffer_command[0] = ndata;
			wifi_buffer_command_length = 1;
			wifi_data_state = WIFI_DATA_WAIT_FOR_COMMAND_END;
			return;
		}

		if(ndata < WIFI_DATA_ASCII_START || ndata > WIFI_DATA_ASCII_END) {
			logwifid("ndata not ASCII (0): %x\n\r", ndata);
			// TODO: No ESC, no \r or \n and no ASCII. What now?
		}
	}
}

void wifi_data_next_handle_wait_for_command_start(char ndata) {
	if(ndata == WIFI_DATA_CHAR_ESC) {
		wifi_data_state = WIFI_DATA_WAIT_FOR_ESC_CHAR;
		return;
	}

	if(ndata == '\r' || ndata == '\n') {
		return;
	}

	if(ndata < WIFI_DATA_ASCII_START || ndata > WIFI_DATA_ASCII_END) {
		logwifid("ndata not ASCII (1): %x\n\r", ndata);
		// TODO: ndata not ASCII: What now?
	}

	wifi_buffer_command[0] = ndata;
	wifi_buffer_command_length = 1;
	wifi_data_state = WIFI_DATA_WAIT_FOR_COMMAND_END;
}

void wifi_data_next_handle_wait_for_command_end(char ndata) {
	if(ndata == WIFI_DATA_CHAR_ESC) {
		wifi_data_state = WIFI_DATA_WAIT_FOR_ESC_CHAR;
		return;
	}

	if(ndata == '\r' || ndata == '\n') {
		wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;

		wifi_buffer_command[wifi_buffer_command_length] = '\0';

		wifi_command_parse((char*)wifi_buffer_command, wifi_buffer_command_length);
		wifi_buffer_command_length = 0;
		return;
	}

	if(ndata < WIFI_DATA_ASCII_START || ndata > WIFI_DATA_ASCII_END) {
		logwifid("ndata not ASCII (2): %x\n\r", ndata);
		// TODO: ndata not ASCII: What now?
	}

	if(wifi_buffer_command_length >= (WIFI_COMMAND_BUFFER_SIZE-1)) {
		logwifid("wifi_buffer_command_length > 128: %d\n\r", wifi_buffer_command_length);
		// TODO: wifi_buffer_command_length > 128, what now?
		return;
	}

	wifi_buffer_command[wifi_buffer_command_length] = ndata;
	wifi_buffer_command_length++;
}

void wifi_data_next_handle_wait_for_esc_char(char ndata) {
#ifdef WIFI_DEBUG_PRINT_DATA
	logwohd("%c", ndata);
#endif
	if(ndata == WIFI_DATA_CHAR_Z) {
		wifi_data_state = WIFI_DATA_WAIT_FOR_CID;
#if LOGGING_LEVEL == LOGGING_DEBUG
		if(wifi_data_recv_length != 0) {
			logwifid("<ESC>Z while buffer full\n\r");
		}
#endif
		wifi_data_recv_length = 0;
		wifi_data_recv_length_desired = 0;
	} else if(ndata == WIFI_DATA_CHAR_A) {
		wifi_data_state = WIFI_DATA_WAIT_FOR_ASYNC;
#if LOGGING_LEVEL == LOGGING_DEBUG
		if(wifi_data_recv_length != 0) {
			logwifid("<ESC>A while buffer full\n\r");
		}
#endif
		wifi_data_recv_length = 0;
		wifi_data_recv_length_desired = 0;
	} else if(ndata == WIFI_DATA_CHAR_O) {
#ifdef WIFI_DEBUG_PRINT_DATA
		logwohd("\n\r");
#endif
		wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;
	} else {
		logwohd("\n\r");
		wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;
	}
}

void wifi_data_next_handle_wait_for_async(char ndata) {
	logwohd("%c", ndata);
	wifi_data_current_async_type = ndata;
	wifi_data_state = WIFI_DATA_WAIT_FOR_ASYNC_READ_SIZE;

	switch(ndata) {
		case WIFI_DATA_ASYNC_TYPE_CONNECT: {
			wifi_data_expecting_new_cid = 4; // read cid in 4 bytes
			return;
		}

		case WIFI_DATA_ASYNC_TYPE_DISCONNECT: {
			wifi_data_expecting_new_cid = 2; // read cid in 2 bytes
			return;
		}

		case WIFI_DATA_ASYNC_TYPE_DISASSOCIATION: {
			wifi_status.state = WIFI_STATE_DISASSOCIATED;
			break;
		}

		default: {
			logwifiw("Unexpected Async option: %d\n\r", ndata);
			break;
		}
	}

	wifi_data_expecting_new_cid = -1;
}

void wifi_data_next_handle_wait_for_async_read_size(char ndata) {
	logwohd("%c", ndata);
	wifi_data_recv_length++;
	if(wifi_data_recv_length == 1) {
		wifi_data_recv_length_desired += wifi_data_hex_to_int(ndata)*16;
	} else if(wifi_data_recv_length == 2) {
		wifi_data_recv_length = 0;
		wifi_data_recv_length_desired += wifi_data_hex_to_int(ndata)+2;
		wifi_data_state = WIFI_DATA_WAIT_FOR_ASYNC_READ_MSG;
	} else {
		logwifie("wifi_data_recv_length too large (while waiting for async)\n\r");
		// TODO: What now?
	}
}

void wifi_data_next_handle_wait_for_async_read_msg(char ndata) {
	logwohd("%c", ndata);
	if(wifi_data_expecting_new_cid > 0) {
		wifi_data_expecting_new_cid--;
	} else if(wifi_data_expecting_new_cid == 0) {
		const int8_t cid = wifi_data_hex_to_int(ndata);
		switch(wifi_data_current_async_type) {
			case WIFI_DATA_ASYNC_TYPE_CONNECT: {
				if(cid != -1) {
					wifi_new_cid = cid;
					wifi_data_cid_present[cid] = true;
					led_on(LED_EXT_BLUE_3);
				}

				wifi_data_expecting_new_cid = -1;
				break;
			}

			case WIFI_DATA_ASYNC_TYPE_DISCONNECT: {
				wifi_data_disconnect(cid);

				wifi_data_expecting_new_cid = -1;
				break;
			}

		}
	}

	wifi_data_recv_length++;
	if(wifi_data_recv_length >= wifi_data_recv_length_desired) {
		logwohd("\n\r");
		wifi_data_recv_length = 0;
		wifi_data_recv_length_desired = 0;
		wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;
		wifi_data_expecting_new_cid = -1;
	}
}

void wifi_data_next_handle_wait_for_cid(char ndata) {
#ifdef WIFI_DEBUG_PRINT_DATA
	logwohd("%c", ndata);
#endif
	wifi_data_current_cid = wifi_data_hex_to_int(ndata);
	wifi_data_state = WIFI_DATA_WAIT_FOR_LENGTH;
}

void wifi_data_next_handle_wait_for_length(char ndata) {
#ifdef WIFI_DEBUG_PRINT_DATA
	logwohd("%c", ndata);
#endif
	wifi_data_recv_length_desired += wifi_data_hex_to_int(ndata) * wifi_data_lenght_mul[wifi_data_recv_length];
	wifi_data_recv_length++;
	if(wifi_data_recv_length == 4) {
#if LOGGING_LEVEL == LOGGING_DEBUG
		if(wifi_data_recv_length_desired > 80) {
			logwifid("Got recv length desired > 80: %d (1), i: %d, %d\n\r", wifi_data_recv_length_desired, wifi_data_recv_length, ndata);
		}
#endif
		wifi_data_state = WIFI_DATA_WAIT_FOR_DATA;
		wifi_data_recv_length = 0;
#ifdef WIFI_DEBUG_PRINT_DATA
		logwohd(": [");
#endif
	}
}

void wifi_data_next_handle_wait_for_data(char ndata) {
#ifdef WIFI_DEBUG_PRINT_DATA
#if LOGGING_LEVEL == LOGGING_DEBUG
	if(wifi_buffer_size_counter == 0) {
		logwohd("{");
	}
#endif

	logwohd("%x, ", ndata);
#endif
	wifi_buffer_recv[wifi_buffer_size_counter] = ndata;
	wifi_buffer_size_counter++;

	if(wifi_buffer_size_counter > 4) {
		uint16_t length = wifi_buffer_recv[MESSAGE_HEADER_LENGTH_POSITION];
		if(length > 80) {
			logwifie("Got length > 80: %d (1), desired: %d\n\r", length, wifi_data_recv_length_desired);
		}

		if(wifi_buffer_size_counter == length) {
			if(!wifi_data_cid_present[wifi_data_current_cid]) {
				wifi_data_cid_present[wifi_data_current_cid] = true;
				led_on(LED_EXT_BLUE_3);
			}

			wifi_buffer_size_recv = wifi_buffer_size_counter;
			brickd_route_from(wifi_buffer_recv, wifi_data_current_cid);

			wifi_buffer_size_counter = 0;

			// We will force an ACK in 10ms
			wifi_force_ack_counter[wifi_data_current_cid] = 10;

#ifdef WIFI_DEBUG_PRINT_DATA
			logwohd("}\n\r");
#endif
		}
	}

	wifi_data_recv_length++;
	if(wifi_data_recv_length == wifi_data_recv_length_desired) {
		wifi_data_recv_length_desired = 0;
		wifi_data_recv_length = 0;
		wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;
#ifdef WIFI_DEBUG_PRINT_DATA
		logwohd("]\n\r");
#endif
	}
}

char wifi_data_next(const char data, bool transceive) {
	char ndata = data;
	if(!wifi_data_next_handle_ringbuffer(&ndata, transceive)) {
		return ndata;
	}

	if(!wifi_data_next_handle_stuffing(&ndata)) {
		return ndata;
	}

	switch(wifi_data_state) {
		case WIFI_DATA_WAIT_FOR_ESC: wifi_data_next_handle_wait_for_esc(ndata); break;
		case WIFI_DATA_WAIT_FOR_COMMAND_START: wifi_data_next_handle_wait_for_command_start(ndata); break;
		case WIFI_DATA_WAIT_FOR_COMMAND_END: wifi_data_next_handle_wait_for_command_end(ndata); break;
		case WIFI_DATA_WAIT_FOR_ESC_CHAR: wifi_data_next_handle_wait_for_esc_char(ndata); break;
		case WIFI_DATA_WAIT_FOR_ASYNC: wifi_data_next_handle_wait_for_async(ndata); break;
		case WIFI_DATA_WAIT_FOR_ASYNC_READ_SIZE: wifi_data_next_handle_wait_for_async_read_size(ndata); break;
		case WIFI_DATA_WAIT_FOR_ASYNC_READ_MSG: wifi_data_next_handle_wait_for_async_read_msg(ndata); break;
		case WIFI_DATA_WAIT_FOR_CID: wifi_data_next_handle_wait_for_cid(ndata); break;
		case WIFI_DATA_WAIT_FOR_LENGTH: wifi_data_next_handle_wait_for_length(ndata); break;
		case WIFI_DATA_WAIT_FOR_DATA: wifi_data_next_handle_wait_for_data(ndata); break;
		default: logwifiw("Unhandled wifi_data_state: %d\n\r", wifi_data_state); break;
	}
	return ndata;
}

void wifi_data_send(const char *data, const uint16_t length) {
	for(uint16_t i = 0; i < length; i++) {
		char ret1;
		char ret2;
		uint8_t num = wifi_low_level_write_byte_stuffing(data[i], &ret1, &ret2);
		wifi_data_next(ret1, false);

		if((num == 2)) {
			wifi_data_next(ret2, false);
		}
	}
}

void wifi_data_send_escape_cid(const char *data, const uint16_t length, const uint8_t cid) {
	char escape_buffer[WIFI_DATA_ESCAPE_BUFFER_SIZE+1] = {
		0x1B, 'Z',
		wifi_data_int_to_hex(cid),
		'0', '0', '0', '0',
	};

	uint16_t length_tmp = length;
	for(uint8_t i = 0; i < 4; i++) {
		const uint8_t value = length_tmp % 10;
		length_tmp /= 10;
		escape_buffer[WIFI_DATA_ESCAPE_BUFFER_SIZE-1-i] = wifi_data_int_to_hex(value);
	}

	const uint16_t free = wifi_ringbuffer_get_free();

	// Received data wouldn't fit anymore, we have to throw away data :-(
	if(free < (WIFI_DATA_ESCAPE_BUFFER_SIZE+length+2)*2) { // *2 since every byte could be a stuffing byte
		logwifiw("Ringbuffer overflow\n\r");
		wifi_ringbuffer_overflow++;
		return;
	}
	wifi_data_send(escape_buffer, WIFI_DATA_ESCAPE_BUFFER_SIZE);
	wifi_data_send(data, length);

	wifi_force_ack_counter[cid] = -1; // An ACK will be transmitted with the send here

#ifdef WIFI_DEBUG_PRINT_DATA
#if LOGGING_LEVEL == LOGGING_DEBUG
	if(!wifi_data_cid_present[cid]) {
		logwifid("CID not present anymore... %d\n\r", cid);
	}
	logwifid("Data Send: <ESC>%s: [", escape_buffer+1);
	for(uint8_t i = 0; i < length; i++) {
		logwohd("%x, ", data[i]);
	}
	logwohd("]\n\r");
#endif
#endif
}

void wifi_data_send_escape(const char *data, const uint16_t length) {
	EnumerateCallback *data_enum = (EnumerateCallback*)data;

	int8_t cid = -1;

	if(wifi_new_cid != -1 &&
	   data_enum->header.fid == FID_ENUMERATE_CALLBACK &&
	   data_enum->enumeration_type == ENUMERATE_TYPE_ADDED) {
		// Handle initial enumeration (only send to new socket)
		cid = wifi_new_cid;
	} else {
		cid = brickd_route_to((const uint8_t*)data);
	}

	if(cid == -1) {
		for(uint8_t i = 1; i < WIFI_DATA_MAX_CID; i++) {
			if(wifi_data_cid_present[i]) {
				wifi_data_send_escape_cid(data, length, i);
			}
		}
		return;
	} else {
		if(!wifi_data_cid_present[cid]) {
			return;
		}
	}

	wifi_data_send_escape_cid(data, length, cid);
}

void wifi_data_poll(void) {
	while((PIO_Get(&extension_pins[WIFI_DATA_RDY]) ||
		  (!wifi_ringbuffer_is_empty())) &&
	      (wifi_buffer_size_recv == 0)) {
		wifi_data_next(WIFI_LOW_LEVEL_SPI_IDLE_CHAR, true);
	}
}

int8_t wifi_data_hex_to_int(const char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}

	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}

	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}

	return -1;
}

char wifi_data_int_to_hex(const int8_t c) {
	if (c >= 0 && c <= 9) {
		return c + '0';
	}
	else if (c >= 10 && c <= 15) {
		return c + 'A' - 10;
	}

	return 'X';
}

void wifi_data_disconnect(const uint8_t cid) {
	if(cid > 0 && cid < 16) {
		wifi_data_cid_present[cid] = false;
		brickd_disconnect(cid);
	}
}
