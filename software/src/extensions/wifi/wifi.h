/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi.h: High Level WIFI protocol implementation
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

#ifndef WIFI_H
#define WIFI_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	WIFI_STATE_COMMAND_IDLE,
	WIFI_STATE_COMMAND_SEND,
	WIFI_STATE_COMMAND_RECV
} WIFIState;

bool wifi_init(void);
uint16_t wifi_send(const void *data, const uint16_t length);
uint16_t wifi_recv(void *data, const uint16_t length);
void wifi_message_loop(void *parameters);
void wifi_message_loop_return(char *data, uint16_t length);
void wifi_init_extension(uint8_t extension);

#endif
