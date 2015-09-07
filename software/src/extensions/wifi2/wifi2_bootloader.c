/* master-brick
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi2_bootloader.c: WIFI2 bootloader communication with ESP8266
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

#include "wifi2_bootloader.h"

#include "config.h"
#include "wifi2_config.h"
#include "wifi2_uart.h"

#include "bricklib/drivers/usart/usart.h"
#include "bricklib/drivers/pio/pio.h"

#include "bricklib/utility/util_definitions.h"

extern Pin extension_pins[];
extern Wifi2 *w2;

int8_t wifi2_bootloader_start(void) {
	logwifi2d("WIFI2 bootloader start\n\r");
    const uint32_t mode = US_MR_USCLKS_MCK       |
                          US_MR_CHRL_8_BIT       |
                          US_MR_CHMODE_NORMAL    |
                          US_MR_NBSTOP_1_BIT     |
                          US_MR_PAR_NO;

    PMC->PMC_PCER0 = 1 << ID_WIFI2;
    USART_Configure(USART_WIFI2, mode, 115200, BOARD_MCK);
    USART_WIFI2->US_IDR = 0xFFFFFFFF;
    NVIC_EnableIRQ(IRQ_WIFI2);

    USART_SetReceiverEnabled(USART_WIFI2, 1);
    USART_SetTransmitterEnabled(USART_WIFI2, 1);

    USART_WIFI2->US_RPR = (uint32_t)w2->recv_dma_buffer[0];
    USART_WIFI2->US_RCR = WIFI2_UART_BUFFER_SIZE;
    USART_WIFI2->US_RNPR = (uint32_t)w2->recv_dma_buffer[1];
    USART_WIFI2->US_RNCR = WIFI2_UART_BUFFER_SIZE;
    USART_WIFI2->US_PTCR = US_PTCR_RXTEN;

	Pin pio15 = PIN_WIFI2_RXD;
	pio15.type = PIO_OUTPUT_0;
	PIO_Configure(&pio15, 1);

	SLEEP_MS(200);

	extension_pins[w2->enable].type      = PIO_OUTPUT_1;
	extension_pins[w2->enable].attribute = PIO_DEFAULT;
	extension_pins[w2->nreset].type      = PIO_OUTPUT_0;
	extension_pins[w2->nreset].attribute = PIO_DEFAULT;
	extension_pins[w2->flashc].type      = PIO_OUTPUT_1;
	extension_pins[w2->flashc].attribute = PIO_DEFAULT;
	PIO_Configure(&extension_pins[w2->nreset], 1);
	PIO_Configure(&extension_pins[w2->enable], 1);
	PIO_Configure(&extension_pins[w2->flashc], 1);

	// TODO: Pull IO0 low to bring ESP8266 in bootloader!


	SLEEP_MS(200);
	extension_pins[w2->nreset].type = PIO_OUTPUT_1;
	PIO_Configure(&extension_pins[w2->nreset], 1);
	SLEEP_MS(200);

    const Pin pins_wifi2[] = PINS_WIFI2;
	PIO_Configure(pins_wifi2, PIO_LISTSIZE(pins_wifi2));

	return 0;
}

int8_t wifi2_bootloader_write(uint8_t *data, uint8_t length) {
	USART_WIFI2->US_TPR  = (uint32_t)data;
	USART_WIFI2->US_TCR  = length;
	USART_WIFI2->US_PTCR = US_PTCR_TXTEN;

	while(!(USART_WIFI2->US_CSR & US_CSR_TXBUFE)) {
		logwifi2d("wifi2_bootloader_write waiting...\n\r");
		__NOP();
	}
	return 0;
}

uint8_t wifi2_bootloader_read(uint8_t *data, uint8_t length) {
	uint8_t i;
	for(i = 0; i < length; i++) {
		if(!wifi2_uart_rx(&data[i])) {
			return i;
		}
	}

	return length;
}
