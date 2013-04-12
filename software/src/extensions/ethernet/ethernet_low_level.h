/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_low_level.h: Low level Ethernet protocol implementation
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

#ifndef ETHERNET_LOW_LEVEL_H
#define ETHERNET_LOW_LEVEL_H

#include <stdint.h>

#include "ethernet_config.h"

#define ETH_REG_MODE                   0x0000
#define ETH_REG_GATEWAY_ADDRESS        0x0001
#define ETH_REG_SUBNET_MASK            0x0005
#define ETH_REG_SOURCE_HW_ADDRESS      0x0009
#define ETH_REG_SOURCE_IP_ADDRESS      0x000F
#define ETH_REG_INTERRUPT              0x0015
#define ETH_REG_INTERRUPT_MASK         0x0016
#define ETH_REG_RETRY_TIME             0x0017
#define ETH_REG_RETRY_COUNT            0x0019
#define ETH_REG_AUTH_TYPE              0x001C
#define ETH_REG_AUTH_ALGORITHM         0x001E
#define ETH_REG_CHIP_VERSION           0x001F
#define ETH_REG_PPP_LCP_REQUEST_TIMER  0x0028
#define ETH_REG_PPP_LCP_MAGIC_NUMBER   0x0029
#define ETH_REG_INTERRUPT_LOW_LEVEL    0x0030
#define ETH_REG_SOCKET_INTERRUPT       0x0034
#define ETH_REG_PHY_STATUS             0x0035
#define ETH_REG_SOCKET_INTERRUPT_MASK  0x0036

#define ETH_REG_SN_MR                  0x4000
#define ETH_REG_SN_CR                  0x4001
#define ETH_REG_SN_IR                  0x4002
#define ETH_REG_SN_SR                  0x4003
#define ETH_REG_SN_PORT                0x4004
#define ETH_REG_SN_DHAR                0x4006
#define ETH_REG_SN_DIPR                0x400C
#define ETH_REG_SN_DPORT               0x4010
#define ETH_REG_SN_MSSR                0x4012
#define ETH_REG_SN_PROTO               0x4014
#define ETH_REG_SN_TOS                 0x4015
#define ETH_REG_SN_TTL                 0x4016
#define ETH_REG_SN_RXMEM_SIZE          0x401E
#define ETH_REG_SN_TXMEM_SIZE          0x401F
#define ETH_REG_SN_TX_FSR              0x4020
#define ETH_REG_SN_TX_RD               0x4022
#define ETH_REG_SN_TX_WR               0x4024
#define ETH_REG_SN_RX_RSR              0x4026
#define ETH_REG_SN_RX_RD               0x4028
#define ETH_REG_SN_RX_WR               0x402A
#define ETH_REG_SN_IMR                 0x402C
#define ETH_REG_SN_FRAG                0x402D

#define ETH_REG_SOCKET_NUM(n)          (0x0100*(n))


#define ETH_VAL_MODE_SW_RESET          (1 << 7)
#define ETH_VAL_MODE_PB                (1 << 6)
#define ETH_VAL_MODE_PPOE              (1 << 3)

#define ETH_VAL_PHY_STATUS_LINK        (1 << 5)
#define ETH_VAL_PHY_STATUS_POWERDOWN   (1 << 3)

#define ETH_VAL_SN_MR_MULTI            (1 << 7)
#define ETH_VAL_SN_MR_MF               (1 << 6)
#define ETH_VAL_SN_MR_ND_MC            (1 << 5)
#define ETH_VAL_SN_MR_PROTO_CLOSED     (0 << 0)
#define ETH_VAL_SN_MR_PROTO_TCP        (1 << 0)
#define ETH_VAL_SN_MR_PROTO_UDP        (2 << 0)
#define ETH_VAL_SN_MR_PROTO_IPRAW      (3 << 0)
#define ETH_VAL_SN_MR_PROTO_MACRAW     (4 << 0)
#define ETH_VAL_SN_MR_PROTO_PPPOE      (5 << 0)


#define ETH_VAL_SN_CR_OPEN             0x01
#define ETH_VAL_SN_CR_LISTEN           0x02
#define ETH_VAL_SN_CR_CONNECT          0x04
#define ETH_VAL_SN_CR_DISCON           0x08
#define ETH_VAL_SN_CR_CLOSE            0x10
#define ETH_VAL_SN_CR_SEND             0x20
#define ETH_VAL_SN_CR_SEND_MAC         0x21
#define ETH_VAL_SN_CR_SEND_KEEP        0x22
#define ETH_VAL_SN_CR_RECV             0x40

