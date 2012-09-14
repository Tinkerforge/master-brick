/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
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
#include "wifi_command.h"
#include "wifi_brickd.h"

#include "bricklib/com/com_messages.h"
#include "bricklib/utility/led.h"

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

WIFIDataState wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;
uint16_t wifi_data_recv_length_desired = 0;
uint16_t wifi_data_recv_length = 0;
uint16_t wifi_buffer_size_counter = 0;
int8_t  wifi_data_current_cid = -1;
char wifi_data_length_buffer[4] = {0};
extern uint8_t wifi_buffer_recv[];
extern uint16_t wifi_buffer_size_recv;
extern Pin pins_wifi_spi[];

uint16_t wifi_data_lenght_mul[4] = {1000, 100, 10, 1};

bool wifi_data_currently_stuffing = false;

bool wifi_data_cid_present[WIFI_DATA_MAX_CID] = {false};


void wifi_data_next(const char data) {
	char ndata = data;
	if(wifi_data_currently_stuffing) {
		ndata ^= 0x20;
		wifi_data_currently_stuffing = false;
	} else if(wifi_low_level_is_byte_stuffing(ndata)) {
		if(ndata == WIFI_LOW_LEVEL_SPI_ESC_CHAR) {
			wifi_data_currently_stuffing = true;
		}
		return;
	}

	switch(wifi_data_state) {
		case WIFI_DATA_WAIT_FOR_ESC: {
			if(ndata == WIFI_DATA_CHAR_ESC) {
				wifi_data_state = WIFI_DATA_WAIT_FOR_ESC_CHAR;
			} else if(ndata == '\r') {
				// It seems we are receiving a command
				wifi_command_recv_and_parse();
			}

			break;
		}

		case WIFI_DATA_WAIT_FOR_ESC_CHAR: {
			if(ndata == WIFI_DATA_CHAR_Z) {
				wifi_data_state = WIFI_DATA_WAIT_FOR_CID;
				wifi_data_recv_length = 0;
				wifi_data_recv_length_desired = 0;
			} else {
				wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;
			}
			break;
		}

		case WIFI_DATA_WAIT_FOR_CID: {
			wifi_data_current_cid = wifi_data_hex_to_int(ndata);
			wifi_data_state = WIFI_DATA_WAIT_FOR_LENGTH;
			break;
		}

		case WIFI_DATA_WAIT_FOR_LENGTH: {
			wifi_data_recv_length_desired += wifi_data_hex_to_int(ndata) * wifi_data_lenght_mul[wifi_data_recv_length];
			wifi_data_recv_length++;
			if(wifi_data_recv_length == 4) {
				wifi_data_state = WIFI_DATA_WAIT_FOR_DATA;
				wifi_data_recv_length = 0;
			}

			break;
		}

		case WIFI_DATA_WAIT_FOR_DATA: {
			wifi_buffer_recv[wifi_buffer_size_counter] = ndata;
			wifi_buffer_size_counter++;
			wifi_data_recv_length++;
			if(wifi_data_recv_length == wifi_data_recv_length_desired) {
				wifi_data_recv_length = 0;
				wifi_data_state = WIFI_DATA_WAIT_FOR_ESC;
			}
			if(wifi_buffer_size_counter >= 4) {
				uint16_t length = get_length_from_data((const char*)wifi_buffer_recv);
				if(wifi_buffer_size_counter == length) {
					if(!wifi_data_cid_present[wifi_data_current_cid]) {
						wifi_data_cid_present[wifi_data_current_cid] = true;
						led_on(LED_EXT_BLUE_3);
					}
					wifi_buffer_size_recv = wifi_buffer_size_counter;
					wifi_buffer_size_counter = 0;
					wifi_brickd_route_from(wifi_buffer_recv, wifi_data_current_cid);
				}
			}
			break;
		}
	}
}

void wifi_data_send(const char *data, const uint16_t length) {
	for(uint16_t i = 0; i < length; i++) {
		char ret1;
		char ret2;
		uint8_t num = wifi_low_level_write_byte_stuffing(data[i], &ret1, &ret2);

		if((ret1 != WIFI_LOW_LEVEL_SPI_IDLE_CHAR) &&
		   (ret1 != WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ZERO) &&
		   (ret1 != WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ONE)) {
			wifi_data_next(ret1);
		}

		if((num == 2)) {
			if((ret2 != WIFI_LOW_LEVEL_SPI_IDLE_CHAR) &&
			   (ret2 != WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ZERO) &&
			   (ret2 != WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ONE)) {
				wifi_data_next(ret2);
			}
		}
	}
}

void wifi_data_send_escape_cid(const char *data, const uint16_t length, const uint8_t cid) {
	char escape_buffer[WIFI_DATA_ESCAPE_BUFFER_SIZE] = {
		0x1B, 'Z',
		wifi_data_int_to_hex(cid),
		'0', '0', '0', '0'
	};

	uint16_t length_tmp = length;
	for(uint8_t i = 0; i < 4; i++) {
		const uint8_t value = length_tmp % 10;
		length_tmp /= 10;
		escape_buffer[WIFI_DATA_ESCAPE_BUFFER_SIZE-1-i] = wifi_data_int_to_hex(value);
	}

	wifi_data_send(escape_buffer, WIFI_DATA_ESCAPE_BUFFER_SIZE);
	wifi_data_send(data, length);
}

void wifi_data_send_escape(const char *data, const uint16_t length) {
	int8_t cid = wifi_brickd_route_to((const uint8_t*)data);
	if(cid == -1) {
		for(uint8_t i = 1; i < WIFI_DATA_MAX_CID; i++) {
			if(wifi_data_cid_present[i]) {
				wifi_data_send_escape_cid(data, length, i);
			}
		}
		return;
	}

	wifi_data_send_escape_cid(data, length, cid);
}

void wifi_data_poll(void) {
	uint8_t i = 0;
	if(wifi_buffer_size_recv != 0i/* || !PIO_Get(&pins_wifi_spi[WIFI_DATA_RDY])*/) {
		return;
	}

	while(((wifi_buffer_size_recv == 0) && (i < 100)) ||
	      (wifi_data_state != WIFI_DATA_WAIT_FOR_ESC)) {
		wifi_data_next(wifi_low_level_write_byte(WIFI_LOW_LEVEL_SPI_IDLE_CHAR));
		i++;
	}
}

int8_t wifi_data_hex_to_int(char c) {
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

char wifi_data_int_to_hex(int8_t c) {
	if (c >= 0 && c <= 9) {
		return c + '0';
	}
	else if (c >= 10 && c <= 15) {
		return c + 'A' - 10;
	}

	return 'X';
}

void wifi_data_disconnect(uint8_t cid) {
	if(cid > 0 && cid < 16) {
		wifi_data_cid_present[cid] = false;
		wifi_brickd_disconnect(cid);
	}
}
