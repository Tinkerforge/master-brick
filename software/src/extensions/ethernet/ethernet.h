/* master-brick
 * Copyright (C) 2012-2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet.h: High Level Ethernet protocol implementation
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

#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdint.h>
#include <stdbool.h>

#define ETHERNET_CONFIGURATION_DEFAULT {0, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 4223, "Tinkerforge", 3, 4280}
#define ETHERNET_STATUS_DEFAULT        {{0x40, 0xD8, 0x55, 0x02, 0xA0, 0x00}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, 0, "TBD"}

typedef struct {
	uint8_t connection;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint16_t port;
	char hostname[32];
	uint8_t websocket_sockets;
	uint16_t websocket_port;
} __attribute__((__packed__)) EthernetConfiguration;

typedef struct {
	uint8_t mac_address[6];
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint32_t rx_count;
	uint32_t tx_count;
	char hostname[32];
} __attribute__((__packed__)) EthernetStatus;

typedef uint8_t (*read_data_tcp_t)(const uint8_t, uint8_t*, const uint8_t);

void ethernet_init_extension(uint8_t extension);
bool ethernet_init(void);
uint16_t ethernet_send(const void *data, const uint16_t length, uint32_t *options);
uint16_t ethernet_recv(void *data, const uint16_t length, uint32_t *options);
void ethernet_message_loop(void *parameters);
void ethernet_message_loop_return(const char *data, const uint16_t length);
void ethernet_tick(const uint8_t tick_type);

uint32_t ethernet_read_key(uint16_t key_pos);
void ethernet_write_key(uint16_t key_pos);
bool ethernet_read_config(char *data, const uint8_t length, const uint16_t position, uint16_t key_pos);
void ethernet_write_config(const char *data, const uint8_t length, const uint16_t position, uint16_t key_pos);

#endif
