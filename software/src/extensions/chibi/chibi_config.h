/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi_config.h: Configuration of chibi protocol implementation
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

#ifndef CHIBI_CONFIG_H
#define CHIBI_CONFIG_H

#include "extensions/extension_config.h"

#define CHIBI_SELECT_0    0
#define CHIBI_SELECT_1    4
#define CHIBI_RESET_0     1
#define CHIBI_RESET_1     5
#define CHIBI_INT_0       2
#define CHIBI_INT_1       6
#define CHIBI_SLP_TR_0    3
#define CHIBI_SLP_TR_1    7

#define CHIBI_MISO        8
#define CHIBI_MOSI        9
#define CHIBI_CLK        10

#define CHIBI_USART_SPI_CLOCK 8000000 // 8 Mhz

#define CHIBI_PAN_ID               0x1234

#define CHIBI_CSMA_RETRIES_SLAVE   5   // Retries for CSMA (max 5)
#define CHIBI_FRAME_RETRIES_SLAVE  15  // Retries if no ACK (max 15)
#define CHIBI_CSMA_RETRIES_MASTER  5
#define CHIBI_FRAME_RETRIES_MASTER 0
#define CHIBI_USE_CRC
//#define CHIBI_USE_PROMISCUOUS_MODE

#define CHIBI_NUM_SLAVE_ADDRESS 32
#define CHIBI_MAX_WAIT_FOR_RECV 5000
#define CHIBI_MAX_WAIT_FOR_SEND 5000

#define CHIBI_ADDRESS_FREQUENCY (EXTENSION_POS_DATA_START + 0)
#define CHIBI_ADDRESS_CHANNEL (EXTENSION_POS_DATA_START + 1)



#if(DEBUG_CHIBI)
#define logchibid(str, ...) do{logd("chibi: " str, ##__VA_ARGS__);}while(0)
#define logchibii(str, ...) do{logi("chibi: " str, ##__VA_ARGS__);}while(0)
#define logchibiw(str, ...) do{logw("chibi: " str, ##__VA_ARGS__);}while(0)
#define logchibie(str, ...) do{loge("chibi: " str, ##__VA_ARGS__);}while(0)
#define logchibif(str, ...) do{logf("chibi: " str, ##__VA_ARGS__);}while(0)
#else
#define logchibid(str, ...) {}
#define logchibii(str, ...) {}
#define logchibiw(str, ...) {}
#define logchibie(str, ...) {}
#define logchibif(str, ...) {}
#endif

#endif
