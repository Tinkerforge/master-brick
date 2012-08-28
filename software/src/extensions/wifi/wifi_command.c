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
#include <stdio.h>

#include "extensions/extension_i2c.h"
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
	WIFI_COMMAND_AT_ATV1,
	WIFI_COMMAND_AT_WWPA,
	WIFI_COMMAND_AT_WAUTO,
	WIFI_COMMAND_AT_NAUTO,
	WIFI_COMMAND_AT_NSTCP,
	WIFI_COMMAND_AT_WA,
	WIFI_COMMAND_AT_NSET,
	WIFI_COMMAND_AT_WAUTH_WEP,
	WIFI_COMMAND_AT_WWEP,
	WIFI_COMMAND_AT_SPICONF,
	WIFI_COMMAND_AT_WRSSI,
	WIFI_COMMAND_AT_NSTAT
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
	sizeof(WIFI_COMMAND_AT_ATV1)-1,
	sizeof(WIFI_COMMAND_AT_WWPA)-1,
	sizeof(WIFI_COMMAND_AT_WAUTO)-1,
	sizeof(WIFI_COMMAND_AT_NAUTO)-1,
	sizeof(WIFI_COMMAND_AT_NSTCP)-1,
	sizeof(WIFI_COMMAND_AT_WA)-1,
	sizeof(WIFI_COMMAND_AT_NSET)-1,
	sizeof(WIFI_COMMAND_AT_WAUTH_WEP)-1,
	sizeof(WIFI_COMMAND_AT_WWEP)-1,
	sizeof(WIFI_COMMAND_AT_SPICONF)-1,
	sizeof(WIFI_COMMAND_AT_WRSSI)-1,
	sizeof(WIFI_COMMAND_AT_NSTAT)-1
};

extern uint8_t wifi_state;

extern WifiConfiguration wifi_configuration;

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
			uint8_t length;
			for(length = 0; length < 51; length++) {
				if(wifi_configuration.key[length] == '\0') {
					break;
				}
			}
			wifi_low_level_write_buffer(wifi_configuration.key, length);
			break;
		}

		case WIFI_COMMAND_ID_AT_WAUTO: {
			uint8_t length;
			for(length = 0; length < 33; length++) {
				if(wifi_configuration.ssid[length] == '\0') {
					break;
				}
			}
			wifi_low_level_write_buffer("0,", 2);
			wifi_low_level_write_buffer(wifi_configuration.ssid, length);
			wifi_low_level_write_buffer(",,0", 3);
			break;
		}

		case WIFI_COMMAND_ID_AT_NAUTO: {
			char str[10];
			sprintf(str, "1,1,,%d", wifi_configuration.port);
			uint8_t length = 9;
			if(wifi_configuration.port >= 10000) {
				length = 10;
			}
			wifi_low_level_write_buffer(str, length);
			break;
		}

		case WIFI_COMMAND_ID_AT_NSTCP: {
			char str[5];
			sprintf(str, "%d", wifi_configuration.port);
			uint8_t length = 4;
			if(wifi_configuration.port >= 10000) {
				length = 5;
			}
			wifi_low_level_write_buffer(str, length);
			break;
		}

		case WIFI_COMMAND_ID_AT_WA: {
			uint8_t length;
			for(length = 0; length < 33; length++) {
				if(wifi_configuration.ssid[length] == '\0') {
					break;
				}
			}
			wifi_low_level_write_buffer(wifi_configuration.ssid, length);
			break;
		}

		case WIFI_COMMAND_ID_AT_NSET: {
			char str[2 + (4*3 + 3)*3 + 1] = {'\0'};
			sprintf(str,
			        "%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d",
			        wifi_configuration.ip[3],
			        wifi_configuration.ip[2],
			        wifi_configuration.ip[1],
			        wifi_configuration.ip[0],
			        wifi_configuration.subnet_mask[3],
			        wifi_configuration.subnet_mask[2],
			        wifi_configuration.subnet_mask[1],
			        wifi_configuration.subnet_mask[0],
			        wifi_configuration.gateway[3],
			        wifi_configuration.gateway[2],
			        wifi_configuration.gateway[1],
			        wifi_configuration.gateway[0]);

			wifi_low_level_write_buffer(str, strlen(str));
		}

		case WIFI_COMMAND_ID_AT_WWEP: {
			char str[64] = {'\0'};
			sprintf(str,
			        "%d=%s",
			        wifi_configuration.key_index,
			        wifi_configuration.key);

			wifi_low_level_write_buffer(str, strlen(str));
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
	if(length == 0) {
		return WIFI_ANSWER_NO_ANSWER;
	}

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
		uint8_t b = wifi_low_level_read_byte();
	}

	wifi_state = WIFI_STATE_COMMAND_IDLE;
}
