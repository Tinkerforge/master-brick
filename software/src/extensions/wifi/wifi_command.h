/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
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
	WIFI_COMMAND_ID_AT_WWEP,
	WIFI_COMMAND_ID_AT_SPICONF,
	WIFI_COMMAND_ID_AT_WRSSI,
	WIFI_COMMAND_ID_AT_NSTAT,
	WIFI_COMMAND_ID_AT_END
} WIFICommand;

typedef enum {
	WIFI_ANSWER_OK,
	WIFI_ANSWER_NO_ANSWER,
	WIFI_ANSWER_CONNECT,
	WIFI_ANSWER_DISCONNECT,
	WIFI_ANSWER_ERROR
} WIFIAnswer;

#define WIFI_COMMAND_AT           "AT"
#define WIFI_COMMAND_AT_WD        "AT+WD"
#define WIFI_COMMAND_AT_ATA       "ATA"
#define WIFI_COMMAND_AT_ATO       "ATO"
#define WIFI_COMMAND_AT_ATC_ON    "ATC1"
#define WIFI_COMMAND_AT_ATC_OFF   "ATC0"
#define WIFI_COMMAND_AT_NDHCP_ON  "AT+NDHCP=1"
#define WIFI_COMMAND_AT_NDHCP_OFF "AT+NDHCP=0"
#define WIFI_COMMAND_AT_BDATA_ON  "AT+BDATA=1"
#define WIFI_COMMAND_AT_BDATA_OFF "AT+BDATA=0"
#define WIFI_COMMAND_AT_WRXACTIVE_ON  "AT+WRXACTIVE=1"
#define WIFI_COMMAND_AT_WRXACTIVE_OFF "AT+WRXACTIVE=0"
#define WIFI_COMMAND_AT_WRXPS_ON  "AT+WRXPS=1"
#define WIFI_COMMAND_AT_WRXPS_OFF "AT+WRXPS=0"
#define WIFI_COMMAND_AT_ATE0      "ATE0"
#define WIFI_COMMAND_AT_ATV0      "ATV0"
#define WIFI_COMMAND_AT_ATV1      "ATV1"
#define WIFI_COMMAND_AT_WWPA      "AT+WWPA="
#define WIFI_COMMAND_AT_WAUTO     "AT+WAUTO="
#define WIFI_COMMAND_AT_NAUTO     "AT+NAUTO="
#define WIFI_COMMAND_AT_NSTCP     "AT+NSTCP="
#define WIFI_COMMAND_AT_WA        "AT+WA="
#define WIFI_COMMAND_AT_NSET      "AT+NSET="
#define WIFI_COMMAND_AT_WAUTH_WEP "AT+WAUTH=2"
#define WIFI_COMMAND_AT_WWEP      "AT+WWEP"
#define WIFI_COMMAND_AT_SPICONF   "AT+SPICONF=0,1"
#define WIFI_COMMAND_AT_WRSSI     "AT+WRSSI=?"
#define WIFI_COMMAND_AT_NSTAT     "AT+NSTAT=?"

void wifi_command_send(const WIFICommand command);
uint8_t wifi_command_recv(char *data, const uint8_t length);
uint8_t wifi_command_parse(const char *data, const uint8_t length);
uint8_t wifi_command_recv_and_parse(void);
uint8_t wifi_command_send_recv_and_parse(const WIFICommand command);
void wifi_command_flush(void);

#endif
