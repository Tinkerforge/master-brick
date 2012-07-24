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

#ifndef CHIBI_CONFIG_H
#define CHIBI_CONFIG_H

#include "extensions/extension_config.h"

#define WIFI_MAX_DATA_LENGTH 64
#define WIFI_BUFFER_SIZE WIFI_MAX_DATA_LENGTH

#define PIN_WIFI_RXD     {1 << 21, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_WIFI_TXD     {1 << 22, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_WIFI_RTS     {1 << 24, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

#define PIN_WIFI_CTS     PIN_EXT_0_GP_1
#define PIN_WIFI_RESET   PIN_EXT_0_GP_0
#define PIN_WIFI_WAKE    PIN_EXT_0_GP_2
#define PIN_WIFI_SELECT  PIN_EXT_0_SELECT

#define PIN_WIFI_SPI_MISO    PIN_EXT_RXD
#define PIN_WIFI_SPI_MOSI    PIN_EXT_TXD
#define PIN_WIFI_SPI_CLK     PIN_EXT_SCK
#define PIN_WIFI_SPI_CS      PIN_EXT_0_GP_0
#define PIN_WIFI_SPI_WAKEUP  PIN_EXT_0_SELECT

#define USART_WIFI_SPI       USART0
#define ID_WIFI_SPI          ID_USART0
#define IRQ_WIFI_SPI         USART0_IRQn
#define PINS_WIFI_SPI        PIN_EXT_RXD, \
                             PIN_EXT_TXD, \
                             PIN_EXT_SCK, \
                             PIN_EXT_0_GP_0, \
                             PIN_EXT_0_SELECT


/*#define USART_WIFI    USART1
#define ID_WIFI       ID_USART1
#define IRQ_WIFI      USART1_IRQn
#define PINS_WIFI     {PIN_WIFI_RXD, PIN_WIFI_TXD, PIN_WIFI_RTS}

#define WIFI_USART_CLOCK 9600*/
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
