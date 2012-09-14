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
#define ENCRYPTION_WPA_ENTERPRISE 1
#define ENCRYPTION_WEP 2
#define ENCRYPTION_OPEN 3

#define WIFI_ERROR_NO_ERROR 0
#define WIFI_ERROR_BAD_RESPONSE 1
#define WIFI_ERROR_COULD_NOT_CONNECT 2

#define WIFI_POWER_MODE_FULL_SPEED 0
#define WIFI_POWER_MODE_LOW_POWER  1

typedef enum {
	WIFI_STATE_DISASSOCIATED = 0,
	WIFI_STATE_ASSOCIATED = 1,
	WIFI_STATE_ASSOCIATING = 2,
	WIFI_STATE_STARTUP_ERROR = 3,

	WIFI_STATE_NO_STARTUP = 255
} WifiState;

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
	uint8_t eap_options;
	uint16_t certificate_length;
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
	uint8_t state;
} __attribute__((__packed__)) WifiStatus;

bool wifi_init(void);
uint16_t wifi_send(const void *data, const uint16_t length);
uint16_t wifi_recv(void *data, const uint16_t length);
void wifi_establish_connection(void);
void wifi_message_loop(void *parameters);
void wifi_message_loop_return(char *data, uint16_t length);
void wifi_init_extension(uint8_t extension);
void wifi_read_config(char *data, const uint8_t length, const uint16_t position);
void wifi_write_config(const char *data, const uint8_t length, const uint16_t position);
void wifi_tick(uint8_t tick_type);
void wifi_refresh_status(void);
void wifi_set_power_mode(const uint8_t mode);

#endif
