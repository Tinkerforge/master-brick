/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_data.h: Data mode functionality for WIFI Extension
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

#ifndef WIFI_DATA_H
#define WIFI_DATA_H

#include <stdint.h>
#include <stdbool.h>

#define WIFI_DATA_CHAR_ESC 0x1B
#define WIFI_DATA_CHAR_Z   'Z'
#define WIFI_DATA_CHAR_A   'A'

#define WIFI_DATA_MAX_CID 16

#define WIFI_DATA_ESCAPE_BUFFER_SIZE 7
#define WIFI_DATA_RINGBUFFER_SIZE 1500

typedef enum {
	WIFI_DATA_WAIT_FOR_ESC,
	WIFI_DATA_WAIT_FOR_ESC_CHAR,
	WIFI_DATA_WAIT_FOR_ASYNC,
	WIFI_DATA_WAIT_FOR_ASYNC_READ_SIZE,
	WIFI_DATA_WAIT_FOR_ASYNC_READ_MSG,
	WIFI_DATA_WAIT_FOR_CID,
	WIFI_DATA_WAIT_FOR_LENGTH,
	WIFI_DATA_WAIT_FOR_DATA
} WIFIDataState;

uint16_t wifi_data_get_ringbuffer_diff(void);
void wifi_data_next(const char data, bool transceive);
void wifi_data_send(const char *data, const uint16_t length);
void wifi_data_send_escape(const char *data, const uint16_t length);
void wifi_data_send_escape_cid(const char *data, const uint16_t length, const uint8_t cid);
void wifi_data_poll(void);
int8_t wifi_data_hex_to_int(const char c);
char wifi_data_int_to_hex(const int8_t c);
void wifi_data_disconnect(const uint8_t cid);

#endif
