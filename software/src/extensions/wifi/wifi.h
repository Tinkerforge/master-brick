/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi.h: High Level WIFI protocol implementation
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

#ifndef WIFI_H
#define WIFI_H

#include <stdint.h>
#include <stdbool.h>

#define CONNECTION_DHCP 0
#define CONNECTION_STATIC_IP 1

#define ENCRYPTION_WPA 0
#define ENCRYPTION_WEP 1
#define ENCRYPTION_NONE 2

typedef enum {
	WIFI_STATE_COMMAND_IDLE,
	WIFI_STATE_COMMAND_SEND,
	WIFI_STATE_COMMAND_RECV
} WIFIState;

typedef struct {
	char ssid[32];
	uint8_t connection;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint16_t port;
	uint8_t encryption;
	char key[50];
	uint8_t key_index;
} __attribute__((__packed__)) WifiConfiguration;

typedef struct {
	uint8_t mac_address[6];
	uint8_t bssid[6];
	uint8_t channel;
	int16_t rssi;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint32_t rx_count;
	uint32_t tx_count;
} __attribute__((__packed__)) WifiStatus;

bool wifi_init(void);
uint16_t wifi_send(const void *data, const uint16_t length);
uint16_t wifi_recv(void *data, const uint16_t length);
void wifi_refresh_status(void);
void wifi_message_loop(void *parameters);
void wifi_message_loop_return(char *data, uint16_t length);
void wifi_init_extension(uint8_t extension);
void wifi_read_config(char *data, uint8_t length, uint8_t position);
void wifi_write_config(char *data, uint8_t length, uint8_t position);

#endif
