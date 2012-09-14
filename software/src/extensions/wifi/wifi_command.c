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
	WIFI_COMMAND_AT_WEAP
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
	sizeof(WIFI_COMMAND_AT_WEAP)-1
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
			char str[33] = {'\0'};

			uint8_t type = (wifi_configuration.eap_options >> 3) & 0b00000011;

			sprintf(str, "%d,0,%d,1\n\r%cW", type, wifi_configuration.certificate_length, 0x1B);
			wifi_low_level_write_buffer(str, strlen(str));

			uint16_t i;
			str[32] = '\0';
			for(i = 0; i < wifi_configuration.certificate_length; i+=32) {
				wifi_read_config(str, 32, WIFI_CERTIFICATE + i);
				if(i + 32 < wifi_configuration.certificate_length) {
					wifi_low_level_write_buffer(str, 32);
				} else {
					wifi_low_level_write_buffer(str, wifi_configuration.certificate_length % 32);
				}
			}

			break;
		}

		default: {
			break;
		}
	}

	wifi_low_level_write_buffer("\r\n", 2);
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
					//while(!PIO_Get(&pins_wifi_spi[WIFI_DATA_RDY]));
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
			wifi_status.state = WIFI_STATE_DISASSOCIATED;
			wifi_data_disconnect(wifi_data_hex_to_int(data[2]));
			return WIFI_ANSWER_DISCONNECT;
		}

		case '9': {
			logwifii("disassociated\n\r");
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
	const uint8_t length = wifi_command_recv(data, 255, WIFI_COMMAND_RECV_TIMEOUT);
	return wifi_command_parse(data, length);
}

void wifi_command_flush(void) {
	for(uint8_t i = 0; i < 255; i++) {
		wifi_low_level_read_byte();
	}
}
