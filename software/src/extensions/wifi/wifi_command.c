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

#include "bricklib/logging/logging.h"
#include "bricklib/drivers/pio/pio.h"

#include "extensions/extension_i2c.h"
#include "wifi.h"
#include "wifi_data.h"
#include "wifi_low_level.h"

extern WifiStatus wifi_status;
extern int8_t wifi_new_cid;

uint8_t eap_type = 0xFF;

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
	WIFI_COMMAND_AT_WAUTH_OPEN,
	WIFI_COMMAND_AT_WWEP,
	WIFI_COMMAND_AT_SPICONF,
	WIFI_COMMAND_AT_WRSSI,
	WIFI_COMMAND_AT_NSTAT,
	WIFI_COMMAND_AT_VER,
	WIFI_COMMAND_AT_PSPOLLINTRL,
	WIFI_COMMAND_AT_WEAPCONF,
	WIFI_COMMAND_AT_WEAP,
	WIFI_COMMAND_AT_WSYNCINTRL,
	WIFI_COMMAND_AT_ASYNCMSGFMT,
	WIFI_COMMAND_AT_F,
	WIFI_COMMAND_AT_SETSOCKOPT_SO,
	WIFI_COMMAND_AT_SETSOCKOPT_TC,
	WIFI_COMMAND_AT_ATS
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
	sizeof(WIFI_COMMAND_AT_WAUTH_OPEN)-1,
	sizeof(WIFI_COMMAND_AT_WWEP)-1,
	sizeof(WIFI_COMMAND_AT_SPICONF)-1,
	sizeof(WIFI_COMMAND_AT_WRSSI)-1,
	sizeof(WIFI_COMMAND_AT_NSTAT)-1,
	sizeof(WIFI_COMMAND_AT_VER)-1,
	sizeof(WIFI_COMMAND_AT_PSPOLLINTRL)-1,
	sizeof(WIFI_COMMAND_AT_WEAPCONF)-1,
	sizeof(WIFI_COMMAND_AT_WEAP)-1,
	sizeof(WIFI_COMMAND_AT_WSYNCINTRL)-1,
	sizeof(WIFI_COMMAND_AT_ASYNCMSGFMT)-1,
	sizeof(WIFI_COMMAND_AT_F)-1,
	sizeof(WIFI_COMMAND_AT_SETSOCKOPT_SO)-1,
	sizeof(WIFI_COMMAND_AT_SETSOCKOPT_TC)-1,
	sizeof(WIFI_COMMAND_AT_ATS)-1
};

extern WifiConfiguration wifi_configuration;
extern Pin pins_wifi_spi[];

void wifi_command_send(const WIFICommand command) {
	wifi_low_level_write_buffer(wifi_command_str[command],
	                            wifi_command_length[command]);

	switch(command) {
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
			break;
		}

		case WIFI_COMMAND_ID_AT_WWEP: {
			char str[64] = {'\0'};
			sprintf(str,
			        "%d=%s",
			        wifi_configuration.key_index,
			        wifi_configuration.key);

			wifi_low_level_write_buffer(str, strlen(str));
			break;
		}

		case WIFI_COMMAND_ID_AT_WEAPCONF: {
			char str[72] = {'\0'};
			uint8_t outer = 0;
			switch(wifi_configuration.eap_options & 0b00000011) {
				case 0:
					outer = 43;
					break;
				case 1:
					outer = 13;
					break;
				case 2:
					outer = 21;
					break;
				case 3:
					outer = 25;
					break;
			}

			uint8_t inner = 0;
			switch((wifi_configuration.eap_options >> 2) & 0b00000001) {
				case 0:
					inner = 26;
					break;
				case 1:
					inner = 6;
					break;
			}

			uint8_t printed = sprintf(str, "%d,%d,", outer, inner);

			wifi_read_config(&str[printed], 32, WIFI_USERNAME_POS);
			printed = strlen(str);
			str[printed] = ',';

			wifi_read_config(&str[printed+1], 32, WIFI_PASSWORD_POS);

			wifi_low_level_write_buffer(str, strlen(str));

			break;
		}

		case WIFI_COMMAND_ID_AT_WEAP: {
			char str[13] = {'\0'};

			uint16_t length;
			switch(eap_type) {
				case 0: {
					length = wifi_configuration.ca_certificate_length;
					break;
				}

				case 1: {
					length = wifi_configuration.client_certificate_length;
					break;
				}

				case 2: {
					length = wifi_configuration.private_key_length;
					break;
				}

				default: {
					return;
				}
			}

			sprintf(str, "%d,0,%d,1", eap_type, length);
			wifi_low_level_write_buffer(str, strlen(str));

			break;
		}

		case WIFI_COMMAND_ID_AT_SETSOCKOPT_SO: {
			char str[20] = {'\0'};

			sprintf(str, "%d,65535,8,1,4", wifi_new_cid);
			wifi_low_level_write_buffer(str, strlen(str));

			break;
		}

		case WIFI_COMMAND_ID_AT_SETSOCKOPT_TC: {
			char str[20] = {'\0'};

			sprintf(str, "%d,6,4001,600,4", wifi_new_cid);
			wifi_low_level_write_buffer(str, strlen(str));

			break;
		}

		case WIFI_COMMAND_ID_AT_ATS: {
			char str[20] = {'\0'};
			sprintf(str, "%d=1", 65000);
			wifi_low_level_write_buffer(str, strlen(str));
		}

		default: {
			break;
		}
	}

	wifi_low_level_write_buffer("\r\n", 2);
}

