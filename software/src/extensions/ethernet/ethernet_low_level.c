/* master-brick
 * Copyright (C) 2012-2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_low_level.c: Low level Ethernet protocol implementation
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

#include "ethernet_low_level.h"

#include "config.h"

#include "ethernet_config.h"
#include "ethernet_dhcp.h"
#include "ethernet_websocket.h"
#include "ethernet_w5200.h"
#include "ethernet_w5500.h"

#include "extensions/brickd.h"

#include "bricklib/drivers/pio/pio.h"
#include "bricklib/utility/util_definitions.h"

#include <string.h>

#define W5X00_VERSION_W5200 3
#define W5X00_VERSION_W5500 4

extern Pin extension_pins[];
extern uint8_t ETHERNET_CS;
extern uint8_t ETHERNET_RESET;

extern bool ethernet_dhcp_server;
extern EthernetStatus ethernet_status;
extern BrickdAuthenticationState brickd_authentication_state[];

uint8_t ethernet_plain_sockets = 4;
uint16_t ethernet_port = 4223;
uint16_t ethernet_websocket_port = 4280;

bool ethernet_low_level_socket_open[ETHERNET_MAX_SOCKETS] = {false, false, false, false, false, false, false};

const W5X00Register w5200_regs = {
	.ETH_REG_MODE = 0x000000,
	.ETH_REG_GATEWAY_ADDRESS = 0x000100,
	.ETH_REG_SUBNET_MASK = 0x000500,
	.ETH_REG_SOURCE_HW_ADDRESS = 0x000900,
	.ETH_REG_SOURCE_IP_ADDRESS = 0x000F00,
	.ETH_REG_RETRY_TIME = 0x001700,
	.ETH_REG_RETRY_COUNT = 0x001900,
	.ETH_REG_CHIP_VERSION = 0x001F00,
	.CH_BASE = 0x400000,
	.CH_OFFSET_SHIFT = 16
};

const W5X00Register w5500_regs = {
	.ETH_REG_MODE = 0x000000,
	.ETH_REG_GATEWAY_ADDRESS = 0x000100,
	.ETH_REG_SUBNET_MASK = 0x000500,
	.ETH_REG_SOURCE_HW_ADDRESS = 0x000900,
	.ETH_REG_SOURCE_IP_ADDRESS = 0x000F00,
	.ETH_REG_RETRY_TIME = 0x001900,
	.ETH_REG_RETRY_COUNT = 0x001B00,
	.ETH_REG_CHIP_VERSION = 0x003900,
	.CH_BASE = 0x000008,
	.CH_OFFSET_SHIFT = 5
};

uint8_t ethernet_chip_version = W5X00_VERSION_W5200;

// Set during initialization depending on usage of W5200 or W5500
W5X00Register *ethernet_regs;
uint8_t (*ethernet_read_buffer)(const uint32_t address, uint8_t *buffer, const uint16_t length);
uint8_t (*ethernet_write_buffer)(const uint32_t address, const uint8_t *buffer, const uint16_t length);

void ethernet_low_level_reset(void) {
	PIO_Clear(&extension_pins[ETHERNET_RESET]);
	SLEEP_US(10);
	PIO_Set(&extension_pins[ETHERNET_RESET]);
	SLEEP_MS(150);
}

void ethernet_low_level_get_default_hostname(char hostname[ETHERNET_HOSTNAME_LENGTH]) {
	const uint8_t tflen = 11;
	memcpy(hostname, "Tinkerforge", tflen);
	hostname[tflen + 0] = int_to_char(ethernet_status.mac_address[3]/16);
	hostname[tflen + 1] = int_to_char(ethernet_status.mac_address[3] % 16);
	hostname[tflen + 2] = int_to_char(ethernet_status.mac_address[4]/16);
	hostname[tflen + 3] = int_to_char(ethernet_status.mac_address[4] % 16);
	hostname[tflen + 4] = int_to_char(ethernet_status.mac_address[5]/16);
	hostname[tflen + 5] = int_to_char(ethernet_status.mac_address[5] % 16);
}

void ethernet_low_level_update_functions(void) {
	if(ethernet_chip_version == W5X00_VERSION_W5200) {
		ethernet_regs = (W5X00Register*)(&w5200_regs);
		ethernet_read_buffer = ethernet_w5200_read_buffer;
		ethernet_write_buffer = ethernet_w5200_write_buffer;
	} else {
		ethernet_regs = (W5X00Register*)(&w5500_regs);
		ethernet_read_buffer = ethernet_w5500_read_buffer;
		ethernet_write_buffer = ethernet_w5500_write_buffer;
	}
}

bool ethernet_low_level_discover_chip_version(void) {
	ethernet_low_level_reset();
	ethernet_low_level_reset();
	ethernet_chip_version = W5X00_VERSION_W5200;
	ethernet_low_level_update_functions();

	if(ethernet_read_register(ethernet_regs->ETH_REG_CHIP_VERSION) == W5X00_VERSION_W5200) {
		logethi("Found chip WIZNet W5200\n\r");
		return true;
	}

	ethernet_low_level_reset();
	ethernet_low_level_reset();
	ethernet_chip_version = W5X00_VERSION_W5500;
	ethernet_low_level_update_functions();

	if(ethernet_read_register(ethernet_regs->ETH_REG_CHIP_VERSION) == W5X00_VERSION_W5500) {
		logethi("Found chip WIZNet W5500\n\r");
		return true;
	}

	return false;
}

void ethernet_low_level_init(void) {
	logethi("Start low level init\n\r");

	EthernetConfiguration ec = ETHERNET_CONFIGURATION_DEFAULT;

	if(ethernet_read_config((char*)ethernet_status.mac_address, ETHERNET_MAC_SIZE, ETHERNET_MAC_POS, ETHERNET_KEY_POS)) {
		ethernet_low_level_get_default_hostname(ec.hostname);
		ethernet_read_config((char*)&ec, sizeof(ec), ETHERNET_CONFIGURATION_POS, ETHERNET_KEY_POS);
	} else {
		ethernet_low_level_get_default_hostname(ec.hostname);
		ethernet_write_config((char*)&ec, sizeof(ec), ETHERNET_CONFIGURATION_POS, ETHERNET_KEY_POS);
	}

	memcpy(ethernet_status.hostname, ec.hostname, ETHERNET_HOSTNAME_LENGTH);
	ethernet_port = ec.port;
	ethernet_websocket_port = ec.websocket_port;
	ethernet_plain_sockets = ETHERNET_MAX_SOCKETS - ec.websocket_sockets;

	if(ec.connection == ETHERNET_CONNECTION_STATIC) {
		ethernet_dhcp_server = false;
		memcpy(ethernet_status.gateway, ec.gateway, ETHERNET_IP_SIZE);
		memcpy(ethernet_status.subnet_mask, ec.subnet_mask, ETHERNET_IP_SIZE);
		memcpy(ethernet_status.ip, ec.ip, ETHERNET_IP_SIZE);
	}

	if(!ethernet_low_level_discover_chip_version()) {
		logethe("Could not identify version of WIZNet chip\n\r");
		// TODO: Stop Ethernet loop
		return;
	}

	// Configure socket memory
	for(uint8_t i = 0; i < ETHERNET_NUM_SOCKETS; i++) {
		ethernet_write_register(ETH_REG_SN_TXMEM_SIZE(i), ETHERNET_TX_SIZE);
		ethernet_write_register(ETH_REG_SN_RXMEM_SIZE(i), ETHERNET_RX_SIZE);
	}

	ethernet_write_buffer(ethernet_regs->ETH_REG_SOURCE_HW_ADDRESS, ethernet_status.mac_address, ETHERNET_MAC_SIZE);

	// Configure network
	if(ethernet_dhcp_server) {
		dhcp_init_socket(DHCP_SOCKET);
		dhcp_get_ip();
	} else {
		ethernet_write_buffer(ethernet_regs->ETH_REG_GATEWAY_ADDRESS, ethernet_status.gateway, ETHERNET_IP_SIZE);
		ethernet_write_buffer(ethernet_regs->ETH_REG_SUBNET_MASK, ethernet_status.subnet_mask, ETHERNET_IP_SIZE);
		ethernet_write_buffer(ethernet_regs->ETH_REG_SOURCE_IP_ADDRESS, ethernet_status.ip, ETHERNET_IP_SIZE);
	}

	for(uint8_t socket = 0; socket < ETHERNET_MAX_SOCKETS; socket++) {
		ethernet_low_level_socket_init(socket);
		ethernet_low_level_socket_listen(socket);
	}
}

// Check if the configuration is still set correctly. According to WIZnet the
// W5500 can lose its configuration in case of exposure to ESD...
// One thing that we can easily check that should never change is the MAC address.
void ethernet_low_level_check_config(void) {
	uint8_t mac_address[6];
	ethernet_read_buffer(ethernet_regs->ETH_REG_SOURCE_HW_ADDRESS, mac_address, ETHERNET_MAC_SIZE);

	if((mac_address[0] != ethernet_status.mac_address[0]) ||
	   (mac_address[1] != ethernet_status.mac_address[1]) ||
	   (mac_address[2] != ethernet_status.mac_address[2]) ||
	   (mac_address[3] != ethernet_status.mac_address[3]) ||
	   (mac_address[4] != ethernet_status.mac_address[4]) ||
	   (mac_address[5] != ethernet_status.mac_address[5])) {
		logethw("W5X00 has lost its configuration, reinitializing...\n\r");
		ethernet_low_level_init();
	}
}

// Should be called if we have an irreversible protocol failure
void ethernet_low_level_emergency_disconnect(const uint8_t socket) {
	logethe("Emergency disconnect on socket %d\n\r", socket);
	ethernet_write_register(ETH_REG_SN_CR(socket), ETH_VAL_SN_CR_DISCON);

	// Read buffer until empty
	uint8_t tmp_buffer[10];
	while(ethernet_low_level_read_data_tcp(socket, tmp_buffer, 10) == 10);
}

void ethernet_low_level_disconnect(const uint8_t socket) {
	ethernet_write_register(ETH_REG_SN_CR(socket), ETH_VAL_SN_CR_DISCON);
}

uint8_t ethernet_low_level_get_status(const uint8_t socket) {
	return ethernet_read_register(ETH_REG_SN_SR(socket));
}

void ethernet_low_level_socket_init(const uint8_t socket) {
	logethd("Socket %d: start initialization\n\r", socket);
	if(ethernet_low_level_get_status(socket) != ETH_VAL_SN_SR_SOCK_CLOSED) {
		return;
	}

	if(brickd_authentication_state[socket] != BRICKD_AUTHENTICATION_STATE_DISABLED) {
		brickd_authentication_state[socket] = BRICKD_AUTHENTICATION_STATE_ENABLED;
	}

	uint16_t port = ethernet_port;
	if(socket >= ethernet_plain_sockets) {
		ethernet_websocket_init(socket);
		port = ethernet_websocket_port;
	}

	ethernet_write_register(ETH_REG_SN_MR(socket), ETH_VAL_SN_MR_PROTO_TCP | ETH_VAL_SN_MR_ND_MC);
	ethernet_write_register(ETH_REG_SN_PORT(socket), (port & 0xFF00) >> 8);
	ethernet_write_register(ETH_REG_SN_PORT(socket) + (1 << 8), port & 0x00FF);
	ethernet_write_register(ETH_REG_SN_CR(socket), ETH_VAL_SN_CR_OPEN);

	ethernet_low_level_set_retry_time(6000);
	ethernet_low_level_set_retry_count(3);

	while(ethernet_low_level_get_status(socket) != ETH_VAL_SN_SR_SOCK_INIT);
	logethd("Socket %d: initialized\n\r", socket);
}

void ethernet_low_level_socket_listen(const uint8_t socket) {
	logethd("Socket %d: start listen\n\r", socket);
	if(ethernet_low_level_get_status(socket) != ETH_VAL_SN_SR_SOCK_INIT) {
		return;
	}

	ethernet_write_register(ETH_REG_SN_CR(socket), ETH_VAL_SN_CR_LISTEN);

	uint8_t status = 0;
	// There is a small race condition here, a new socket might already be
	// established before we can read back the SOCK_LISTEN status.
	// So we accept the SOCK_ESTABLISHED status too.
	while(status != ETH_VAL_SN_SR_SOCK_LISTEN && status != ETH_VAL_SN_SR_SOCK_ESTABLISHED) {
		status = ethernet_low_level_get_status(socket);
	}
	logethd("Socket %d: listening\n\r", socket);
}

uint16_t ethernet_low_level_get_received_data_length(const uint8_t socket) {
	uint16_t value = 0;

	value = ethernet_read_register(ETH_REG_SN_RX_RSR(socket)) << 8;
	value |= ethernet_read_register(ETH_REG_SN_RX_RSR(socket) + (1 << 8));

	return value;
}

uint16_t ethernet_low_level_get_free_data_length(const uint8_t socket) {
	uint16_t value = 0;

	value = ethernet_read_register(ETH_REG_SN_TX_FSR(socket)) << 8;
	value |= ethernet_read_register(ETH_REG_SN_TX_FSR(socket) + (1 << 8));

	return value;
}

uint16_t ethernet_low_level_get_receive_pointer(const uint8_t socket) {
	uint16_t value = 0;

	value = ethernet_read_register(ETH_REG_SN_RX_RD(socket)) << 8;
	value |= ethernet_read_register(ETH_REG_SN_RX_RD(socket) + (1 << 8));

	return value;
}


void ethernet_low_level_set_receive_pointer(const uint8_t socket, const uint16_t pointer) {
	ethernet_write_register(ETH_REG_SN_RX_RD(socket), (pointer & 0xFF00) >> 8);
	ethernet_write_register(ETH_REG_SN_RX_RD(socket) + (1 << 8), pointer & 0x00FF);
}

uint16_t ethernet_low_level_get_transmit_pointer(const uint8_t socket) {
	uint16_t value = 0;

	value = ethernet_read_register(ETH_REG_SN_TX_WR(socket)) << 8;
	value |= ethernet_read_register(ETH_REG_SN_TX_WR(socket) + (1 << 8));

	return value;
}

void ethernet_low_level_set_transmit_pointer(const uint8_t socket, const uint16_t pointer) {
	ethernet_write_register(ETH_REG_SN_TX_WR(socket), (pointer & 0xFF00) >> 8);
	ethernet_write_register(ETH_REG_SN_TX_WR(socket) + (1 << 8), pointer & 0x00FF);
}


void ethernet_low_level_set_retry_time(const uint16_t retry_time) {
	ethernet_write_register(ethernet_regs->ETH_REG_RETRY_TIME, (retry_time & 0xFF00) >> 8);
	ethernet_write_register(ethernet_regs->ETH_REG_RETRY_TIME + (1 << 8), retry_time & 0x00FF);
}

void ethernet_low_level_set_retry_count(const uint8_t retry_count) {
	ethernet_write_register(ethernet_regs->ETH_REG_RETRY_COUNT, retry_count);
}

uint8_t ethernet_low_level_read_data_tcp(const uint8_t socket, uint8_t *buffer, const uint8_t length) {
	uint8_t status = ethernet_low_level_get_status(socket);
	if(status != ETH_VAL_SN_SR_SOCK_ESTABLISHED) {
		if(!ethernet_low_level_socket_open[socket]) {
			return 0;
		}
	}

	ethernet_low_level_socket_open[socket] = true;

	const uint16_t recv_length = ethernet_low_level_get_received_data_length(socket);
	uint16_t read_length = MIN(length, recv_length);
	if(read_length == 0) {
		return 0;
	}

	uint16_t offset_address = ethernet_low_level_get_receive_pointer(socket);

	if(ethernet_chip_version == W5X00_VERSION_W5200) {
		uint16_t addr = ETHERNET_RX_BASE + socket*(ETHERNET_RX_MASK + 1) + (offset_address & ETHERNET_RX_MASK);

		if(addr + read_length > ETHERNET_RX_BASE + (socket + 1)*(ETHERNET_RX_MASK + 1)) {
			uint16_t upper_length = ETHERNET_RX_BASE + (socket + 1)*(ETHERNET_RX_MASK + 1) - addr;
			ethernet_read_buffer(addr << 8, buffer, upper_length);
			ethernet_read_buffer((ETHERNET_RX_BASE + socket*(ETHERNET_RX_MASK + 1)) << 8,
								 buffer + upper_length,
								 read_length - upper_length);
		} else {
			ethernet_read_buffer(addr << 8, buffer, read_length);
		}
	} else {
		uint16_t addr = offset_address & ETHERNET_RX_MASK;

		if(addr + read_length > (ETHERNET_RX_MASK+1)) {
			uint16_t upper_length = (ETHERNET_RX_MASK + 1) - addr;
			ethernet_read_buffer((addr << 8) | W5500_BSB_RX(socket), buffer, upper_length);
			ethernet_read_buffer((0 << 8) | W5500_BSB_RX(socket),
								 buffer + upper_length,
								 read_length - upper_length);
		} else {
			ethernet_read_buffer((addr << 8) | W5500_BSB_RX(socket), buffer, read_length);
		}
	}

	offset_address += read_length;
	ethernet_low_level_set_receive_pointer(socket, offset_address);

	ethernet_write_register(ETH_REG_SN_CR(socket), ETH_VAL_SN_CR_RECV);
	while(ethernet_read_register(ETH_REG_SN_CR(socket)) != 0);

	if(read_length == 0) {
		if(status != ETH_VAL_SN_SR_SOCK_ESTABLISHED) {
			ethernet_low_level_socket_open[socket] = false;
		}
	}

	ethernet_status.rx_count += read_length;
	return read_length;
}

uint8_t ethernet_low_level_write_data_tcp_blocking(const uint8_t socket, const uint8_t *buffer, const uint8_t length) {
	uint8_t status = ethernet_low_level_get_status(socket);
	if(status != ETH_VAL_SN_SR_SOCK_ESTABLISHED) {
		// Return 0 if socket is not established
		return 0;
	}

	uint8_t length_to_send = length;

	while(length_to_send > 0) {
		length_to_send -= ethernet_low_level_write_data_tcp(socket, buffer, length_to_send);
	}

	return length;
}

uint8_t ethernet_low_level_write_data_tcp(const uint8_t socket, const uint8_t *buffer, const uint8_t length) {
	uint8_t status = ethernet_low_level_get_status(socket);
	if(status != ETH_VAL_SN_SR_SOCK_ESTABLISHED) {
		return 0;
	}

	const uint16_t size_free = ethernet_low_level_get_free_data_length(socket);
	uint8_t size_max = MIN(size_free, length);
	if(size_max == 0) {
		return 0;
	}

	uint16_t offset_address = ethernet_low_level_get_transmit_pointer(socket);

	if(ethernet_chip_version == W5X00_VERSION_W5200) {
		uint16_t addr = ETHERNET_TX_BASE + socket*(ETHERNET_TX_MASK + 1) + (offset_address & ETHERNET_TX_MASK);

		if(addr + size_max > ETHERNET_TX_BASE + (socket + 1)*(ETHERNET_TX_MASK + 1)) {
			uint16_t upper_length = ETHERNET_TX_BASE + (socket + 1)*(ETHERNET_TX_MASK + 1) - addr;
			ethernet_write_buffer(addr << 8, buffer, upper_length);
			ethernet_write_buffer((ETHERNET_TX_BASE + socket*(ETHERNET_TX_MASK + 1)) << 8,
								  buffer + upper_length,
								  size_max - upper_length);
		} else {
			ethernet_write_buffer(addr << 8, buffer, size_max);
		}
	} else {
		uint16_t addr = offset_address & ETHERNET_TX_MASK;

		if(addr + size_max > (ETHERNET_TX_MASK + 1)) {
			uint16_t upper_length = (ETHERNET_TX_MASK + 1) - addr;
			ethernet_write_buffer((addr << 8) | W5500_BSB_TX(socket), buffer, upper_length);
			ethernet_write_buffer((0 << 8) | W5500_BSB_TX(socket),
								  buffer + upper_length,
								  size_max - upper_length);
		} else {
			ethernet_write_buffer((addr << 8) | W5500_BSB_TX(socket), buffer, size_max);
		}
	}

	ethernet_low_level_set_transmit_pointer(socket, offset_address + size_max);

	ethernet_write_register(ETH_REG_SN_CR(socket), ETH_VAL_SN_CR_SEND);
	while(ethernet_read_register(ETH_REG_SN_CR(socket)) != 0);

	ethernet_status.tx_count += size_max;
	return size_max;
}

uint16_t ethernet_low_level_read_data_udp(const uint8_t socket, uint8_t *buffer, const uint16_t length, uint8_t ip[4], uint16_t *port) {
	if(ethernet_low_level_get_status(socket) != ETH_VAL_SN_SR_SOCK_UDP) {
		return 0;
	}

	const uint16_t recv_length = ethernet_low_level_get_received_data_length(socket);
	uint16_t read_length = MIN(length, recv_length);
	if(read_length < ETHERNET_UDP_HEADER_SIZE) {
		return 0;
	}

	uint16_t offset_address;
	uint8_t udp_head[ETHERNET_UDP_HEADER_SIZE];
	uint16_t addr;

	if(ethernet_chip_version == W5X00_VERSION_W5200) {
		offset_address = ethernet_low_level_get_receive_pointer(socket);
		addr = ETHERNET_RX_BASE + socket*(ETHERNET_RX_MASK + 1) + (offset_address & ETHERNET_RX_MASK);

		if(addr + ETHERNET_UDP_HEADER_SIZE > ETHERNET_RX_BASE + (socket + 1)*(ETHERNET_RX_MASK + 1)) {
			uint16_t upper_length = ETHERNET_RX_BASE + (socket + 1)*(ETHERNET_RX_MASK + 1) - addr;
			ethernet_read_buffer(addr << 8, udp_head, upper_length);
			ethernet_read_buffer((ETHERNET_RX_BASE + socket*(ETHERNET_RX_MASK + 1)) << 8,
								 udp_head + upper_length,
								 ETHERNET_UDP_HEADER_SIZE - upper_length);
		} else {
			ethernet_read_buffer(addr << 8, udp_head, ETHERNET_UDP_HEADER_SIZE);
		}
	} else {
		offset_address = ethernet_low_level_get_receive_pointer(socket);
		addr = offset_address & ETHERNET_RX_MASK;

		if(addr + ETHERNET_UDP_HEADER_SIZE > (ETHERNET_RX_MASK + 1)) {
			uint16_t upper_length = (ETHERNET_RX_MASK + 1) - addr;
			ethernet_read_buffer((addr << 8) | W5500_BSB_RX(socket), udp_head, upper_length);
			ethernet_read_buffer((0 << 8) | W5500_BSB_RX(socket),
								 udp_head + upper_length,
								 ETHERNET_UDP_HEADER_SIZE - upper_length);
		} else {
			ethernet_read_buffer((addr << 8) | W5500_BSB_RX(socket), udp_head, ETHERNET_UDP_HEADER_SIZE);
		}
	}

	read_length -= ETHERNET_UDP_HEADER_SIZE;
	offset_address += ETHERNET_UDP_HEADER_SIZE;

	ip[0] = udp_head[0];
	ip[1] = udp_head[1];
	ip[2] = udp_head[2];
	ip[3] = udp_head[3];
	*port = udp_head[4] << 8;
	*port |= udp_head[5];

	uint16_t udp_length = udp_head[6] << 8;
	udp_length |= udp_head[7];

	if(read_length < udp_length) {
		return 0;
	}

	if(ethernet_chip_version == W5X00_VERSION_W5200) {
		addr = ETHERNET_RX_BASE + socket*(ETHERNET_RX_MASK + 1) + (offset_address & ETHERNET_RX_MASK);

		if(addr + udp_length > ETHERNET_RX_BASE + (socket + 1)*(ETHERNET_RX_MASK + 1)) {
			uint16_t upper_length = ETHERNET_RX_BASE + (socket + 1)*(ETHERNET_RX_MASK + 1) - addr;
			ethernet_read_buffer(addr << 8, buffer, upper_length);
			ethernet_read_buffer((ETHERNET_RX_BASE + socket*(ETHERNET_RX_MASK + 1)) << 8,
								 buffer + upper_length,
								 udp_length - upper_length);
		} else {
			ethernet_read_buffer(addr << 8, buffer, udp_length);
		}
	} else {
		addr = offset_address & ETHERNET_RX_MASK;

		if(addr + udp_length > (ETHERNET_RX_MASK + 1)) {
			uint16_t upper_length = (ETHERNET_RX_MASK + 1) - addr;
			ethernet_read_buffer((addr << 8) | W5500_BSB_RX(socket), buffer, upper_length);
			ethernet_read_buffer((0 << 8) | W5500_BSB_RX(socket),
								 buffer + upper_length,
								 udp_length - upper_length);
		} else {
			ethernet_read_buffer((addr << 8) | W5500_BSB_RX(socket), buffer, udp_length);
		}
	}

	offset_address += udp_length;

	ethernet_low_level_set_receive_pointer(socket, offset_address);

	ethernet_write_register(ETH_REG_SN_CR(socket), ETH_VAL_SN_CR_RECV);
	while(ethernet_read_register(ETH_REG_SN_CR(socket)) != 0);

	return udp_length;
}

uint16_t ethernet_low_level_write_data_udp(const uint8_t socket, const uint8_t *buffer, const uint16_t length, const uint8_t ip[4], const uint16_t port) {
	uint8_t status = ethernet_low_level_get_status(socket);
	if(status != ETH_VAL_SN_SR_SOCK_UDP) {
		return 0;
	}

	const uint16_t size_free = ethernet_low_level_get_free_data_length(socket);
	uint16_t size_max = MIN(size_free, length);
	if(size_max == 0) {
		return 0;
	}

	ethernet_write_register(ETH_REG_SN_DIPR(socket), ip[0]);
	ethernet_write_register(ETH_REG_SN_DIPR(socket) + (1 << 8), ip[1]);
	ethernet_write_register(ETH_REG_SN_DIPR(socket) + (2 << 8), ip[2]);
	ethernet_write_register(ETH_REG_SN_DIPR(socket) + (3 << 8), ip[3]);

	ethernet_write_register(ETH_REG_SN_DPORT(socket), (port & 0xFF00) >> 8);
	ethernet_write_register(ETH_REG_SN_DPORT(socket) + (1 << 8), port & 0x00FF);

	uint16_t offset_address = ethernet_low_level_get_transmit_pointer(socket);

	if(ethernet_chip_version == W5X00_VERSION_W5200) {
		uint16_t addr = ETHERNET_TX_BASE + socket*(ETHERNET_TX_MASK + 1) + (offset_address & ETHERNET_TX_MASK);

		if(addr + size_max > ETHERNET_TX_BASE + (socket + 1)*(ETHERNET_TX_MASK + 1)) {
			uint16_t upper_length = ETHERNET_TX_BASE + (socket + 1)*(ETHERNET_TX_MASK + 1) - addr;
			ethernet_write_buffer(addr << 8, buffer, upper_length);
			ethernet_write_buffer((ETHERNET_TX_BASE + socket*(ETHERNET_TX_MASK + 1)) << 8,
								  buffer + upper_length,
								  size_max - upper_length);
		} else {
			ethernet_write_buffer(addr << 8, buffer, size_max);
		}
	} else {
		uint16_t addr = offset_address & ETHERNET_TX_MASK;

		if(addr + size_max > (ETHERNET_TX_MASK + 1)) {
			uint16_t upper_length = (ETHERNET_TX_MASK + 1) - addr;
			ethernet_write_buffer((addr << 8) | W5500_BSB_TX(socket), buffer, upper_length);
			ethernet_write_buffer((0 << 8) | W5500_BSB_TX(socket),
								  buffer + upper_length,
								  size_max - upper_length);
		} else {
			ethernet_write_buffer((addr << 8) | W5500_BSB_TX(socket), buffer, size_max);
		}
	}

	ethernet_low_level_set_transmit_pointer(socket, offset_address + size_max);

	ethernet_write_register(ETH_REG_SN_CR(socket), ETH_VAL_SN_CR_SEND);
	while(ethernet_read_register(ETH_REG_SN_CR(socket)) != 0);

	if(ethernet_read_register(ETH_REG_SN_IR(socket)) & ETH_VAL_SN_IR_TIMEOUT) {
		return 0;
	}

	return length;
}

void ethernet_select(void) {
	__disable_irq();
	PIO_Clear(&extension_pins[ETHERNET_CS]);
}

void ethernet_deselect(void) {
	PIO_Set(&extension_pins[ETHERNET_CS]);
	__enable_irq();
}

uint8_t ethernet_transceive_byte(const uint8_t value) {
	// Wait for transfer buffer to be empty
	while((USART0->US_CSR & US_CSR_TXEMPTY) == 0);
	USART0->US_THR = value;

	// Wait until receive buffer has new data
	while((USART0->US_CSR & US_CSR_RXRDY) == 0);
	return USART0->US_RHR;
}

uint8_t ethernet_read_register(const uint32_t address) {
	uint8_t value;
	ethernet_read_buffer(address, &value, 1);

	return value;
}

void ethernet_write_register(const uint32_t address, const uint8_t value) {
	ethernet_write_buffer(address, &value, 1);
}
