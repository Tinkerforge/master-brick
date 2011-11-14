/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi_init.c: Chibi initialization
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


#include "chibi_slave.h"
#include "chibi_master.h"
#include "extensions/extension_init.h"

extern uint8_t chibi_address;
extern uint8_t chibi_receiver_address;

void chibi_init_masterslave(uint8_t extension) {
	chibi_address = extension_get_address(extension);
	// TODO: More than one receiver
	chibi_receiver_address = extension_get_receiver_address(0, extension);

	// TODO: initialize chibi pins according to extension

	if(usb_is_connected()) {
		chibi_master_init();
	} else {
		chibi_slave_init();
	}

}
