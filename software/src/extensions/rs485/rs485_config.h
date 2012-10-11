/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rs485_config.h: Configuration of RS485 protocol implementation
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

#ifndef RS485_CONFIG_H
#define RS485_CONFIG_H

#include "bricklib/logging/logging.h"

#define DEBUG_RS485 1

#define RS485_BAUDRATE 2000000
#define RS485_MAX_DATA_LENGTH 64
#define RS485_BUFFER_SIZE RS485_MAX_DATA_LENGTH

#define PIN_RS485_RXD  {1 << 21, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_RS485_TXD  {1 << 22, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_RS485_RTS  {1 << 24, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

#define RS485_RECV_0 1
#define RS485_RECV_1 5

#define USART_RS485    USART1
#define ID_RS485       ID_USART1
#define IRQ_RS485      USART1_IRQn
#define PINS_RS485     {PIN_RS485_RXD, PIN_RS485_TXD, PIN_RS485_RTS}

#define RS485_MODE_NONE    0
#define RS485_MODE_RECEIVE 1
#define RS485_MODE_SEND    2

#define RS485_TYPE_NONE    0
#define RS485_TYPE_MASTER  1
#define RS485_TYPE_SLAVE   2

#define RS485_OK           1
#define RS485_NOK          0xFE
#define RS485_NOK_SEND     0x7F

#define RS485_NUM_SLAVE_ADDRESS  32

#if(DEBUG_RS485)
#define logrsd(str, ...) do{logd("rs485: " str, ##__VA_ARGS__);}while(0)
#define logrsi(str, ...) do{logi("rs485: " str, ##__VA_ARGS__);}while(0)
#define logrsw(str, ...) do{logw("rs485: " str, ##__VA_ARGS__);}while(0)
#define logrse(str, ...) do{loge("rs485: " str, ##__VA_ARGS__);}while(0)
#define logrsf(str, ...) do{logf("rs485: " str, ##__VA_ARGS__);}while(0)
#else
#define logrsd(str, ...) {}
#define logrsi(str, ...) {}
#define logrsw(str, ...) {}
#define logrse(str, ...) {}
#define logcrsf(str, ...) {}
#endif

#endif
