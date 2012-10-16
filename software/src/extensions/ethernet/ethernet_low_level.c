/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet_low_level.c: Low level Ethernet protocol implementation
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

#include "ethernet_low_level.h"

#include "config.h"

#include "ethernet_config.h"

#include "bricklib/drivers/pio/pio.h"
#include "bricklib/utility/util_definitions.h"

extern Pin extension_pins[];
extern uint8_t ETHERNET_CS;
extern uint8_t ETHERNET_RESET;
extern uint8_t ETHERNET_INT;
extern uint8_t ETHERNET_PWDN;

void ethernet_low_level_reset(void) {
	PIO_Clear(&extension_pins[ETHERNET_RESET]);
	SLEEP_US(2);
	PIO_Set(&extension_pins[ETHERNET_RESET]);
	SLEEP_MS(150);
}

void ethernet_low_level_init(void) {

}

void ethernet_select(void) {
	__disable_irq();
	PIO_Clear(&extension_pins[ETHERNET_CS]);
}

void ethernet_deselect(void) {
	PIO_Set(&extension_pins[ETHERNET_CS]);
	__enable_irq();
}

uint8_t ethernet_transceive_byte(const uint8_t value) {
	// Wait for transfer buffer to be empty
	while((USART0->US_CSR & US_CSR_TXEMPTY) == 0);
	USART0->US_THR = value;

	// Wait until receive buffer has new data
	while((USART0->US_CSR & US_CSR_RXRDY) == 0);
	return USART0->US_RHR;
}

uint8_t ethernet_read_register(const uint16_t address) {
	uint8_t value;
	ethernet_read_buffer(address, &value, 1);

	return value;
}

void ethernet_write_register(const uint16_t address, const uint8_t value) {
	ethernet_write_buffer(address, &value, 1);
}

uint8_t ethernet_read_buffer(const uint16_t address,
                             uint8_t *buffer,
                             const uint8_t length) {
	ethernet_select();
	SLEEP_MS(1);

	ethernet_transceive_byte((address & 0xFF00) >> 8);
	ethernet_transceive_byte(address & 0x00FF);

	ethernet_transceive_byte(ETHERNET_READ_COMMAND | ((length & 0x7F00) >> 8));
	ethernet_transceive_byte(length & 0x00FF);

	for(uint8_t i = 0; i < length; i++) {
		buffer[i] = ethernet_transceive_byte(0);
	}

	SLEEP_MS(1);
	ethernet_deselect();

	return length;
}

uint8_t ethernet_write_buffer(const uint16_t address,
                              const uint8_t *buffer,
                              const uint8_t length) {

	ethernet_select();

	ethernet_transceive_byte((address & 0xFF00) >> 8);
	ethernet_transceive_byte(address & 0x00FF);

	ethernet_transceive_byte(ETHERNET_WRITE_COMMAND | ((length & 0x7F00) >> 8));
	ethernet_transceive_byte(length & 0x00FF);

	for(uint8_t i = 0; i < length; i++) {
		ethernet_transceive_byte(buffer[i]);
	}

	ethernet_deselect();

	return length;
}
