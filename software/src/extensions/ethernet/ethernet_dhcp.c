/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_dhcp.c: DHCP implementation for Ethernet Extension
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

#include "ethernet_dhcp.h"

#include "ethernet_config.h"
#include "ethernet_low_level.h"

#include "bricklib/utility/init.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define HOST_NAME "EthernetExtension"

extern uint8_t ethernet_low_level_mac[];
extern uint8_t ethernet_low_level_ip[];
extern uint8_t ethernet_low_level_gw[];
extern uint8_t ethernet_low_level_dns[];
extern uint8_t ehternet_low_level_subnet_mask[];


static uint8_t dhcp_dns_server_ip[4] = {0, 0, 0, 0};
static uint8_t dhcp_real_dns_server_ip[4] = {0, 0, 0, 0};
static uint8_t dhcp_old_ip[4] = {0, 0, 0, 0};

static char dhcp_state;
static char retry_count;

static uint8_t DHCP_timeout;
static LeaseTime lease_time;
static uint32_t dhcp_time, next_dhcp_time;

static uint32_t dhcp_xid;
static uint8_t dhcp_socket = 7;

uint16_t htons(uint16_t hostshort) {
	uint16_t ret = 0;
	ret = (hostshort & 0xFF) << 8;
	ret |= ((hostshort >> 8)& 0xFF);
	return ret;
}

uint32_t htonl(uint32_t hostlong) {
	uint32_t ret=0;
	ret = (hostlong & 0xFF) << 24;
	ret |= ((hostlong >> 8) & 0xFF) << 16;
	ret |= ((hostlong >> 16) & 0xFF) << 8;
	ret |= ((hostlong >> 24) & 0xFF);
	return ret;
}

uint32_t ntohl(uint32_t netlong) {
	return htonl(netlong);
}

char int_to_char(const uint8_t value) {
	if(value >= 0 && value <= 9) {
		return '0' + value;
	}

	if(value >= 10 && value <= 15) {
		return 'A' + value - 10;
	}

	return 'X';
}


void dhcp_reset_time(void) {
	dhcp_time = 0;
	next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
	retry_count = 0;
}


void dhcp_send_discover(uint8_t s) {
	DHCPMessage dhcp_message = {0};

	dhcp_dns_server_ip[0] = 0;
	dhcp_dns_server_ip[1] = 0;
	dhcp_dns_server_ip[2] = 0;
	dhcp_dns_server_ip[3] = 0;

	dhcp_real_dns_server_ip[0] = 0;
	dhcp_real_dns_server_ip[1] = 0;
	dhcp_real_dns_server_ip[2] = 0;
	dhcp_real_dns_server_ip[3] = 0;

	dhcp_message.op = DHCP_BOOTREQUEST;
	dhcp_message.htype = DHCP_HTYPE10MB;
	dhcp_message.hlen = DHCP_HLENETHERNET;
	dhcp_message.hops = DHCP_HOPS;
	dhcp_message.xid = htonl(dhcp_xid);
	dhcp_message.secs = htons(DHCP_SECS);
	dhcp_message.flags = htons(DHCP_FLAGSBROADCAST);
	dhcp_message.chaddr[0] = ethernet_low_level_mac[0];
	dhcp_message.chaddr[1] = ethernet_low_level_mac[1];
	dhcp_message.chaddr[2] = ethernet_low_level_mac[2];
	dhcp_message.chaddr[3] = ethernet_low_level_mac[3];
	dhcp_message.chaddr[4] = ethernet_low_level_mac[4];
	dhcp_message.chaddr[5] = ethernet_low_level_mac[5];

	uint32_t i = 0;
	// Magic Cookie
	dhcp_message.OPT[i++] = (char)((DHCP_MAGIC_COOKIE >> 24) & 0xFF);
	dhcp_message.OPT[i++] = (char)((DHCP_MAGIC_COOKIE >> 16) & 0xFF);
	dhcp_message.OPT[i++] = (char)((DHCP_MAGIC_COOKIE >> 8) & 0xFF);
	dhcp_message.OPT[i++] = (char)(DHCP_MAGIC_COOKIE & 0xFF);

	// Option Request Param
	dhcp_message.OPT[i++] = dhcpMessageType;
	dhcp_message.OPT[i++] = 0x01;
	dhcp_message.OPT[i++] = DHCP_DISCOVER;

	// Client identifier
	dhcp_message.OPT[i++] = dhcpClientIdentifier;
	dhcp_message.OPT[i++] = 0x07;
	dhcp_message.OPT[i++] = 0x01;
	dhcp_message.OPT[i++] = ethernet_low_level_mac[0];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[1];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[2];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[3];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[4];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[5];

	// Host name
	dhcp_message.OPT[i++] = hostName;
	dhcp_message.OPT[i++] = strlen(HOST_NAME) + 6; // length of hostname + 6
	strcpy((char*)&(dhcp_message.OPT[i]), HOST_NAME);

	i += strlen(HOST_NAME);

	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[3]/16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[3] % 16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[4]/16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[4] % 16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[5]/16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[5] % 16);

	dhcp_message.OPT[i++] = dhcpParamRequest;
	dhcp_message.OPT[i++] = 0x06;
	dhcp_message.OPT[i++] = subnetMask;
	dhcp_message.OPT[i++] = routersOnSubnet;
	dhcp_message.OPT[i++] = dns;
	dhcp_message.OPT[i++] = domainName;
	dhcp_message.OPT[i++] = dhcpT1value;
	dhcp_message.OPT[i++] = dhcpT2value;
	dhcp_message.OPT[i++] = endOption;

	uint8_t ip[4] = {255, 255, 255, 255};
	ethernet_low_level_write_data_udp(s, (uint8_t*)&dhcp_message, sizeof(DHCPMessage), ip, DHCP_SERVER_PORT);
}


