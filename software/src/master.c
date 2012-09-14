/* master-brick
 * Copyright (C) 2010-2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * master.c: Master specific functions
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

#include "master.h"

#include <stdio.h>

#include "bricklib/bricklet/bricklet_init.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/com/spi/spi_stack/spi_stack_master.h"
#include "bricklib/com/spi/spi_stack/spi_stack_common.h"
#include "bricklib/com/spi/spi_stack/spi_stack_select.h"
#include "bricklib/drivers/adc/adc.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/logging/logging.h"
#include "bricklib/utility/util_definitions.h"

#include "extensions/extension_i2c.h"
#include "extensions/chibi/chibi_master.h"
#include "extensions/chibi/chibi_slave.h"
#include "extensions/chibi/chibi_low_level.h"
#include "extensions/rs485/rs485_config.h"
#include "extensions/rs485/rs485_master.h"
#include "extensions/rs485/rs485_low_level.h"
#include "extensions/wifi/wifi.h"

#include "config.h"

extern uint8_t com_stack_id;
extern uint8_t com_last_stack_address;
extern uint8_t com_last_ext_id[];
extern ComType com_ext[];
extern uint16_t spi_stack_buffer_size_send;
extern uint8_t com_last_spi_stack_id;

uint8_t master_routing_table[MAX_STACK_IDS] = {0};
uint8_t master_mode = MASTER_MODE_NONE;

extern uint8_t chibi_address;
extern uint8_t chibi_slave_address[];
extern uint8_t chibi_type;

extern uint8_t rs485_slave_address[];
extern uint8_t rs485_address;
extern uint8_t rs485_type;
extern uint8_t rs485_mode;

extern uint16_t chibi_wait_for_recv;
extern uint16_t spi_stack_buffer_size_recv;
extern uint8_t rs485_buffer_size_send;
extern uint8_t rs485_last_sequence_number;

extern xTaskHandle rs485_handle_master_message_loop;
extern xTaskHandle rs485_handle_master_state_machine_loop;

bool chibi_enumerate_ready = false;

void master_init(void) {
	Pin power_pins[] = {PIN_STACK_VOLTAGE, PIN_STACK_CURRENT};
	PIO_Configure(power_pins, PIO_LISTSIZE(power_pins));

	adc_channel_enable(STACK_VOLTAGE_CHANNEL);
	adc_channel_enable(STACK_CURRENT_CHANNEL);

	extension_i2c_clear_eeproms();
}

void master_create_routing_table_rs485(uint8_t extension) {
	logrsi("Start routing table creation\n\r");
	com_last_ext_id[extension] = com_last_spi_stack_id;

	for(int8_t i = 0; i < RS485_NUM_SLAVE_ADDRESS; i++) {
		uint8_t slave_address = rs485_slave_address[i];
		if(slave_address == 0) {
			if(rs485_slave_address[0] == 0) {
				logrsi("Couldn't find any bus participants\n\r");
				vTaskDelete(rs485_handle_master_message_loop);
				vTaskDelete(rs485_handle_master_state_machine_loop);
			}
			rs485_buffer_size_send = 0;
			return;
		}

		uint32_t tries = 0;

		StackEnumerate se = {
			0,
			TYPE_STACK_ENUMERATE,
			sizeof(StackEnumerate),
			com_last_ext_id[extension] + 1
		};

		tries = 0;
		master_routing_table[0] = slave_address;

		uint8_t sequence_number = 1;
		USART_DisableIt(USART_RS485, US_IER_RXRDY);
		rs485_send(&se, sizeof(StackEnumerate));
		rs485_low_level_set_mode_send_from_task();
		rs485_address = slave_address;
		rs485_low_level_send(slave_address, sequence_number, false);

		master_routing_table[0] = 0;

		uint8_t data[64];
		StackEnumerateReturn *ser = (StackEnumerateReturn*)data;

		tries = 0;
		uint8_t tries_resend = 0;

		uint16_t s = rs485_wait_time()/10;
		uint16_t max_tries = 200;
		if(max_tries*s > 1000) {
			max_tries = 1000/s;
		}

		while(tries < max_tries) {
			if(s < 3) {
				SLEEP_MS(3);
			} else {
				SLEEP_MS(s);
			}
			if(rs485_recv(ser, 64)) {
				if(ser->type == TYPE_STACK_ENUMERATE) {
					break;
				}
				rs485_low_level_set_mode_send_from_task();
			}
			tries++;

			if(rs485_mode == RS485_MODE_SEND) {
				rs485_low_level_send(slave_address, rs485_last_sequence_number+1, false);
			} else {
				tries_resend++;
				if(tries_resend == 5) {
					USART_DisableIt(USART_RS485, US_IER_RXRDY);
					rs485_send(&se, sizeof(StackEnumerate));
					rs485_low_level_set_mode_send_from_task();
					rs485_address = slave_address;
					rs485_low_level_send(slave_address, ++sequence_number, false);
					tries_resend = 0;
				}
			}
		}

		if(tries == max_tries) {
			for(uint8_t j = i; j < RS485_NUM_SLAVE_ADDRESS-1; j++) {
				rs485_slave_address[j] = rs485_slave_address[j+1];
			}
			rs485_slave_address[RS485_NUM_SLAVE_ADDRESS-1] = 0;

			i--;

			logrsi("Reached 200 tries, couldn't find %d\n\r", slave_address);
			continue;
		}

		for(uint8_t i = com_last_ext_id[extension] + 1; i <= ser->stack_id_upto; i++) {
			master_routing_table[i] = slave_address;
		}

		com_last_ext_id[extension] = ser->stack_id_upto;
		logrsi("last ext id %d for slave/ext %d/%d\n\r", ser->stack_id_upto, slave_address, extension);
	}
}

void master_create_routing_table_chibi(uint8_t extension) {
	com_last_ext_id[extension] = com_last_spi_stack_id;

	for(int8_t i = 0; i < CHIBI_NUM_SLAVE_ADDRESS; i++) {
		uint8_t slave_address = chibi_slave_address[i];
		if(slave_address == 0) {
			return;
		}

		uint32_t tries = 0;

		StackEnumerate se = {
			0,
			TYPE_STACK_ENUMERATE,
			sizeof(StackEnumerate),
			com_last_ext_id[extension] + 1
		};

		tries = 0;
		master_routing_table[0] = slave_address;
		while(!chibi_send(&se, sizeof(StackEnumerate)) && tries < 10) {
			SLEEP_MS(10);
			tries++;
		}

		chibi_enumerate_ready = true;
		master_routing_table[0] = 0;

		if(tries == 10) {
			logspisw("Could not send Stack Enumerate message (chibi)\n\r");
			continue;
		}

		uint8_t data[64];
		StackEnumerateReturn *ser = (StackEnumerateReturn*)data;

		tries = 0;
		while(tries < 2000) {
			SLEEP_US(10);
			if(chibi_recv(ser, 64)) {
				if(ser->type == TYPE_STACK_ENUMERATE) {
					break;
				}
			}
			tries++;
		}

		if(tries == 2000) {
			for(uint8_t j = i; j < CHIBI_NUM_SLAVE_ADDRESS-1; j++) {
				chibi_slave_address[j] = chibi_slave_address[j+1];
			}
			chibi_slave_address[CHIBI_NUM_SLAVE_ADDRESS-1] = 0;

			i--;

			logspisw("Did not receive answer for Stack Enumerate (chibi)\n\r");
			continue;
		}

		for(uint8_t i = com_last_ext_id[extension] + 1; i <= ser->stack_id_upto; i++) {
			master_routing_table[i] = slave_address;
		}

		com_last_ext_id[extension] = ser->stack_id_upto;

		logchibii("last ext id %d for slave/ext %d/%d\n\r", ser->stack_id_upto, slave_address, extension);
	}
}

void master_create_routing_table_extensions(void) {
	for(uint8_t i = 0; i < 2; i++) {
		switch(com_ext[i]) {
			case COM_CHIBI: {
				if(chibi_type == CHIBI_TYPE_MASTER) {
					master_create_routing_table_chibi(i);
				}

				break;
			}

			case COM_RS485: {
				if(rs485_type == RS485_TYPE_MASTER) {
					master_create_routing_table_rs485(i);
				}

				break;
			}

			case COM_NONE:
			default: {

				break;
			}
		}
	}
}

void master_create_routing_table_stack(void) {
	for(uint16_t i = 0; i < MAX_STACK_IDS; i++) {
		master_routing_table[i] = 0;
	}

	uint8_t stack_address = 0;
	uint8_t tries = 0;
	while(stack_address <= 8) {
		StackEnumerate se = {
			0,
			TYPE_STACK_ENUMERATE,
			sizeof(StackEnumerate),
			com_last_spi_stack_id + 1
		};

		if(spi_stack_send(&se, sizeof(StackEnumerate)) != 0) {
			spi_stack_select(stack_address + 1);

			tries = 0;
			while(!spi_stack_master_transceive() && tries < 10) {
				SLEEP_MS(50);
				tries++;
			}
			if(tries == 10) {
				break;
			}

			spi_stack_deselect();

			spi_stack_buffer_size_recv = 0;
		}

		StackEnumerateReturn ser;
		tries = 0;
		while(tries < 10) {
			SLEEP_MS(50);
			spi_stack_select(stack_address + 1);
			spi_stack_master_transceive();
			spi_stack_deselect();
			if(spi_stack_recv(&ser, sizeof(StackEnumerateReturn))) {
				break;
			}
			tries++;
		}

		if(tries == 10) {
			logspise("Did not receive answer for Stack Enumerate\n\r");
			break;
		}

		logspisi("New Stack participant (from, to): %d, %d\n\r",
		        com_last_spi_stack_id + 1,
		        ser.stack_id_upto);
		stack_address++;
		for(uint8_t i = com_last_spi_stack_id + 1; i <= ser.stack_id_upto; i++) {
			master_routing_table[i] = stack_address;
		}
		com_last_spi_stack_id = ser.stack_id_upto;
	}

	spi_stack_buffer_size_send = 0;
	com_last_stack_address = stack_address;
}

void tick_task(uint8_t tick_type) {
	wifi_tick(tick_type);
}
