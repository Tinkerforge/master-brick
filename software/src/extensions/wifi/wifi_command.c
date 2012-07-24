/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_command.c: Command mode functionality for WIFI Extension
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

#include "wifi_command.h"

#include <string.h>

#include "wifi.h"
#include "wifi_low_level.h"

static const char *wifi_command_str[] = {
	WIFI_COMMAND_AT,
	WIFI_COMMAND_AT_WD,
	WIFI_COMMAND_AT_ATA,
	WIFI_COMMAND_AT_ATO,
	WIFI_COMMAND_AT_ATC_ON,
	WIFI_COMMAND_AT_ATC_OFF,
	WIFI_COMMAND_AT_NDHCP_ON,
	WIFI_COMMAND_AT_NDHCP_OFF,
	WIFI_COMMAND_AT_BDATA_ON,
	WIFI_COMMAND_AT_BDATA_OFF,
	WIFI_COMMAND_AT_WRXACTIVE_ON,
	WIFI_COMMAND_AT_WRXACTIVE_OFF,
	WIFI_COMMAND_AT_WRXPS_ON,
	WIFI_COMMAND_AT_WRXPS_OFF,
	WIFI_COMMAND_AT_ATE0,
	WIFI_COMMAND_AT_ATV0,
	WIFI_COMMAND_AT_WWPA,
	WIFI_COMMAND_AT_WAUTO,
	WIFI_COMMAND_AT_NAUTO,
	WIFI_COMMAND_AT_NSTCP,
	WIFI_COMMAND_AT_WA
};

static const uint8_t wifi_command_length[] = {
	sizeof(WIFI_COMMAND_AT)-1,
	sizeof(WIFI_COMMAND_AT_WD)-1,
	sizeof(WIFI_COMMAND_AT_ATA)-1,
	sizeof(WIFI_COMMAND_AT_ATO)-1,
	sizeof(WIFI_COMMAND_AT_ATC_ON)-1,
	sizeof(WIFI_COMMAND_AT_ATC_OFF)-1,
	sizeof(WIFI_COMMAND_AT_NDHCP_ON)-1,
	sizeof(WIFI_COMMAND_AT_NDHCP_OFF)-1,
	sizeof(WIFI_COMMAND_AT_BDATA_ON)-1,
	sizeof(WIFI_COMMAND_AT_BDATA_OFF)-1,
	sizeof(WIFI_COMMAND_AT_WRXACTIVE_ON)-1,
	sizeof(WIFI_COMMAND_AT_WRXACTIVE_OFF)-1,
	sizeof(WIFI_COMMAND_AT_WRXPS_ON)-1,
	sizeof(WIFI_COMMAND_AT_WRXPS_OFF)-1,
	sizeof(WIFI_COMMAND_AT_ATE0)-1,
	sizeof(WIFI_COMMAND_AT_ATV0)-1,
	sizeof(WIFI_COMMAND_AT_WWPA)-1,
	sizeof(WIFI_COMMAND_AT_WAUTO)-1,
	sizeof(WIFI_COMMAND_AT_NAUTO)-1,
	sizeof(WIFI_COMMAND_AT_NSTCP)-1,
	sizeof(WIFI_COMMAND_AT_WA)-1
};

extern uint8_t wifi_state;

extern char wifi_ssid[];
extern char wifi_wpa_key[];
extern uint8_t wifi_wpa_key_length;
extern uint8_t wifi_ssid_length;

