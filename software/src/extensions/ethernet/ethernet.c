/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ethernet.c: High Level Ethernet protocol implementation
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

#include "ethernet.h"

#include "ethernet_config.h"
#include "ethernet_low_level.h"

#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "bricklib/bricklet/bricklet_config.h"

#include "bricklib/drivers/usart/usart.h"

extern ComType com_ext[];
extern uint8_t com_last_ext_id[];
extern uint32_t led_rxtx;
extern uint32_t led_ext3_rxtx;
extern ComType com_current;

extern uint8_t com_stack_id;
extern uint8_t com_last_spi_stack_id;
extern BrickletSettings bs[];
extern const BrickletAddress baddr[];

extern Pin extension_pins[];
uint8_t ETHERNET_CS = ETHERNET_CS_0;
uint8_t ETHERNET_RESET = ETHERNET_RESET_0;
uint8_t ETHERNET_INT = ETHERNET_INT_0;
uint8_t ETHERNET_PWDN = ETHERNET_PWDN_0;

void ethernet_init_extension(uint8_t extension) {
	if(extension == 0) {
		ETHERNET_CS = ETHERNET_CS_0;
		ETHERNET_RESET = ETHERNET_RESET_0;
		ETHERNET_INT = ETHERNET_INT_0;
		ETHERNET_PWDN = ETHERNET_PWDN_0;
	} else if(extension == 1) {
		ETHERNET_CS = ETHERNET_CS_1;
		ETHERNET_RESET = ETHERNET_RESET_1;
		ETHERNET_INT = ETHERNET_INT_1;
		ETHERNET_PWDN = ETHERNET_PWDN_1;
	}

	ethernet_init();
}

bool ethernet_init(void) {
	extension_pins[ETHERNET_CS].type = PIO_OUTPUT_1;
	extension_pins[ETHERNET_RESET].type = PIO_OUTPUT_1;
    extension_pins[ETHERNET_INT].type = PIO_INPUT;
    extension_pins[ETHERNET_PWDN].type = PIO_OUTPUT_0;
    PIO_Configure(&extension_pins[ETHERNET_CS], 4);

    extension_pins[ETHERNET_MISO].type = PIO_PERIPH_A;
    extension_pins[ETHERNET_MOSI].type = PIO_PERIPH_A;
    extension_pins[ETHERNET_CLK].type = PIO_PERIPH_B;
    PIO_Configure(&extension_pins[ETHERNET_MISO], 3);

	// Enable peripheral clock
	PMC->PMC_PCER0 = 1 << ID_USART0;

	// Configure the USART0 as SPI
	USART_Configure(USART0,
					US_MR_USART_MODE_SPI_MASTER |
					US_MR_USCLKS_MCK            |
					US_MR_CHRL_8_BIT            |
					US_MR_PAR_NO                |
					US_MR_CHMODE_NORMAL         |
					US_MR_CLKO                  |
					US_SPI_CPOL_0               |
					US_SPI_CPHA_1,
					ETHERNET_USART_SPI_CLOCK,
					BOARD_MCK);

	// Enable receiver and transmitter
	USART0->US_CR = US_CR_TXEN;
	USART0->US_CR = US_CR_RXEN;

	ethernet_low_level_init();

	return true;
}

uint16_t ethernet_send(const void *data, const uint16_t length) {
	return 0;
}

uint16_t ethernet_recv(void *data, const uint16_t length) {
	return 0;
}

void ethernet_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = ETHERNET_MAX_DATA_LENGTH;
	mlp.com_type    = COM_ETHERNET;
	mlp.return_func = ethernet_message_loop_return;
	com_message_loop(&mlp);
}

void ethernet_message_loop_return(char *data, uint16_t length) {
	const uint8_t stack_id = get_stack_id_from_data(data);

	if(stack_id == com_stack_id || stack_id == 0) {
		const ComMessage *com_message = get_com_from_data(data);
		if(com_message->reply_func != NULL) {
			com_message->reply_func(COM_ETHERNET, (void*)data);
		}

		return;
	}
	for(uint8_t i = 0; i < BRICKLET_NUM; i++) {
		if(bs[i].stack_id == stack_id) {
			baddr[i].entry(BRICKLET_TYPE_INVOCATION, COM_ETHERNET, (void*)data);
			return;
		}
	}

	if(stack_id <= com_last_spi_stack_id) {
		send_blocking_with_timeout(data, length, COM_SPI_STACK);
		return;
	}

	if(stack_id <= com_last_ext_id[0]) {
		send_blocking_with_timeout(data, length, com_ext[0]);
		return;
	}

	if(stack_id <= com_last_ext_id[1]) {
		send_blocking_with_timeout(data, length, com_ext[1]);
		return;
	}
}
