/* master-brick
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi2_config.h: Configuration of WIFI2 implementation
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

#ifndef WIFI2_CONFIG_H
#define WIFI2_CONFIG_H

#include "extensions/extension_config.h"

#include "bricklib/logging/logging.h"

#define DEBUG_WIFI2 1

#define WIFI2_BAUDRATE 1000000
#define WIFI2_MAX_DATA_LENGTH 80
#define WIFI2_BUFFER_SIZE WIFI2_MAX_DATA_LENGTH

#define WIFI2_NRESET_0   1
#define WIFI2_NRESET_1   5
#define WIFI2_ENABLE_0   2
#define WIFI2_ENABLE_1   6
#define WIFI2_FLASHC_0   3
#define WIFI2_FLASHC_1   7
#define PIN_WIFI2_RXD    {1 << 5, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_WIFI2_TXD    {1 << 6, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}


#define USART_WIFI2    USART0
#define ID_WIFI2       ID_USART0
#define IRQ_WIFI2      USART0_IRQn
#define PINS_WIFI2     {PIN_WIFI2_RXD, PIN_WIFI2_TXD}

#if(DEBUG_WIFI2)
#define logwifi2d(str, ...) do{logd("wifi2: " str, ##__VA_ARGS__);}while(0)
#define logwifi2i(str, ...) do{logi("wifi2: " str, ##__VA_ARGS__);}while(0)
#define logwifi2w(str, ...) do{logw("wifi2: " str, ##__VA_ARGS__);}while(0)
#define logwifi2e(str, ...) do{loge("wifi2: " str, ##__VA_ARGS__);}while(0)
#define logwifi2f(str, ...) do{logf("wifi2: " str, ##__VA_ARGS__);}while(0)
#else
#define logwifi2d(str, ...) {}
#define logwifi2i(str, ...) {}
#define logwifi2w(str, ...) {}
#define logwifi2e(str, ...) {}
#define logwifi2f(str, ...) {}
#endif


#endif
