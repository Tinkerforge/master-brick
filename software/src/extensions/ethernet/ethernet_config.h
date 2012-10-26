/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_config.h: Configuration of Ethernet implementation
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

#ifndef ETHERNET_CONFIG_H
#define ETHERNET_CONFIG_H

#include "bricklib/logging/logging.h"

#define ETHERNET_MAX_DATA_LENGTH 64
#define ETHERNET_MAX_SOCKETS 7
#define ETHERNET_BUFFER_SIZE WIFI_MAX_DATA_LENGTH
#define ETHERNET_UDP_HEADER_SIZE 8

#define ETHERNET_USART_SPI_CLOCK 2000000
#define ETHERNET_READ_COMMAND    0x00
#define ETHERNET_WRITE_COMMAND   0x80

#define ETHERNET_CS_0        0
#define ETHERNET_CS_1        4
#define ETHERNET_RESET_0     1
#define ETHERNET_RESET_1     5
#define ETHERNET_INT_0       2
#define ETHERNET_INT_1       6
#define ETHERNET_PWDN_0      3
#define ETHERNET_PWDN_1      7

#define ETHERNET_MISO        8
#define ETHERNET_MOSI        9
#define ETHERNET_CLK        10

#define ETHERNET_RX_SIZE     2
#define ETHERNET_TX_SIZE     2
#define ETHERNET_MAC_SIZE    6
#define ETHERNET_IP_SIZE     4
#define ETHERNET_NUM_SOCKETS 8

#define ETHERNET_RX_BASE 0xC000
#define ETHERNET_TX_BASE 0x8000
#define ETHERNET_RX_MASK 0x07FF
#define ETHERNET_TX_MASK 0x07FF


#define DEBUG_ETHERNET 1

#if(DEBUG_ETHERNET)
#define logethd(str, ...) do{logd("eth: " str, ##__VA_ARGS__);}while(0)
#define logethi(str, ...) do{logi("eth: " str, ##__VA_ARGS__);}while(0)
#define logethw(str, ...) do{logw("eth: " str, ##__VA_ARGS__);}while(0)
#define logethe(str, ...) do{loge("eth: " str, ##__VA_ARGS__);}while(0)
#define logethf(str, ...) do{logf("eth: " str, ##__VA_ARGS__);}while(0)
#else
#define logethd(str, ...) {}
#define logethi(str, ...) {}
#define logethw(str, ...) {}
#define logethe(str, ...) {}
#define logethf(str, ...) {}
#endif

#endif
