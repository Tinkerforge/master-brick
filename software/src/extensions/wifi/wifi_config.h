/* master-brick
 * Copyright (C) 2012-2013 Olaf Lüke <olaf@tinkerforge.com>
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

#define WIFI_KEY (42 | (23 << 8) | (17 << 16) | 8)
#define WIFI_KEY_POS 4
#define WIFI_CONFIGURATION_POS 8
#define WIFI_ENCRYPTION_POS (WIFI_CONFIGURATION_POS + sizeof(SetWifiConfiguration) - sizeof(MessageHeader))
#define WIFI_REGULATORY_DOMAIN_POS (WIFI_ENCRYPTION_POS + sizeof(SetWifiEncryption) - sizeof(MessageHeader))
#define WIFI_USERNAME_POS (WIFI_REGULATORY_DOMAIN_POS + sizeof(SetWifiRegulatoryDomain) - sizeof(MessageHeader))
#define WIFI_PASSWORD_POS (WIFI_USERNAME_POS + 32)
#define WIFI_CA_CERTIFICATE_POS 1024
#define WIFI_CLIENT_CERTIFICATE_POS (WIFI_CA_CERTIFICATE_POS + WIFI_CA_CERTIFICATE_MAX_LENGTH)
#define WIFI_PRIVATE_KEY_POS (WIFI_CLIENT_CERTIFICATE_POS + WIFI_CLIENT_CERTIFICATE_MAX_LENGTH)
#define WIFI_CA_CERTIFICATE_MAX_LENGTH 1312
#define WIFI_CLIENT_CERTIFICATE_MAX_LENGTH 1312
#define WIFI_PRIVATE_KEY_MAX_LENGTH 4320

#define WIFI_MAX_DATA_LENGTH     80
#define WIFI_BUFFER_SIZE         WIFI_MAX_DATA_LENGTH
#define WIFI_COMMAND_BUFFER_SIZE 128

#define USART_WIFI_SPI       USART0
#define ID_WIFI_SPI          ID_USART0
#define IRQ_WIFI_SPI         USART0_IRQn

#define WIFI_CS_0        0
#define WIFI_CS_1        4
#define WIFI_RESET_0     1
#define WIFI_RESET_1     5
#define WIFI_LED_0       2
#define WIFI_LED_1       6
#define WIFI_DATA_RDY_0  3
#define WIFI_DATA_RDY_1  7

#define WIFI_MISO        8
#define WIFI_MOSI        9
#define WIFI_CLK        10

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
