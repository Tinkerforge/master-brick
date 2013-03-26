/* master-brick
 * Copyright (C) 2013 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_ringbuffer.c: WIFI ringbuffer implementation
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

#include "wifi_ringbuffer.h"

#include "wifi_low_level.h"
#include "wifi_config.h"

#include <stdbool.h>
#include "bricklib/logging/logging.h"

uint32_t wifi_ringbuffer_overflow = 0;
uint16_t wifi_ringbuffer_low_watermark = WIFI_RINGBUFFER_SIZE;
uint16_t wifi_ringbuffer_start = 0;
uint16_t wifi_ringbuffer_end = 0;
char wifi_ringbuffer[WIFI_RINGBUFFER_SIZE] = {0};

uint16_t wifi_ringbuffer_get_free(void) {
	uint16_t diff;
	if(wifi_ringbuffer_end < wifi_ringbuffer_start) {
		diff = WIFI_RINGBUFFER_SIZE + wifi_ringbuffer_end - wifi_ringbuffer_start;
	} else {
		diff = wifi_ringbuffer_end - wifi_ringbuffer_start;
	}

	const uint16_t free = WIFI_RINGBUFFER_SIZE - diff;

	if(free < wifi_ringbuffer_low_watermark) {
		wifi_ringbuffer_low_watermark = free;
	}

	return free;
}

bool wifi_ringbuffer_is_empty(void) {
	return wifi_ringbuffer_start == wifi_ringbuffer_end;
}

bool wifi_ringbuffer_is_full(void) {
	return wifi_ringbuffer_get_free() < 2;
}

bool wifi_ringbuffer_add(const char data) {
	if(wifi_ringbuffer_is_full()) {
		logwifie("Add: Ringbuffer overflow\n\r");
		wifi_ringbuffer_overflow++;
		return false;
	}

	// Don't add stuffing bytes to ringbuffer
	if(!wifi_ringbuffer_is_empty()) {
		if(wifi_low_level_is_byte_stuffing(data)) {
			uint16_t last_index = wifi_ringbuffer_end-1;
			if(wifi_ringbuffer_end == 0) {
				last_index = WIFI_RINGBUFFER_SIZE-1;
			}

			if((data != WIFI_LOW_LEVEL_SPI_ESC_CHAR) &&
			   (wifi_ringbuffer[last_index] != WIFI_LOW_LEVEL_SPI_ESC_CHAR)) {
				return true;
			}
		}
	}

	wifi_ringbuffer[wifi_ringbuffer_end] = data;
	wifi_ringbuffer_end++;
	if(wifi_ringbuffer_end >= WIFI_RINGBUFFER_SIZE) {
		wifi_ringbuffer_end = 0;
	}
	return true;
}

bool wifi_ringbuffer_get(char *data) {
	if(wifi_ringbuffer_is_empty()) {
		logwifie("Get: Ringbuffer empty\n\r");
		return false;
	}
	*data = wifi_ringbuffer[wifi_ringbuffer_start];
	wifi_ringbuffer_start++;
	if(wifi_ringbuffer_start >= WIFI_RINGBUFFER_SIZE) {
		wifi_ringbuffer_start = 0;
	}
	return true;
}

void wifi_ringbuffer_print(void) {
	uint16_t end = wifi_ringbuffer_end;
	if(wifi_ringbuffer_start > end) {
		end += WIFI_RINGBUFFER_SIZE;
	}

	logwifid("Ringbuffer (%d): [", WIFI_RINGBUFFER_SIZE - wifi_ringbuffer_get_free());
	for(uint16_t i = wifi_ringbuffer_start; i < end; i++) {
		logwohd("%d, ", wifi_ringbuffer[i]);
	}
	logwohd("]\n\r");
}