void dhcp_send_request(uint8_t s) {
	DHCPMessage dhcp_message = {0};

	dhcp_message.op = DHCP_BOOTREQUEST;
	dhcp_message.htype = DHCP_HTYPE10MB;
	dhcp_message.hlen = DHCP_HLENETHERNET;
	dhcp_message.hops = DHCP_HOPS;
	dhcp_message.xid = htonl(dhcp_xid);
	dhcp_message.secs = htons(DHCP_SECS);

	if(dhcp_state < STATE_DHCP_LEASED) {
		dhcp_message.flags = htons(DHCP_FLAGSBROADCAST);
	} else {
		dhcp_message.flags = 0; // For Unicast
		dhcp_message.ciaddr[0] = ethernet_low_level_ip[0];
		dhcp_message.ciaddr[1] = ethernet_low_level_ip[1];
		dhcp_message.ciaddr[2] = ethernet_low_level_ip[2];
		dhcp_message.ciaddr[3] = ethernet_low_level_ip[3];
	}

	dhcp_message.chaddr[0] = ethernet_low_level_mac[0];
	dhcp_message.chaddr[1] = ethernet_low_level_mac[1];
	dhcp_message.chaddr[2] = ethernet_low_level_mac[2];
	dhcp_message.chaddr[3] = ethernet_low_level_mac[3];
	dhcp_message.chaddr[4] = ethernet_low_level_mac[4];
	dhcp_message.chaddr[5] = ethernet_low_level_mac[5];

	uint32_t i = 0;
	// Magic Cookie
	dhcp_message.OPT[i++] = (uint8_t)((DHCP_MAGIC_COOKIE >> 24) & 0xFF);
	dhcp_message.OPT[i++] = (uint8_t)((DHCP_MAGIC_COOKIE >> 16) & 0xFF);
	dhcp_message.OPT[i++] = (uint8_t)((DHCP_MAGIC_COOKIE >> 8) & 0xFF);
	dhcp_message.OPT[i++] = (uint8_t)(DHCP_MAGIC_COOKIE & 0xFF);

	// Option Request Param
	dhcp_message.OPT[i++] = dhcpMessageType;
	dhcp_message.OPT[i++] = 0x01;
	dhcp_message.OPT[i++] = DHCP_REQUEST;

	dhcp_message.OPT[i++] = dhcpClientIdentifier;
	dhcp_message.OPT[i++] = 0x07;
	dhcp_message.OPT[i++] = 0x01;
	dhcp_message.OPT[i++] = ethernet_low_level_mac[0];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[1];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[2];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[3];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[4];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[5];

	if(dhcp_state < STATE_DHCP_LEASED) {
		dhcp_message.OPT[i++] = dhcpRequestedIPaddr;
		dhcp_message.OPT[i++] = 0x04;
		dhcp_message.OPT[i++] = ethernet_low_level_ip[0];
		dhcp_message.OPT[i++] = ethernet_low_level_ip[1];
		dhcp_message.OPT[i++] = ethernet_low_level_ip[2];
		dhcp_message.OPT[i++] = ethernet_low_level_ip[3];

		dhcp_message.OPT[i++] = dhcpServerIdentifier;
		dhcp_message.OPT[i++] = 0x04;
		dhcp_message.OPT[i++] = dhcp_dns_server_ip[0];
		dhcp_message.OPT[i++] = dhcp_dns_server_ip[1];
		dhcp_message.OPT[i++] = dhcp_dns_server_ip[2];
		dhcp_message.OPT[i++] = dhcp_dns_server_ip[3];
	}

	// Host name
	dhcp_message.OPT[i++] = hostName;
	dhcp_message.OPT[i++] = strlen(HOST_NAME) + 6; // length of hostname + 6
	strcpy((char*)&(dhcp_message.OPT[i]), HOST_NAME);

	i += strlen(HOST_NAME);

	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[3]/16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[3] % 16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[4]/16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[4] % 16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[5]/16);
	dhcp_message.OPT[i++] = int_to_char(ethernet_low_level_mac[5] % 16);

	dhcp_message.OPT[i++] = dhcpParamRequest;
	dhcp_message.OPT[i++] = 0x08;
	dhcp_message.OPT[i++] = subnetMask;
	dhcp_message.OPT[i++] = routersOnSubnet;
	dhcp_message.OPT[i++] = dns;
	dhcp_message.OPT[i++] = domainName;
	dhcp_message.OPT[i++] = dhcpT1value;
	dhcp_message.OPT[i++] = dhcpT2value;
	dhcp_message.OPT[i++] = performRouterDiscovery;
	dhcp_message.OPT[i++] = staticRoute;
	dhcp_message.OPT[i++] = endOption;

	// Send broadcasting packet
	uint8_t ip[4];
	if(dhcp_state < STATE_DHCP_LEASED) {
		ip[0] = 255;
		ip[1] = 255;
		ip[2] = 255;
		ip[3] = 255;
	} else {
		ip[0] = dhcp_dns_server_ip[0];
		ip[1] = dhcp_dns_server_ip[1];
		ip[2] = dhcp_dns_server_ip[2];
		ip[3] = dhcp_dns_server_ip[3];
	}

	ethernet_low_level_write_data_udp(s, (uint8_t*)&dhcp_message, sizeof(DHCPMessage), ip, DHCP_SERVER_PORT);
}


