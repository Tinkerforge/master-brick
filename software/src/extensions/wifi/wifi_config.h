/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_config.h: Configuration of WIFI implementation
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

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#include "extensions/extension_config.h"

#define WIFI_CONFIGURATION_POS 0
#define WIFI_ENCRYPTION_POS (sizeof(SetWifiConfiguration) - 4)

#define WIFI_MAX_DATA_LENGTH 64
#define WIFI_BUFFER_SIZE WIFI_MAX_DATA_LENGTH

#define PIN_WIFI_RESET       PIN_EXT_0_GP_0
#define PIN_WIFI_LED         PIN_EXT_0_GP_1
#define PIN_WIFI_DATA_RDY    PIN_EXT_0_GP_2

#define PIN_WIFI_SPI_MISO    PIN_EXT_RXD
#define PIN_WIFI_SPI_MOSI    PIN_EXT_TXD
#define PIN_WIFI_SPI_CLK     PIN_EXT_SCK
#define PIN_WIFI_SPI_CS      PIN_EXT_0_SELECT

#define USART_WIFI_SPI       USART0
#define ID_WIFI_SPI          ID_USART0
#define IRQ_WIFI_SPI         USART0_IRQn
#define PINS_WIFI_SPI        PIN_WIFI_SPI_MISO, \
                             PIN_WIFI_SPI_MOSI, \
                             PIN_WIFI_SPI_CLK, \
                             PIN_WIFI_SPI_CS, \
                             PIN_WIFI_RESET, \
                             PIN_WIFI_LED, \
                             PIN_WIFI_DATA_RDY


#define WIFI_SPI_CLOCK 2000000

#define DEBUG_WIFI 1

#if(DEBUG_WIFI)
#define logwifid(str, ...) do{logd("wifi: " str, ##__VA_ARGS__);}while(0)
#define logwifii(str, ...) do{logi("wifi: " str, ##__VA_ARGS__);}while(0)
#define logwifiw(str, ...) do{logw("wifi: " str, ##__VA_ARGS__);}while(0)
#define logwifie(str, ...) do{loge("wifi: " str, ##__VA_ARGS__);}while(0)
#define logwifif(str, ...) do{logf("wifi: " str, ##__VA_ARGS__);}while(0)
#else
#define logwifid(str, ...) {}
#define logwifii(str, ...) {}
#define logwifiw(str, ...) {}
#define logwifie(str, ...) {}
#define logwifif(str, ...) {}
#endif

#endif
