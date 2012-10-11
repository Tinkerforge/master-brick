/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi_low_level.c: Low level WIFI protocol implementation
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

#include "wifi_low_level.h"

#include <stddef.h>

#include "config.h"
#include "wifi_config.h"

#include "bricklib/drivers/pio/pio.h"
#include "bricklib/utility/util_definitions.h"

extern Pin extension_pins[];
extern uint8_t WIFI_CS;

bool wifi_low_level_is_byte_stuffing(char value) {
	return ((WIFI_LOW_LEVEL_SPI_ESC_CHAR	          == value) ||
		    (WIFI_LOW_LEVEL_SPI_XON_CHAR	          == value) ||
		    (WIFI_LOW_LEVEL_SPI_XOFF_CHAR             == value) ||
	        (WIFI_LOW_LEVEL_SPI_IDLE_CHAR             == value) ||
	        (WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ONE  == value) ||
	        (WIFI_LOW_LEVEL_SPI_INVALID_CHAR_ALL_ZERO == value) ||
	        (WIFI_LOW_LEVEL_SPI_LINK_READY            == value));
}

uint8_t wifi_low_level_write_byte_stuffing(char b, char *ret1, char *ret2) {
	bool is_stuff = false;
	if(wifi_low_level_is_byte_stuffing(b)) {
		is_stuff = true;
		wifi_low_level_select();

		while((USART_WIFI_SPI->US_CSR & US_CSR_TXEMPTY) == 0);
		USART_WIFI_SPI->US_THR = WIFI_LOW_LEVEL_SPI_ESC_CHAR;

		while((USART_WIFI_SPI->US_CSR & US_CSR_RXRDY) == 0);
		char ret = USART_WIFI_SPI->US_RHR;
		if(ret1 != NULL) {
			*ret1 = ret;
		}

		wifi_low_level_deselect();

		b = b^0x20;
	}

	wifi_low_level_select();

	while((USART_WIFI_SPI->US_CSR & US_CSR_TXEMPTY) == 0);
	USART_WIFI_SPI->US_THR = b;

	while((USART_WIFI_SPI->US_CSR & US_CSR_RXRDY) == 0);
	char ret = USART_WIFI_SPI->US_RHR;

	wifi_low_level_deselect();

	if(is_stuff) {
		if(ret2 != NULL) {
			*ret2 = ret;
		}
		return 2;
	}

	if(ret1 != NULL) {
		*ret1 = ret;
	}
	return 1;
}

char wifi_low_level_write_byte(char b) {
	wifi_low_level_select();

	while((USART_WIFI_SPI->US_CSR & US_CSR_TXEMPTY) == 0);
	USART_WIFI_SPI->US_THR = b;

	while((USART_WIFI_SPI->US_CSR & US_CSR_RXRDY) == 0);
	char ret = USART_WIFI_SPI->US_RHR;

	wifi_low_level_deselect();

	return ret;
}

char wifi_low_level_read_byte(void) {
	return wifi_low_level_write_byte(WIFI_LOW_LEVEL_SPI_IDLE_CHAR);
}

void wifi_low_level_read_buffer(char *buffer, const uint8_t length) {
	for(uint8_t i = 0; i < length; i++) {
		buffer[i] = wifi_low_level_read_byte();
	}
}

void wifi_low_level_write_buffer(const char *buffer, const uint8_t length) {
	for(uint8_t i = 0; i < length; i++) {
		wifi_low_level_write_byte_stuffing(buffer[i], NULL, NULL);
	}
}

void wifi_low_level_select(void) {
	SLEEP_NS(250);
	PIO_Clear(&extension_pins[WIFI_CS]);
	SLEEP_NS(250);
}

void wifi_low_level_deselect(void) {
	SLEEP_NS(250);
	PIO_Set(&extension_pins[WIFI_CS]);
	SLEEP_NS(250);
}