void dhcp_send_release_decline(uint8_t s, char msgtype) {
	DHCPMessage dhcp_message = {0};

	dhcp_message.op = DHCP_BOOTREQUEST;
	dhcp_message.htype = DHCP_HTYPE10MB;
	dhcp_message.hlen = DHCP_HLENETHERNET;
	dhcp_message.hops = DHCP_HOPS;
	dhcp_message.xid = htonl(dhcp_xid);
	dhcp_message.secs = htons(DHCP_SECS);
	dhcp_message.flags = 0;	//DHCP_FLAGSBROADCAST;

	dhcp_message.chaddr[0] = ethernet_low_level_mac[0];
	dhcp_message.chaddr[1] = ethernet_low_level_mac[1];
	dhcp_message.chaddr[2] = ethernet_low_level_mac[2];
	dhcp_message.chaddr[3] = ethernet_low_level_mac[3];
	dhcp_message.chaddr[4] = ethernet_low_level_mac[4];
	dhcp_message.chaddr[5] = ethernet_low_level_mac[5];

	uint32_t i = 0;

	// Magic Cookie
	dhcp_message.OPT[i++] = (uint8_t)((DHCP_MAGIC_COOKIE >> 24) & 0xFF);
	dhcp_message.OPT[i++] = (uint8_t)((DHCP_MAGIC_COOKIE >> 16) & 0xFF);
	dhcp_message.OPT[i++] = (uint8_t)((DHCP_MAGIC_COOKIE >> 8) & 0xFF);
	dhcp_message.OPT[i++] = (uint8_t)(DHCP_MAGIC_COOKIE & 0xFF);

	// Option Request Param
	dhcp_message.OPT[i++] = dhcpMessageType;
	dhcp_message.OPT[i++] = 0x01;
	dhcp_message.OPT[i++] = ((!msgtype) ? DHCP_RELEASE : DHCP_DECLINE);

	dhcp_message.OPT[i++] = dhcpClientIdentifier;
	dhcp_message.OPT[i++] = 0x07;
	dhcp_message.OPT[i++] = 0x01;
	dhcp_message.OPT[i++] = ethernet_low_level_mac[0];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[1];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[2];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[3];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[4];
	dhcp_message.OPT[i++] = ethernet_low_level_mac[5];

	dhcp_message.OPT[i++] = dhcpServerIdentifier;
	dhcp_message.OPT[i++] = 0x04;
	dhcp_message.OPT[i++] = dhcp_dns_server_ip[0];
	dhcp_message.OPT[i++] = dhcp_dns_server_ip[1];
	dhcp_message.OPT[i++] = dhcp_dns_server_ip[2];
	dhcp_message.OPT[i++] = dhcp_dns_server_ip[3];

	if(msgtype) {
		dhcp_message.OPT[i++] = dhcpRequestedIPaddr;
		dhcp_message.OPT[i++] = 0x04;
		dhcp_message.OPT[i++] = ethernet_low_level_ip[0];
		dhcp_message.OPT[i++] = ethernet_low_level_ip[1];
		dhcp_message.OPT[i++] = ethernet_low_level_ip[2];
		dhcp_message.OPT[i++] = ethernet_low_level_ip[3];
		dhcp_message.OPT[i++] = endOption;
	} else {
		dhcp_message.OPT[i++] = endOption;
	}

	uint8_t ip[4];
	if(!msgtype) {
		ip[0] = dhcp_dns_server_ip[0];
		ip[1] = dhcp_dns_server_ip[1];
		ip[2] = dhcp_dns_server_ip[2];
		ip[3] = dhcp_dns_server_ip[3];
	} else {
		ip[0] = 255;
		ip[1] = 255;
		ip[2] = 255;
		ip[3] = 255;
	}

	ethernet_low_level_write_data_udp(s, (uint8_t*)&dhcp_message, sizeof(DHCPMessage), ip, DHCP_SERVER_PORT);
}


