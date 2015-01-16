/* master-brick
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_w5200.c: W5200 specific implementation
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

#include "ethernet_w5200.h"
#include "ethernet_low_level.h"

uint8_t ethernet_w5200_read_buffer(const uint32_t address,
                                   uint8_t *buffer,
                                   const uint16_t length) {
	ethernet_select();

	ethernet_transceive_byte((address & 0xFF0000) >> 16);
	ethernet_transceive_byte((address & 0x00FF00) >> 8);

	ethernet_transceive_byte(ETHERNET_W5200_READ_COMMAND | ((length & 0x7F00) >> 8));
	ethernet_transceive_byte(length & 0x00FF);

	for(uint16_t i = 0; i < length; i++) {
		buffer[i] = ethernet_transceive_byte(0);
	}

	ethernet_deselect();

	return length;
}

uint8_t ethernet_w5200_write_buffer(const uint32_t address,
                                    const uint8_t *buffer,
                                    const uint16_t length) {
	ethernet_select();

	ethernet_transceive_byte((address & 0xFF0000) >> 16);
	ethernet_transceive_byte((address & 0x00FF00) >> 8);

	ethernet_transceive_byte(ETHERNET_W5200_WRITE_COMMAND | ((length & 0x7F00) >> 8));
	ethernet_transceive_byte(length & 0x00FF);

	for(uint16_t i = 0; i < length; i++) {
		ethernet_transceive_byte(buffer[i]);
	}

	ethernet_deselect();

	return length;
}
