/* master-brick
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi2_uart.h: Low level WIFI2 communication with ESP8266
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

#ifndef WIFI2_UART_H
#define WIFI2_UART_H

#include <stdint.h>
#include <stdbool.h>

#define WIFI2_ACK_TIMEOUT_MS 250
#define WIFI2_UART_HEADER_SIZE 3
#define WIFI2_UART_BUFFER_SIZE (80+WIFI2_UART_HEADER_SIZE)

#define UART_CON_INDEX_LENGTH        0
#define UART_CON_INDEX_SEQUENCE      1
#define UART_CON_INDEX_TFP_START     2
#define UART_CON_INDEX_CHECKSUM(len) (len - 1)

typedef struct {
	uint8_t enable;
	uint8_t nreset;
	uint8_t flashc;

	uint8_t recv_dma_buffer[2][WIFI2_UART_BUFFER_SIZE];
	uint8_t recv_dma_last_rcr;
	uint8_t recv_dma_index;

	uint8_t recv_buffer[WIFI2_UART_BUFFER_SIZE];
	uint8_t recv_buffer_current_index;
	uint8_t recv_buffer_expected_length;

	uint8_t recv_buffer_tfp[WIFI2_UART_BUFFER_SIZE];
	uint8_t recv_buffer_tfp_length;
	bool    recv_buffer_tfp_wait;

	uint8_t uart_sequence_number;
	uint8_t uart_last_sequence_number_seen;

	uint8_t send_buffer[WIFI2_UART_BUFFER_SIZE];
	uint8_t send_buffer_message_length;

	uint8_t send_dma_buffer[WIFI2_UART_BUFFER_SIZE];
	uint8_t send_dma_buffer_length;
	uint16_t send_timeout;

	uint8_t ack_buffer[WIFI2_UART_HEADER_SIZE];
	bool ack_wait;

	bool bootloader_mode;
} Wifi2;

bool wifi2_uart_rx(uint8_t *data);
void wifi2_uart_init(const uint8_t extension);
void wifi2_uart_tick(const uint8_t tick_type);

#endif
