/* master-brick
 * Copyright (C) 2012-2013 Olaf Lüke <olaf@tinkerforge.com>
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bricklib/logging/logging.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/utility/util_definitions.h"

#include "extensions/extension_i2c.h"
#include "wifi.h"
#include "wifi_data.h"
#include "wifi_low_level.h"

extern WifiStatus wifi_status;

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
	WIFI_COMMAND_AT_ATS,
	WIFI_COMMAND_AT_WM_IFACE,
	WIFI_COMMAND_AT_WM_ADHOC,
	WIFI_COMMAND_AT_WM_AP,
	WIFI_COMMAND_AT_WREGDOMAIN,
	WIFI_COMMAND_AT_WRETRY,
	WIFI_COMMAND_AT_WSEC
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
	sizeof(WIFI_COMMAND_AT_ATS)-1,
	sizeof(WIFI_COMMAND_AT_WM_IFACE)-1,
	sizeof(WIFI_COMMAND_AT_WM_ADHOC)-1,
	sizeof(WIFI_COMMAND_AT_WM_AP)-1,
	sizeof(WIFI_COMMAND_AT_WREGDOMAIN)-1,
	sizeof(WIFI_COMMAND_AT_WRETRY)-1,
	sizeof(WIFI_COMMAND_AT_WSEC)-1
};

extern WifiConfiguration wifi_configuration;

WIFICommand wifi_command_parse_next = WIFI_COMMAND_ID_NONE;

void wifi_command_send_at_wwpa(void) {
	uint8_t length;
	char key[64];

	wifi_data_send("\"", 1);
	if(!(wifi_configuration.key[0] == '-' && wifi_configuration.key[1] == '\0')) {
		for(length = 0; length < sizeof(wifi_configuration.key); length++) {
			if(wifi_configuration.key[length] == '\0') {
				break;
			}
		}
		wifi_data_send(wifi_configuration.key, length);
	} else {
		wifi_read_config(key, sizeof(key), WIFI_LONG_KEY_POS);
		for(length = 0; length < sizeof(key); length++) {
			if(key[length] == '\0') {
				break;
			}
		}
		wifi_data_send(key, length);
	}
	wifi_data_send("\"", 1);

#if LOGGING_LEVEL == LOGGING_DEBUG
	logwohd("\"");
	if(!(wifi_configuration.key[0] == '-' && wifi_configuration.key[1] == '\0')) {
		for(uint8_t i = 0; i < length; i++) {
			logwohd("%c", wifi_configuration.key[i]);
		}
	} else {
		for(uint8_t i = 0; i < length; i++) {
			logwohd("%c", key[i]);
		}
	}
	logwohd("\"");
#endif
}

void wifi_command_send_at_auto(void) {
	uint8_t length;
	for(length = 0; length < sizeof(wifi_configuration.ssid); length++) {
		if(wifi_configuration.ssid[length] == '\0') {
			break;
		}
	}
	wifi_data_send("0,\"", 2);
	wifi_data_send(wifi_configuration.ssid, length);
	wifi_data_send("\",,0", 3);
#if LOGGING_LEVEL == LOGGING_DEBUG
	logwohd("0,\"");
	for(uint8_t i = 0; i < length; i++) {
		logwohd("%c", wifi_configuration.ssid[i]);
	}
	logwohd("\",,0");
#endif
}

void wifi_command_send_at_wauto(void) {
	char str[10];
	sprintf(str, "1,1,,%d", wifi_configuration.port);
	uint8_t length = 9;
	if(wifi_configuration.port >= 10000) {
		length = 10;
	}

	wifi_data_send(str, length);
#if LOGGING_LEVEL == LOGGING_DEBUG
	for(uint8_t i = 0; i < length; i++) {
		logwohd("%c", str[i]);
	}
#endif
}

void wifi_command_send_at_nstcp(void) {
	char str[5];
	sprintf(str, "%d", wifi_configuration.port);
	uint8_t length = 4;
	if(wifi_configuration.port >= 10000) {
		length = 5;
	}
	wifi_data_send(str, length);
#if LOGGING_LEVEL == LOGGING_DEBUG
	for(uint8_t i = 0; i < length; i++) {
		logwohd("%c", str[i]);
	}
#endif
}

void wifi_command_send_at_wa(void) {
	uint8_t length;
	for(length = 0; length < 33; length++) {
		if(wifi_configuration.ssid[length] == '\0') {
			break;
		}
	}
	wifi_data_send("\"", 1);
	wifi_data_send(wifi_configuration.ssid, length);
	wifi_data_send("\"", 1);
#if LOGGING_LEVEL == LOGGING_DEBUG
	logwohd("\"");
	for(uint8_t i = 0; i < length; i++) {
		logwohd("%c", wifi_configuration.ssid[i]);
	}
	logwohd("\"");
#endif
}

void wifi_command_send_at_nset(void) {
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

	wifi_data_send(str, strlen(str));
	logwohd("%s", str);
}

void wifi_command_send_at_wwep(void) {
	char str[64] = {'\0'};
	sprintf(str,
	        "%d=%s",
	        wifi_configuration.key_index,
	        wifi_configuration.key);

	wifi_data_send(str, strlen(str));
	logwohd("%s", str);
}

void wifi_command_send_at_weapconf(void) {
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

	wifi_data_send(str, strlen(str));
	logwohd("%s", str);
}

void wifi_command_send_at_weap(void) {
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
	wifi_data_send(str, strlen(str));
	logwohd("%s", str);
}

void wifi_command_send_at_ats(void) {
	const char str[4] = "4=0";
	wifi_data_send(str, strlen(str));
	logwohd("%s", str);
}

void wifi_command_send_at_wregdomain(void) {
	if(wifi_configuration.regulatory_domain > 3) {
		wifi_configuration.regulatory_domain = 1;
	}

	char str = '0' + wifi_configuration.regulatory_domain;
	wifi_data_send(&str, 1);
	logwohd("%c", str);
}

void wifi_command_send_at_ndhcp(void) {
	WIFIHostname wifi_hostname;
	wifi_read_config((char*)&wifi_hostname, sizeof(WIFIHostname), WIFI_HOSTNAME_POS);
	if(wifi_hostname.key == WIFI_KEY) {
		uint8_t length;
		for(length = 0; length < 16; length++) {
			if(wifi_hostname.hostname[length] == '\0') {
				break;
			}
		}
		if(length > 0) {
			const char comma = ',';
			wifi_data_send(&comma, 1);
			wifi_data_send(wifi_hostname.hostname, length);
#if LOGGING_LEVEL == LOGGING_DEBUG
			logwohd(",");
			for(uint8_t i = 0; i < length; i++) {
				logwohd("%c", wifi_hostname.hostname[i]);
			}
#endif
		}
	}
}

void wifi_command_send(const WIFICommand command) {
	wifi_command_parse_next = command;
	wifi_data_send(wifi_command_str[command],
	               wifi_command_length[command]);

#if LOGGING_LEVEL == LOGGING_DEBUG
	logwifid("Command send (%d): ", command);
	for(uint8_t i = 0; i < wifi_command_length[command]; i++) {
		logwohd("%c", wifi_command_str[command][i]);
	}
#endif

	switch(command) {
		case WIFI_COMMAND_ID_AT_NDHCP_ON: wifi_command_send_at_ndhcp(); break;
		case WIFI_COMMAND_ID_AT_WWPA: wifi_command_send_at_wwpa(); break;
		case WIFI_COMMAND_ID_AT_WAUTO: wifi_command_send_at_auto(); break;
		case WIFI_COMMAND_ID_AT_NAUTO: wifi_command_send_at_wauto(); break;
		case WIFI_COMMAND_ID_AT_NSTCP: wifi_command_send_at_nstcp(); break;
		case WIFI_COMMAND_ID_AT_WA: wifi_command_send_at_wa(); break;
		case WIFI_COMMAND_ID_AT_NSET: wifi_command_send_at_nset(); break;
		case WIFI_COMMAND_ID_AT_WWEP: wifi_command_send_at_wwep(); break;
		case WIFI_COMMAND_ID_AT_WEAPCONF: wifi_command_send_at_weapconf(); break;
		case WIFI_COMMAND_ID_AT_WEAP: wifi_command_send_at_weap(); break;
		case WIFI_COMMAND_ID_AT_ATS: wifi_command_send_at_ats(); break;
		case WIFI_COMMAND_ID_AT_WREGDOMAIN: wifi_command_send_at_wregdomain(); break;
		default: break;
	}

#if LOGGING_LEVEL == LOGGING_DEBUG
	logwohd("\n\r");
#endif

	wifi_data_send("\r\n", 2);
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
	wifi_data_send(str, 2);

	uint16_t i;
	str[32] = '\0';
	for(i = 0; i < length; i+=32) {
		wifi_read_config(str, 32, offset + i);
		if(i + 32 < length) {
			wifi_data_send(str, 32);
		} else {
			wifi_data_send(str, length % 32);
		}
	}

	eap_type = 0xFF;
}

uint8_t wifi_command_parse_simple(const char *data, const uint8_t length) {
	if(length == 0) {
		return WIFI_ANSWER_NO_ANSWER;
	}

	switch(data[0]) {
		case '0': {
			return WIFI_ANSWER_OK;
		}

		case 'f':
		case '3':
		case '4':
		case '5':
		case '6':
		case '1': {
			return WIFI_ANSWER_ERROR;
		}

		case '2': {
			return WIFI_ANSWER_INVALID_COMMAND;
		}

		case '7': {
			return WIFI_ANSWER_CONNECT;
		}

		case '8': {
			logwifii("8: Disconnect: %c\n\r", data[2]);
			wifi_data_disconnect(wifi_data_hex_to_int(data[2]));
			return WIFI_ANSWER_DISCONNECT;
		}

		case '9': {
			logwifii("9: Disassociated\n\r");
			wifi_status.state = WIFI_STATE_DISASSOCIATED;
			return WIFI_ANSWER_DISASSOCIATED;
		}

		default: {
			if(length == 1) {
				return WIFI_ANSWER_NO_ANSWER;
			}

			if((data = strcasestr(data, "OK")) != NULL) {
				return WIFI_ANSWER_OK;
			}

			if((data = strcasestr(data, "Error")) != NULL) {
				return WIFI_ANSWER_ERROR;
			}

			return WIFI_ANSWER_NO_ANSWER;
		}
	}
}

char* wifi_command_parse_ip(const char *data, const char *search_str, uint8_t *result) {
	char *ptr;
	if((ptr = strcasestr(data, search_str)) != NULL) {
		ptr += strlen(search_str);
		char *ptr_tmp = ptr;
		for(int8_t j = 3; j >= 0; j--) {
			for(uint8_t i = 0; i < 4; i++) {
				ptr_tmp++;
				if(!(*ptr_tmp >= '0' && *ptr_tmp <= '9')) {
					result[j] = atoi(ptr);
					ptr = ptr_tmp+1;
					break;
				}
			}
		}

		return ptr;
	}

	return NULL;
}

char* wifi_command_parse_mac(const char *data, const char *search_str, uint8_t *result) {
	char *ptr;
	if((ptr = strcasestr(data, search_str)) != NULL) {
		ptr += strlen(search_str);
		for(int8_t i = 5; i >= 0; i--) {
			result[i] = wifi_data_hex_to_int(*ptr) << 4;
			ptr++;
			result[i] |= wifi_data_hex_to_int(*ptr);
			ptr+=2;
		}

		return ptr;
	}

	return NULL;
}

void wifi_command_parse(const char *data, const uint8_t length) {
	if(length == 0) {
		return;
	}

	logwifid("Command response (command %d): %s\n\r", wifi_command_parse_next, data);

	switch(wifi_command_parse_next) {
		case WIFI_COMMAND_ID_AT_WM_AP:
		case WIFI_COMMAND_ID_AT_WM_ADHOC:
		case WIFI_COMMAND_ID_AT_WM_IFACE:
		case WIFI_COMMAND_ID_AT_NSET:
		case WIFI_COMMAND_ID_AT_NDHCP_ON:
		case WIFI_COMMAND_ID_AT_WWPA:
		case WIFI_COMMAND_ID_AT_WAUTH_WEP:
		case WIFI_COMMAND_ID_AT_WWEP:
		case WIFI_COMMAND_ID_AT_WEAPCONF:
		case WIFI_COMMAND_ID_AT_WEAP:
		case WIFI_COMMAND_ID_AT_WSEC:
		case WIFI_COMMAND_ID_AT_BDATA_ON:
		case WIFI_COMMAND_ID_AT_WREGDOMAIN:
		case WIFI_COMMAND_ID_AT_ASYNCMSGFMT:
		case WIFI_COMMAND_ID_AT_WRETRY:
		case WIFI_COMMAND_ID_AT_PSPOLLINTRL:
		case WIFI_COMMAND_ID_AT_ATS:
		case WIFI_COMMAND_ID_AT_WSYNCINTRL:
		case WIFI_COMMAND_ID_AT_WRXACTIVE_ON:
		case WIFI_COMMAND_ID_AT_WRXACTIVE_OFF:
		case WIFI_COMMAND_ID_AT_WRXPS_ON:
		case WIFI_COMMAND_ID_AT_WRXPS_OFF:
		case WIFI_COMMAND_ID_AT_WD:
		case WIFI_COMMAND_ID_AT_ATV0: wifi_command_parse_other(data, length); break;
		case WIFI_COMMAND_ID_AT_ATE0: wifi_command_parse_ate0(data, length); break;
		case WIFI_COMMAND_ID_AT_WA: wifi_command_parse_wa(data, length); break;
		case WIFI_COMMAND_ID_AT_NSTCP: wifi_command_parse_nstcp(data, length); break;
		case WIFI_COMMAND_ID_AT_NSTAT: wifi_command_parse_nstat(data, length); break;
		case WIFI_COMMAND_ID_RESET: wifi_command_parse_reset(data, length); break;
		case WIFI_COMMAND_ID_END:
		case WIFI_COMMAND_ID_NONE:
		default: logwifiw("Parse default: %s\n\r", data); break;
	}
}

void wifi_command_parse_reset(const char *data, const uint8_t length) {
	if((strcasestr(data, "Serial2Wifi") != NULL) || // Normal reset
	   (strcasestr(data, "UnExpected") != NULL)) {  // "Unexpected Warm Boot"
		wifi_command_parse_next = WIFI_COMMAND_ID_NONE;
	} else {
		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
	}
}

void wifi_command_parse_other(const char *data, const uint8_t length) {
	if(wifi_command_parse_simple(data, length) == WIFI_ANSWER_OK) {
		wifi_command_parse_next = WIFI_COMMAND_ID_NONE;
	} else {
		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
	}
}

void wifi_command_parse_ate0(const char *data, const uint8_t length) {
	if(strcasestr(data, "ATE0") != NULL) {
		return;
	}

	if(wifi_command_parse_simple(data, length) == WIFI_ANSWER_OK) {
		wifi_command_parse_next = WIFI_COMMAND_ID_NONE;
	} else {
		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
	}
}

void wifi_command_parse_wa(const char *data, const uint8_t length) {
	if(strcasestr(data+4, "IP") != NULL) {
		return;
	}

	if(data[0] == ' ') {
		char *ptr = wifi_command_parse_ip(data, " ", wifi_status.ip);
		ptr = wifi_command_parse_ip(ptr, " ", wifi_status.subnet_mask);
		ptr = wifi_command_parse_ip(ptr, " ", wifi_status.gateway);
	} else {
		if(wifi_command_parse_simple(data, length) == WIFI_ANSWER_OK) {
			wifi_command_parse_next = WIFI_COMMAND_ID_NONE;
		} else {
			wifi_status.state = WIFI_STATE_STARTUP_ERROR;
		}
	}
}

void wifi_command_parse_nstcp(const char *data, const uint8_t length) {
	const uint8_t wifi_answer = wifi_command_parse_simple(data, length);
	if(wifi_answer == WIFI_ANSWER_OK) {
		wifi_command_parse_next = WIFI_COMMAND_ID_NONE;
	}
	// TODO: If wifi_answer == WIFI_ANSWER_CONNECT => cid = data[3:4]
}

void wifi_command_parse_nstat(const char *data, const uint8_t length) {
	char *ptr;

	if(wifi_command_parse_mac(data, "MAC=", wifi_status.mac_address)) {
		logwifii("mac: %x:%x:%x:%x:%x:%x\n\r", wifi_status.mac_address[5], wifi_status.mac_address[4], wifi_status.mac_address[3], wifi_status.mac_address[2], wifi_status.mac_address[1], wifi_status.mac_address[0]);
	}

	if(wifi_command_parse_mac(data, "BSSID=", wifi_status.bssid)) {
		logwifii("bssid: %x:%x:%x:%x:%x:%x\n\r", wifi_status.bssid[5], wifi_status.bssid[4], wifi_status.bssid[3], wifi_status.bssid[2], wifi_status.bssid[1], wifi_status.bssid[0]);
	}

	if((ptr = strcasestr(data, "CHANNEL=")) != NULL) {
		ptr += strlen("CHANNEL=");
		wifi_status.channel = atoi(ptr);

		logwifii("channel: %d\n\r", wifi_status.channel);
	}

	if((ptr = strcasestr(data, "RSSI=")) != NULL) {
		ptr += strlen("RSSI=");
		wifi_status.rssi = atoi(ptr);

		logwifii("rssi: %d\n\r", wifi_status.rssi);
	}

	if(wifi_command_parse_ip(data, "IP addr=", wifi_status.ip)) {
		logwifii("ip: %d.%d.%d.%d\n\r", wifi_status.ip[3], wifi_status.ip[2], wifi_status.ip[1], wifi_status.ip[0]);
	}

	if(wifi_command_parse_ip(data, "SubNet=", wifi_status.subnet_mask)) {
		logwifii("Subnet mask: %d.%d.%d.%d\n\r", wifi_status.subnet_mask[3], wifi_status.subnet_mask[2], wifi_status.subnet_mask[1], wifi_status.subnet_mask[0]);
	}

	if(wifi_command_parse_ip(data, "Gateway=", wifi_status.gateway)) {
		logwifii("Gateway: %d.%d.%d.%d\n\r", wifi_status.gateway[3], wifi_status.gateway[2], wifi_status.gateway[1], wifi_status.gateway[0]);
	}

	if((ptr = strcasestr(data, "Rx Count=")) != NULL) {
		ptr += strlen("Rx Count=");
		wifi_status.rx_count = atoi(ptr);

		logwifii("rx_count: %lu\n\r", wifi_status.rx_count);
	}

	if((ptr = strcasestr(data, "Tx Count=")) != NULL) {
		ptr += strlen("Tx Count=");
		wifi_status.tx_count = atoi(ptr);

		logwifii("tx_count: %lu\n\r", wifi_status.tx_count);
	}

	const uint8_t wifi_answer = wifi_command_parse_simple(data, length);
	if(wifi_answer == WIFI_ANSWER_OK) {
		wifi_command_parse_next = WIFI_COMMAND_ID_NONE;
	}
}

void wifi_command_flush(void) {
	for(uint8_t i = 0; i < 255; i++) {
		wifi_low_level_write_byte(WIFI_LOW_LEVEL_SPI_IDLE_CHAR);
	}
}