#define ETH_VAL_SN_IR_PRECV            (1 << 7)
#define ETH_VAL_SN_IR_PFAIL            (1 << 6)
#define ETH_VAL_SN_IR_PNEXT            (1 << 5)
#define ETH_VAL_SN_IR_SEND_OK          (1 << 4)
#define ETH_VAL_SN_IR_TIMEOUT          (1 << 3)
#define ETH_VAL_SN_IR_RECV             (1 << 2)
#define ETH_VAL_SN_IR_DISCON           (1 << 1)
#define ETH_VAL_SN_IR_CON              (1 << 0)

#define ETH_VAL_SN_SR_SOCK_CLOSED      0x00
#define ETH_VAL_SN_SR_SOCK_INIT        0x13
#define ETH_VAL_SN_SR_SOCK_LISTEN      0x14
#define ETH_VAL_SN_SR_SOCK_ESTABLISHED 0x17
#define ETH_VAL_SN_SR_SOCK_CLOSE_WAIT  0x1C
#define ETH_VAL_SN_SR_SOCK_UDP         0x22
#define ETH_VAL_SN_SR_SOCK_IPRAW       0x32
#define ETH_VAL_SN_SR_SOCK_MACRAW      0x42
#define ETH_VAL_SN_SR_SOCK_PPOE        0x5F
#define ETH_VAL_SN_SR_SOCK_SYNSENT     0x15
#define ETH_VAL_SN_SR_SOCK_SYNRECV     0x16
#define ETH_VAL_SN_SR_SOCK_FIN_WAIT    0x18
#define ETH_VAL_SN_SR_SOCK_CLOSING     0x1A
#define ETH_VAL_SN_SR_SOCK_TIME_WAIT   0x1B
#define ETH_VAL_SN_SR_SOCK_LAST_ACK    0x1D
#define ETH_VAL_SN_SR_SOCK_ARP         0x01

#define ETH_VAL_SN_IMR_PRECV           (1 << 7)
#define ETH_VAL_SN_IMR_PFAIL           (1 << 6)
#define ETH_VAL_SN_IMR_PNEXT           (1 << 5)
#define ETH_VAL_SN_IMR_SENDOK          (1 << 4)
#define ETH_VAL_SN_IMR_TIMEOUT         (1 << 3)
#define ETH_VAL_SN_IMR_RECV            (1 << 2)
#define ETH_VAL_SN_IMR_DISCON          (1 << 1)
#define ETH_VAL_SN_IMR_CON             (1 << 0)

void ethernet_low_level_get_default_hostname(char hostname[ETHERNET_HOSTNAME_LENGTH]);
void ethernet_low_level_reset(void);
void ethernet_low_level_init(void);
void ethernet_low_level_disconnect(const uint8_t socket);
uint8_t ethernet_low_level_get_status(const uint8_t socket);
void ethernet_low_level_socket_init(const uint8_t socket);
void ethernet_low_level_socket_listen(const uint8_t socket);
uint16_t ethernet_low_level_get_received_data_length(const uint8_t socket);
uint16_t ethernet_low_level_get_free_data_length(const uint8_t socket);
uint16_t ethernet_low_level_get_transmit_pointer(const uint8_t socket);
void ethernet_low_level_set_transmit_pointer(const uint8_t socket, const uint16_t pointer);
uint8_t ethernet_low_level_read_data_tcp(const uint8_t socket, uint8_t *buffer, const uint8_t length);
uint8_t ethernet_low_level_write_data_tcp(const uint8_t socket, const uint8_t *buffer, const uint8_t length);
uint16_t ethernet_low_level_read_data_udp(const uint8_t socket, uint8_t *buffer, const uint16_t length, uint8_t ip[4], uint16_t *port);
uint16_t ethernet_low_level_write_data_udp(const uint8_t socket, const uint8_t *buffer, const uint16_t length, const uint8_t ip[4], const uint16_t port);
uint16_t ethernet_low_level_get_receive_pointer(const uint8_t socket);
void ethernet_low_level_set_receive_pointer(const uint8_t socket, const uint16_t pointer);
void ethernet_low_level_set_retry_time(const uint16_t retry_time);
void ethernet_low_level_set_retry_count(const uint8_t retry_count);
void ethernet_select(void);
void ethernet_deselect(void);
uint8_t ethernet_transceive_byte(const uint8_t value);
uint8_t ethernet_read_register(const uint16_t address);
void ethernet_write_register(const uint16_t address, const uint8_t value);
uint8_t ethernet_read_buffer(const uint16_t address,
                             uint8_t *buffer,
                             const uint16_t length);

uint8_t ethernet_write_buffer(const uint16_t address,
                              const uint8_t *buffer,
                              const uint16_t length);

#endif