void wifi_command_send(const WIFICommand command) {
	// TODO: flush?
	if(wifi_state != WIFI_STATE_COMMAND_IDLE) {
		return;
	}

	wifi_state = WIFI_STATE_COMMAND_SEND;

	wifi_low_level_write_buffer(wifi_command_str[command],
	                            wifi_command_length[command]);

	switch(command) {
		case WIFI_COMMAND_ID_AT:
		case WIFI_COMMAND_ID_AT_WD:
		case WIFI_COMMAND_ID_AT_ATA:
		case WIFI_COMMAND_ID_AT_ATO:
		case WIFI_COMMAND_ID_AT_ATC_ON:
		case WIFI_COMMAND_ID_AT_ATC_OFF:
		case WIFI_COMMAND_ID_AT_NDHCP_ON:
		case WIFI_COMMAND_ID_AT_NDHCP_OFF: {
			break;
		}

		case WIFI_COMMAND_ID_AT_WWPA: {
			wifi_low_level_write_buffer(wifi_wpa_key, wifi_wpa_key_length);
			break;
		}

		case WIFI_COMMAND_ID_AT_WAUTO: {
			wifi_low_level_write_buffer("0,", 2);
			wifi_low_level_write_buffer(wifi_ssid, wifi_ssid_length);
			wifi_low_level_write_buffer(",,0", 3);
			break;
		}

		case WIFI_COMMAND_ID_AT_NAUTO: {
			wifi_low_level_write_buffer("1,1,,4223", 9);
			break;
		}

		case WIFI_COMMAND_ID_AT_NSTCP: {
			wifi_low_level_write_buffer("4223", 4);
			break;
		}

		case WIFI_COMMAND_ID_AT_WA: {
			wifi_low_level_write_buffer(wifi_ssid, wifi_ssid_length);
			break;
		}

		default: {
			break;
		}
	}

	wifi_low_level_write_buffer("\r\n", 2);

	wifi_state = WIFI_STATE_COMMAND_IDLE;
}

uint8_t wifi_command_recv(char *data, const uint8_t length) {
	if(wifi_state != WIFI_STATE_COMMAND_IDLE) {
		return 0;
	}

	wifi_state = WIFI_STATE_COMMAND_RECV;

	uint8_t i = 0;
	uint8_t last_byte = 0;

	while(true) {
		uint8_t b = wifi_low_level_read_byte();
		if(b == '\r' || b == '\n') {
			if(i > 0 && last_byte == '\r' && b == '\n') {
				wifi_state = WIFI_STATE_COMMAND_IDLE;
				return i;
			}
			last_byte = b;
		} else {
			if(wifi_low_level_is_byte_stuffing(b)) {
				// TODO: handle XON/XOFF etc


				if(b == WIFI_LOW_LEVEL_SPI_ESC_CHAR) {
					b = wifi_low_level_read_byte() ^ 0x20;
				} else {
					last_byte = b;
					continue;
				}
			}

			if(b != 0) {
				data[i] = b;
				i++;

				if(i == length) {
					wifi_state = WIFI_STATE_COMMAND_IDLE;
					return i;
				}
			}
			last_byte = b;
		}
	}

	return 0;
}

uint8_t wifi_command_parse(const char *data, const uint8_t length) {
	switch(data[0]) {
		case '0': return WIFI_ANSWER_OK;
		case '1': return WIFI_ANSWER_ERROR;
		case '7': return WIFI_ANSWER_CONNECT;
		case '8': return WIFI_ANSWER_DISCONNECT;
		default:  return WIFI_ANSWER_NO_ANSWER;
	}
}

uint8_t wifi_command_send_recv_and_parse(const WIFICommand command) {
	wifi_command_send(command);
	uint8_t answer = WIFI_ANSWER_NO_ANSWER;
	while(answer == WIFI_ANSWER_NO_ANSWER) {
		answer = wifi_command_recv_and_parse();
	}
	return answer;
}

uint8_t wifi_command_recv_and_parse(void) {
	char data[255];
	const uint8_t length = wifi_command_recv(data, 255);
	return wifi_command_parse(data, length);
}

void wifi_command_flush(void) {
	if(wifi_state != WIFI_STATE_COMMAND_IDLE) {
		return;
	}

	wifi_state = WIFI_STATE_COMMAND_RECV;

	for(uint8_t i = 0; i < 255; i++) {
		wifi_low_level_read_byte();
	}

	wifi_state = WIFI_STATE_COMMAND_IDLE;
}