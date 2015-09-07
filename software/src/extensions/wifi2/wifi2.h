/* master-brick
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi2.h: High Level WIFI2 protocol implementation
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

#ifndef WIFI2_H
#define WIFI2_H

#include <stdint.h>
#include <stdbool.h>

bool wifi2_init(void);
void wifi2_init_extension(const uint8_t extension);
uint16_t wifi2_send(const void *data, const uint16_t length, uint32_t *options);
uint16_t wifi2_recv(void *data, const uint16_t length, uint32_t *options);
void wifi2_message_loop(void *parameters);
void wifi2_message_loop_return(const char *data, const uint16_t length);
void wifi2_tick(const uint8_t tick_type);
void wifi2_clear_dma_recv_buffer(void);

#endif