char dhcp_parse_msg(uint8_t s, uint32_t length) {
	uint8_t svr_addr[6];
	uint16_t svr_port;

	DHCPMessage dhcp_message = {0};

	uint32_t len;
	uint8_t *p;
	uint8_t *e;
	uint8_t type, opt_len=0;

	len = ethernet_low_level_read_data_udp(s, (uint8_t *)&dhcp_message, length, svr_addr, &svr_port);

	if(dhcp_message.op != DHCP_BOOTREPLY || len == 0) {
		logethi("No dhcp message\n\r");
	} else {
		if(svr_port == DHCP_SERVER_PORT) {
			if(memcmp(dhcp_message.chaddr, ethernet_low_level_mac, 6) != 0 || dhcp_message.xid != htonl(dhcp_xid)) {
				return 0;
			}

			if(dhcp_dns_server_ip[0] != 0 ||
			   dhcp_dns_server_ip[1] != 0 ||
			   dhcp_dns_server_ip[2] != 0 ||
			   dhcp_dns_server_ip[3] != 0) {
				if((dhcp_real_dns_server_ip[0] != svr_addr[0] ||
				    dhcp_real_dns_server_ip[1] != svr_addr[1] ||
				    dhcp_real_dns_server_ip[2] != svr_addr[2] ||
				    dhcp_real_dns_server_ip[3] != svr_addr[3]) &&
				   (dhcp_dns_server_ip[0] != svr_addr[0] ||
			        dhcp_dns_server_ip[1] != svr_addr[1] ||
			        dhcp_dns_server_ip[2] != svr_addr[2] ||
			        dhcp_dns_server_ip[3] != svr_addr[3])) {
					return 0;
				}
			}

			memcpy(ethernet_low_level_ip, dhcp_message.yiaddr, 4);
			logethi("New dhcp ip: %d.%d.%d.%d\n\r", ethernet_low_level_ip[0], ethernet_low_level_ip[1], ethernet_low_level_ip[2], ethernet_low_level_ip[3]);

			type = 0;
			p = (uint8_t *)(&dhcp_message.op);
			p = p + 240;
			e = p + (len - 240);

			while(p < e) {
				switch(*p++) {
					case endOption: {
						return type;
					}

					case padOption: {
						break;
					}

					case dhcpMessageType: {
						opt_len = *p++;
						type = *p;

						break;
					}

					case subnetMask: {
						opt_len =* p++;
						memcpy(ehternet_low_level_subnet_mask, p, 4);

						break;
					}

					case routersOnSubnet: {
						opt_len = *p++;
						memcpy(ethernet_low_level_gw, p, 4);

						break;
					}

					case dns: {
						opt_len = *p++;
						memcpy(ethernet_low_level_dns, p, 4);
						break;
					}

					case dhcpIPaddrLeaseTime: {
						opt_len = *p++;
						lease_time.l_val = ntohl(*((uint32_t*)p));
						break;
					}

					case dhcpServerIdentifier: {
						opt_len = *p++;

						if((dhcp_dns_server_ip[0] == 0 && dhcp_dns_server_ip[1] == 0 && dhcp_dns_server_ip[2] == 0 && dhcp_dns_server_ip[3] == 0) ||
						   (dhcp_real_dns_server_ip[0] != svr_addr[0] || dhcp_real_dns_server_ip[1] != svr_addr[1] || dhcp_real_dns_server_ip[2] != svr_addr[2] || dhcp_real_dns_server_ip[3] != svr_addr[3]) ||
						   (dhcp_dns_server_ip[0] != svr_addr[0] || dhcp_dns_server_ip[1] != svr_addr[1] || dhcp_dns_server_ip[2] != svr_addr[2] || dhcp_dns_server_ip[3] != svr_addr[3])) {
							memcpy(dhcp_dns_server_ip, p, 4);
							memcpy(dhcp_real_dns_server_ip, svr_addr, 4);
						}

						break;
					}

					default: {
						opt_len = *p++;

						break;
					}
				}

				p += opt_len;
			}
		}
	}
	return 0;
}


