/* master-brick
 * Copyright (C) 2012-2013 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_command.h: Command mode functionality for WIFI Extension
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

#ifndef WIFI_COMMAND_H
#define WIFI_COMMAND_H

#include <stdint.h>
#include <stdbool.h>

#define WIFI_COMMAND_RECV_TIMEOUT 1000000

typedef enum {
	WIFI_COMMAND_ID_AT,
	WIFI_COMMAND_ID_AT_WD,
	WIFI_COMMAND_ID_AT_ATA,
	WIFI_COMMAND_ID_AT_ATO,
	WIFI_COMMAND_ID_AT_ATC_ON,
	WIFI_COMMAND_ID_AT_ATC_OFF,
	WIFI_COMMAND_ID_AT_NDHCP_ON,
	WIFI_COMMAND_ID_AT_NDHCP_OFF,
	WIFI_COMMAND_ID_AT_BDATA_ON,
	WIFI_COMMAND_ID_AT_BDATA_OFF,
	WIFI_COMMAND_ID_AT_WRXACTIVE_ON,
	WIFI_COMMAND_ID_AT_WRXACTIVE_OFF,
	WIFI_COMMAND_ID_AT_WRXPS_ON,
	WIFI_COMMAND_ID_AT_WRXPS_OFF,
	WIFI_COMMAND_ID_AT_ATE0,
	WIFI_COMMAND_ID_AT_ATV0,
	WIFI_COMMAND_ID_AT_ATV1,
	WIFI_COMMAND_ID_AT_WWPA,
	WIFI_COMMAND_ID_AT_WAUTO,
	WIFI_COMMAND_ID_AT_NAUTO,
	WIFI_COMMAND_ID_AT_NSTCP,
	WIFI_COMMAND_ID_AT_WA,
	WIFI_COMMAND_ID_AT_NSET,
	WIFI_COMMAND_ID_AT_WAUTH_WEP,
	WIFI_COMMAND_ID_AT_WAUTH_OPEN,
	WIFI_COMMAND_ID_AT_WWEP,
	WIFI_COMMAND_ID_AT_SPICONF,
	WIFI_COMMAND_ID_AT_WRSSI,
	WIFI_COMMAND_ID_AT_NSTAT,
	WIFI_COMMAND_ID_AT_VER,
	WIFI_COMMAND_ID_AT_PSPOLLINTRL,
	WIFI_COMMAND_ID_AT_WEAPCONF,
	WIFI_COMMAND_ID_AT_WEAP,
	WIFI_COMMAND_ID_AT_WSYNCINTRL,
	WIFI_COMMAND_ID_AT_ASYNCMSGFMT,
	WIFI_COMMAND_ID_AT_F,
	WIFI_COMMAND_ID_AT_SETSOCKOPT_SO,
	WIFI_COMMAND_ID_AT_SETSOCKOPT_TC,
	WIFI_COMMAND_ID_AT_ATS,
	WIFI_COMMAND_ID_AT_WM_IFACE,
	WIFI_COMMAND_ID_AT_WM_ADHOC,
	WIFI_COMMAND_ID_AT_WM_AP,
	WIFI_COMMAND_ID_AT_WREGDOMAIN,
	WIFI_COMMAND_ID_AT_WRETRY,
	WIFI_COMMAND_ID_AT_WSEC,
	WIFI_COMMAND_ID_AT_WIEEEPSPOLL,
	WIFI_COMMAND_ID_AT_NCLOSE,
	WIFI_COMMAND_ID_END,
	WIFI_COMMAND_ID_NONE,
	WIFI_COMMAND_ID_RESET
} WIFICommand;

typedef enum {
	WIFI_ANSWER_OK,
	WIFI_ANSWER_NO_ANSWER,
	WIFI_ANSWER_CONNECT,
	WIFI_ANSWER_DISCONNECT,
	WIFI_ANSWER_INVALID_COMMAND,
	WIFI_ANSWER_TIMEOUT,
	WIFI_ANSWER_DISASSOCIATED,
	WIFI_ANSWER_ERROR
} WIFIAnswer;

typedef struct {
	uint32_t key;
	char hostname[16];
} __attribute__((__packed__)) WIFIHostname;

#define WIFI_COMMAND_AT               "AT"
#define WIFI_COMMAND_AT_WD            "AT+WD"
#define WIFI_COMMAND_AT_ATA           "ATA"
#define WIFI_COMMAND_AT_ATO           "ATO"
#define WIFI_COMMAND_AT_ATC_ON        "ATC1"
#define WIFI_COMMAND_AT_ATC_OFF       "ATC0"
#define WIFI_COMMAND_AT_NDHCP_ON      "AT+NDHCP=1"
#define WIFI_COMMAND_AT_NDHCP_OFF     "AT+NDHCP=0"
#define WIFI_COMMAND_AT_BDATA_ON      "AT+BDATA=1"
#define WIFI_COMMAND_AT_BDATA_OFF     "AT+BDATA=0"
#define WIFI_COMMAND_AT_WRXACTIVE_ON  "AT+WRXACTIVE=1"
#define WIFI_COMMAND_AT_WRXACTIVE_OFF "AT+WRXACTIVE=0"
#define WIFI_COMMAND_AT_WRXPS_ON      "AT+WRXPS=1"
#define WIFI_COMMAND_AT_WRXPS_OFF     "AT+WRXPS=0"
#define WIFI_COMMAND_AT_ATE0          "ATE0"
#define WIFI_COMMAND_AT_ATV0          "ATV0"
#define WIFI_COMMAND_AT_ATV1          "ATV1"
#define WIFI_COMMAND_AT_WWPA          "AT+WWPA="
#define WIFI_COMMAND_AT_WAUTO         "AT+WAUTO="
#define WIFI_COMMAND_AT_NAUTO         "AT+NAUTO="
#define WIFI_COMMAND_AT_NSTCP         "AT+NSTCP="
#define WIFI_COMMAND_AT_WA            "AT+WA="
#define WIFI_COMMAND_AT_NSET          "AT+NSET="
#define WIFI_COMMAND_AT_WAUTH_WEP     "AT+WAUTH=2"
#define WIFI_COMMAND_AT_WAUTH_OPEN    "AT+WAUTH=1"
#define WIFI_COMMAND_AT_WWEP          "AT+WWEP"
#define WIFI_COMMAND_AT_SPICONF       "AT+SPICONF=0,1"
#define WIFI_COMMAND_AT_WRSSI         "AT+WRSSI=?"
#define WIFI_COMMAND_AT_NSTAT         "AT+NSTAT=?"
#define WIFI_COMMAND_AT_VER           "AT+VER=?"
#define WIFI_COMMAND_AT_PSPOLLINTRL   "AT+PSPOLLINTRL=0"
#define WIFI_COMMAND_AT_WEAPCONF      "AT+WEAPCONF="
#define WIFI_COMMAND_AT_WEAP          "AT+WEAP="
#define WIFI_COMMAND_AT_WSYNCINTRL    "AT+WSYNCINTRL=65535"
#define WIFI_COMMAND_AT_ASYNCMSGFMT   "AT+ASYNCMSGFMT=1"
#define WIFI_COMMAND_AT_F             "AT&F"
#define WIFI_COMMAND_AT_SETSOCKOPT_SO "AT+SETSOCKOPT="
#define WIFI_COMMAND_AT_SETSOCKOPT_TC "AT+SETSOCKOPT="
#define WIFI_COMMAND_AT_ATS           "ATS"
#define WIFI_COMMAND_AT_WM_IFACE      "AT+WM=0"
#define WIFI_COMMAND_AT_WM_ADHOC      "AT+WM=1"
#define WIFI_COMMAND_AT_WM_AP         "AT+WM=2"
#define WIFI_COMMAND_AT_WREGDOMAIN    "AT+WREGDOMAIN="
#define WIFI_COMMAND_AT_WRETRY        "AT+WRETRY=7"
#define WIFI_COMMAND_AT_WSEC          "AT+WSEC=1"
#define WIFI_COMMAND_AT_WIEEEPSPOLL   "AT+WIEEEPSPOLL=0"
#define WIFI_COMMAND_AT_NCLOSE        "AT+NCLOSE="

void wifi_command_send_at_ndhcp(void);
void wifi_command_send_at_wwpa(void);
void wifi_command_send_at_auto(void);
void wifi_command_send_at_wauto(void);
void wifi_command_send_at_nstcp(void);
void wifi_command_send_at_wa(void);
void wifi_command_send_at_nset(void);
void wifi_command_send_at_wwpa(void);
void wifi_command_send_at_weapconf(void);
void wifi_command_send_at_weap(void);
void wifi_command_send_at_ats(void);
void wifi_command_send_at_wregdomain(void);
void wifi_command_send_at_setsockopt(void);
void wifi_command_send_at_nclose(int8_t cid);

void wifi_command_parse(const char *data, const uint8_t length);
void wifi_command_parse_reset(const char *data, const uint8_t length);
void wifi_command_parse_other(const char *data, const uint8_t length);
void wifi_command_parse_ate0(const char *data, const uint8_t length);
void wifi_command_parse_wa(const char *data, const uint8_t length);
void wifi_command_parse_nstcp(const char *data, const uint8_t length);
void wifi_command_parse_nstat(const char *data, const uint8_t length);

char* wifi_command_parse_ip(const char *data, const char *search_str, uint8_t *result);
char* wifi_command_parse_mac(const char *data, const char *search_str, uint8_t *result);
void wifi_command_flush(void);

void wifi_command_send(const WIFICommand command, void *options);

#endif
