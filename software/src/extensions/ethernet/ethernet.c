/* master-brick
 * Copyright (C) 2012-2014 Olaf Lüke <olaf@tinkerforge.com>
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

#include "ethernet_websocket.h"
#include "ethernet_config.h"
#include "ethernet_low_level.h"
#include "ethernet_dhcp.h"

#include "extensions/brickd.h"
#include "extensions/extension_i2c.h"

#include "bricklib/com/com.h"
#include "bricklib/com/com_common.h"
#include "bricklib/bricklet/bricklet_config.h"
#include "bricklib/utility/init.h"
#include "bricklib/utility/util_definitions.h"

#include "bricklib/drivers/usart/usart.h"

extern Pin extension_pins[];
extern ComInfo com_info;
extern uint8_t ethernet_plain_sockets;

uint8_t ETHERNET_CS = ETHERNET_CS_0;
uint8_t ETHERNET_RESET = ETHERNET_RESET_0;
uint8_t ETHERNET_INT = ETHERNET_INT_0;
uint8_t ETHERNET_PWDN = ETHERNET_PWDN_0;

bool ethernet_dhcp_server = true;

EthernetStatus ethernet_status = ETHERNET_STATUS_DEFAULT;

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

	// Enable authentication if key is set
	char secret = '\0';
	ethernet_read_config(&secret, 1, ETHERNET_AUTHENTICATION_SECRET_POS);

	logethi("First secret char: %x\n\r", secret);
	if(secret == '\0') {
		brickd_disable_authentication();
	} else {
		brickd_enable_authentication();
	}

	brickd_init();

	xTaskCreate(ethernet_message_loop,
				(signed char *)"eth_ml",
				MESSAGE_LOOP_SIZE,
				NULL,
				1,
				(xTaskHandle *)NULL);

	logethi("Ethernet initialized\n\r");

	return true;
}

uint16_t ethernet_send(const void *data, const uint16_t length, uint32_t *options) {
	const int8_t socket = brickd_route_to((const uint8_t*)data);

	if(socket == -1) {
		for(uint8_t socket_i = 0; socket_i < ETHERNET_MAX_SOCKETS; socket_i++) {
			if(brickd_check_auth(data, socket_i)) {
				if(socket_i < ethernet_plain_sockets) {
					ethernet_low_level_write_data_tcp(socket_i, data, length);
				} else {
					ethernet_websocket_write_data_tcp(socket_i, data, length);
				}
			}
		}

		return length;
	}

	if(!brickd_check_auth(data, socket)) {
		return length;
	}

	if(socket < ethernet_plain_sockets) {
		return ethernet_low_level_write_data_tcp(socket, data, length);
	} else {
		return ethernet_websocket_write_data_tcp(socket, data, length);
	}
}

uint16_t ethernet_recv(void *data, const uint16_t length, uint32_t *options) {
	static uint8_t socket = 0;

	if(length < 8) {
		return 0;
	}

	if(socket >= ETHERNET_MAX_SOCKETS) {
		socket = 0;
	}

	uint8_t read_length = 0;
	read_data_tcp_t read_data_tcp = ethernet_low_level_read_data_tcp;
	for(; socket < ETHERNET_MAX_SOCKETS; socket++) {
		if(socket < ethernet_plain_sockets) {
			read_data_tcp = ethernet_low_level_read_data_tcp;
		} else {
			read_data_tcp = ethernet_websocket_read_data_tcp;
		}

		read_length = read_data_tcp(socket, data, 8);
		if(read_length != 0) {
			while(read_length < 8) {
				read_length += read_data_tcp(socket,
			                                 ((uint8_t*)data) + read_length,
			                                 8 - read_length);
			}
			break;
		}
	}

	if(read_length != 0) {
		uint8_t to_read = ((MessageHeader*)data)->length;
		if(to_read > 80) {
			logethe("Message length not in range: %d\n\r", to_read);
			ethernet_low_level_emergency_disconnect(socket);
			return 0;
		}
		if(to_read > length) {
			logethe("Buffer too small: %d > %d\n\r", to_read, length);
			ethernet_low_level_emergency_disconnect(socket);
			return 0;
		}

		while(read_length < to_read) {
			read_length += read_data_tcp(socket,
			                             ((uint8_t*)data) + read_length,
			                             to_read - read_length);
		}

		if(!brickd_check_auth(data, socket)) {
			return 0;
		}

		brickd_route_from((const uint8_t*)data, socket);
		socket++;
	}


	return read_length;
}

void ethernet_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = ETHERNET_MAX_DATA_LENGTH;
	mlp.com_type    = COM_ETHERNET;
	mlp.return_func = ethernet_message_loop_return;
	com_message_loop(&mlp);
}

void ethernet_message_loop_return(const char *data, const uint16_t length) {
	com_route_message_from_pc(data, length, COM_ETHERNET);
}

void ethernet_tick(const uint8_t tick_type) {
	static uint8_t ethernet_counter = 0;

	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		return;
	}

	ethernet_counter++;

	if(ethernet_dhcp_server) {
		dhcp_tick(tick_type);
		if(ethernet_counter >= 10) {
			dhcp_check_state(DHCP_SOCKET);
		}
	}

	if(ethernet_counter >= 10) {
		ethernet_counter = 0;
		for(uint8_t socket = 0; socket < ETHERNET_MAX_SOCKETS; socket++) {
			uint16_t status = ethernet_low_level_get_status(socket);
			switch(status) {
				case ETH_VAL_SN_SR_SOCK_CLOSE_WAIT: {
					ethernet_low_level_disconnect(socket);
					brickd_disconnect(socket);
					break;
				}

				case ETH_VAL_SN_SR_SOCK_CLOSED: {
					ethernet_low_level_socket_init(socket);
					ethernet_low_level_socket_listen(socket);
					brickd_disconnect(socket);

					break;
				}
			}
		}
	}
}


// TODO: Centralize WIFI and Ethernet read/write key/config
uint32_t ethernet_read_key(void) {
	uint8_t extension;
	if(com_info.ext[0] == COM_ETHERNET) {
		extension = 0;
	} else if(com_info.ext[1] == COM_ETHERNET) {
		extension = 1;
	} else {
		// TODO: Error?
		return 0;
	}

	uint32_t key = 0;
	extension_i2c_read(extension,
					   ETHERNET_KEY_POS,
					   (char*)&key,
					   4);
	return key;
}

void ethernet_write_key(void) {
	uint8_t extension;
	if(com_info.ext[0] == COM_ETHERNET) {
		extension = 0;
	} else if(com_info.ext[1] == COM_ETHERNET) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	uint32_t key = ETHERNET_KEY;
	extension_i2c_write(extension,
	                    ETHERNET_KEY_POS,
					    (char*)&key,
					    4);
}

bool ethernet_read_config(char *data, const uint8_t length, const uint16_t position) {
	uint8_t extension;
	if(com_info.ext[0] == COM_ETHERNET) {
		extension = 0;
	} else if(com_info.ext[1] == COM_ETHERNET) {
		extension = 1;
	} else {
		// TODO: Error?
		return false;
	}

	if(ethernet_read_key() != ETHERNET_KEY) {
		logethw("Key mismatch\n\r");
		ethernet_write_config(data, length, position);
		return false;
	}

	uint8_t i;
	for(i = 0; i < length/32; i++) {
		extension_i2c_read(extension,
						   position + i*32,
						   data + i*32,
						   32);
	}

	uint8_t reminder = length - i*32;
	if(reminder != 0) {
		extension_i2c_read(extension,
						   position + i*32,
						   data + i*32,
						   reminder);
	}

	return true;
}

void ethernet_write_config(const char *data, const uint8_t length, const uint16_t position) {
	uint8_t extension;
	if(com_info.ext[0] == COM_ETHERNET) {
		extension = 0;
	} else if(com_info.ext[1] == COM_ETHERNET) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	uint8_t reminder = 32 - (position % 32);
	if(reminder != 32) {
		reminder = MIN(length, reminder);
		extension_i2c_write(extension,
						    position,
						    data,
						    reminder);
	} else {
		reminder = 0;
	}

	uint8_t i = 0;
	for(i = 0; i < (length - reminder)/32; i++) {
		extension_i2c_write(extension,
						    position + reminder + i*32,
						    data + reminder + i*32,
						    32);
	}

	uint8_t last = (length - reminder) - i*32;
	if(last != 0) {
		extension_i2c_write(extension,
						    position + reminder + i*32,
						    data + reminder + i*32,
						    last);
	}

	ethernet_write_key();
}