bool dhcp_create_socket(uint8_t s) {
	logethd("DHCP Socket %d: start initialization\n\r", s);
	if(ethernet_low_level_get_status(s) != ETH_VAL_SN_SR_SOCK_CLOSED) {
		return false;
	}

	ethernet_write_register(ETH_REG_SN_MR | ETH_REG_SOCKET_NUM(s), ETH_VAL_SN_MR_PROTO_UDP);
	ethernet_write_register((ETH_REG_SN_PORT | ETH_REG_SOCKET_NUM(s)), (DHCP_CLIENT_PORT & 0xFF00) >> 8);
	ethernet_write_register((ETH_REG_SN_PORT | ETH_REG_SOCKET_NUM(s)) + 1, DHCP_CLIENT_PORT & 0x00FF);
	ethernet_write_register(ETH_REG_SN_CR | ETH_REG_SOCKET_NUM(s), ETH_VAL_SN_CR_OPEN);

	while(ethernet_low_level_get_status(s) != ETH_VAL_SN_SR_SOCK_UDP);
	logethd("DHCP Socket %d: initialized\n\r", s);

	return true;
}

void dhcp_check_state(uint8_t s) {
	if(dhcp_state == STATE_DHCP_DISCOVER && DHCP_timeout == 1) {
		DHCP_timeout = 0;
		logethw("DHCP timeout, trying again\n\r");
		return;
	}

	uint32_t len;
	uint8_t type = 0;

	if(ethernet_low_level_get_status(s) != ETH_VAL_SN_SR_SOCK_CLOSED) {
		if((len = ethernet_low_level_get_received_data_length(s)) > 0) {
			type = dhcp_parse_msg(s, len);
		}
	} else {
		dhcp_create_socket(s);
	}


	switch( dhcp_state ) {
		case STATE_DHCP_DISCOVER: {
			if(type == DHCP_OFFER) {
				dhcp_reset_time();
				dhcp_send_request(s);
				dhcp_state = STATE_DHCP_REQUEST;
			} else {
				dhcp_check_timeout();
			}
			break;
		}

		case STATE_DHCP_REQUEST: {
			if(type == DHCP_ACK) {
				dhcp_reset_time();
				if(dhcp_check_leased_ip()) {
					dhcp_set_network();
					dhcp_state = STATE_DHCP_LEASED;
				} else {
					dhcp_state = STATE_DHCP_DISCOVER;
				}
			} else if(type == DHCP_NAK) {
				dhcp_reset_time();
				dhcp_state = STATE_DHCP_DISCOVER;
			} else {
				dhcp_check_timeout();
			}
			break;
		}

		case STATE_DHCP_LEASED: {
			if((lease_time.l_val != 0xffffffff) && ((lease_time.l_val/2) < dhcp_time)) {
				type = 0;
				memcpy(dhcp_old_ip, ethernet_low_level_ip, 4);
				dhcp_xid++;
				dhcp_send_request(s);
				dhcp_state = STATE_DHCP_REREQUEST;
				dhcp_reset_time();
			}
			break;
		}

		case STATE_DHCP_REREQUEST: {
			if(type == DHCP_ACK) {
				memcmp(dhcp_old_ip, ethernet_low_level_ip, 4);

				dhcp_reset_time();
				dhcp_state = STATE_DHCP_LEASED;
			} else if(type == DHCP_NAK) {
				dhcp_reset_time();
				dhcp_state = STATE_DHCP_DISCOVER;
			} else {
				dhcp_check_timeout();
			}
			break;
		}

		case STATE_DHCP_RELEASE: {
			break;
		}

		default: {
			break;
		}
	}
}