void wifi_write_eap(void) {
	uint16_t length;
	uint16_t offset;
	switch(eap_type) {
		case 0: {
			length = wifi_configuration.ca_certificate_length;
			offset = WIFI_CA_CERTIFICATE_POS;
			break;
		}

		case 1: {
			length = wifi_configuration.client_certificate_length;
			offset = WIFI_CLIENT_CERTIFICATE_POS;
			break;
		}

		case 2: {
			length = wifi_configuration.private_key_length;
			offset = WIFI_PRIVATE_KEY_POS;
			break;
		}

		default: {
			return;
		}
	}

	char str[33] = {'\0'};

	str[0] = 0x1B;
	str[1] = 'W';
	wifi_low_level_write_buffer(str, 2);

	uint16_t i;
	str[32] = '\0';
	for(i = 0; i < length; i+=32) {
		wifi_read_config(str, 32, offset + i);
		if(i + 32 < length) {
			wifi_low_level_write_buffer(str, 32);
		} else {
			wifi_low_level_write_buffer(str, length % 32);
		}
	}

	wifi_command_recv_and_parse();

	eap_type = 0xFF;
}

uint8_t wifi_command_recv(char *data, const uint8_t length, uint32_t timeout) {
	uint8_t i = 0;
	uint8_t last_byte = 0;

	for(uint32_t counter = 0; counter < timeout; counter++) {
		if(!PIO_Get(&pins_wifi_spi[WIFI_DATA_RDY])) {
			continue;
		}
		uint8_t b = wifi_low_level_read_byte();
		if(b == '\r' || b == '\n') {
			if(i > 0 && last_byte == '\r' && b == '\n') {
				return i;
			}
			last_byte = b;
		} else {
			if(wifi_low_level_is_byte_stuffing(b)) {
				// TODO: handle XON/XOFF etc

				if(b == WIFI_LOW_LEVEL_SPI_ESC_CHAR) {
					while(!PIO_Get(&pins_wifi_spi[WIFI_DATA_RDY]));
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
		return WIFI_ANSWER_TIMEOUT;
	}

	switch(data[0]) {
		case '0': {
			return WIFI_ANSWER_OK;
		}

		case '3':
		case '4':
		case '5':
		case '6':
		case '1': {
			return WIFI_ANSWER_ERROR;
		}

		case '2': {
			return WIFI_ANSWER_INVALD_COMMAND;
		}

		case '7': {
			return WIFI_ANSWER_CONNECT;
		}

		case '8': {
			logwifii("8: Disassociated\n\r");
			wifi_status.state = WIFI_STATE_DISASSOCIATED;
			wifi_data_disconnect(wifi_data_hex_to_int(data[2]));
			return WIFI_ANSWER_DISCONNECT;
		}

		case '9': {
			logwifii("9: Disassociated\n\r");
			wifi_status.state = WIFI_STATE_DISASSOCIATED;
			return WIFI_ANSWER_DISASSOCIATED;
		}

		default: {
			if(data[0] == 'O' && data[1] == 'K') {
				return WIFI_ANSWER_OK;
			}
			logwifid("parse default: %c %d\n\r", data[0], data[0]);
			logwifid("parse default str: %s\n\r", data);
			return WIFI_ANSWER_NO_ANSWER;
		}
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
	const uint8_t length = wifi_command_recv(data, 254, WIFI_COMMAND_RECV_TIMEOUT);
	data[length] = '\0';
	return wifi_command_parse(data, length);
}

void wifi_command_flush(void) {
	for(uint8_t i = 0; i < 255; i++) {
		wifi_low_level_read_byte();
	}
}