void dhcp_check_timeout(void) {
	if(retry_count < DHCP_MAX_RETRY) {
		if(next_dhcp_time < dhcp_time) {
			dhcp_time = 0;
			next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
			retry_count++;

			switch(dhcp_state) {
				case STATE_DHCP_DISCOVER: {
					dhcp_send_discover(dhcp_socket);
					break;
				}

				case STATE_DHCP_REQUEST: {
					dhcp_send_request(dhcp_socket);
					break;
				}

				case STATE_DHCP_REREQUEST: {
					dhcp_send_request(dhcp_socket);
					break;
				}

				default : {
					break;
				}
			}
		}
	} else {
		dhcp_reset_time();
		DHCP_timeout = 1;

		dhcp_send_discover(dhcp_socket);
		dhcp_state = STATE_DHCP_DISCOVER;
	}
}


void dhcp_set_network(void) {
	ethernet_write_buffer(ETH_REG_GATEWAY_ADDRESS, ethernet_low_level_gw, ETHERNET_IP_SIZE);
	ethernet_write_buffer(ETH_REG_SUBNET_MASK, ehternet_low_level_subnet_mask, ETHERNET_IP_SIZE);
	ethernet_write_buffer(ETH_REG_SOURCE_IP_ADDRESS, ethernet_low_level_ip, ETHERNET_IP_SIZE);
}


char dhcp_check_leased_ip(void) {
	uint32_t a = 0;

	if(a > 0) {
		dhcp_send_release_decline(dhcp_socket, 1);
		return 0;
	}
	return 1;
}


void dhcp_get_ip(void) {
	dhcp_send_discover(dhcp_socket);
	dhcp_state = STATE_DHCP_DISCOVER;

	dhcp_reset_time();
	DHCP_timeout = 0;
}

void dhcp_tick(uint8_t tick_type) {
	static uint16_t counter = 0;
	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		return;
	}

	counter++;
	if(counter >= 1000) {
		counter = 0;
		dhcp_time++;
	}
}

uint32_t dhcp_init_socket(uint8_t s) {
	dhcp_xid = DHCP_START_XID;
	memset(ehternet_low_level_subnet_mask, 0, 4);
	memset(ethernet_low_level_gw, 0, 4);
	memset(ehternet_low_level_subnet_mask, 0, 4);

	ethernet_write_buffer(ETH_REG_SOURCE_HW_ADDRESS, ethernet_low_level_mac, ETHERNET_MAC_SIZE);
	ethernet_write_buffer(ETH_REG_GATEWAY_ADDRESS, ethernet_low_level_gw, ETHERNET_IP_SIZE);
	ethernet_write_buffer(ETH_REG_SUBNET_MASK, ehternet_low_level_subnet_mask, ETHERNET_IP_SIZE);
	ethernet_write_buffer(ETH_REG_SOURCE_IP_ADDRESS, ethernet_low_level_ip, ETHERNET_IP_SIZE);

	dhcp_socket = s;
	return dhcp_create_socket(s);
}
