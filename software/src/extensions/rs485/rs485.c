/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * rs485.c: High Level RS485 protocol implementation
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


#include "rs485.h"

#include <string.h>

#include "config.h"
#include "rs485_config.h"

#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/led.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/pearson_hash.h"
#include "bricklib/com/usb/usb.h"
#include "extensions/extension_init.h"
#include "extensions/extension_i2c.h"
#include "extensions/rs485/rs485_slave.h"
#include "extensions/rs485/rs485_master.h"

extern uint32_t led_rxtx;

// Recv and send buffer for RS485
uint8_t rs485_buffer_recv[RS485_BUFFER_SIZE] = {0};
uint8_t rs485_buffer_send[RS485_BUFFER_SIZE] = {0};

// Recv and send buffer size for RS485
uint16_t rs485_buffer_size_send = 0;
uint16_t rs485_buffer_size_recv = 0;

uint8_t rs485_mode = RS485_MODE_NONE;
uint8_t rs485_type = RS485_TYPE_NONE;
uint8_t rs485_address = 0;

uint8_t rs485_slave_address[RS485_NUM_SLAVE_ADDRESS] = {0};

RS485Config rs485_config = {
	RS485_BAUDRATE,
	'n',
	1
};

extern uint8_t master_routing_table[];
extern Pin extension_pins[];
uint8_t RS485_RECV = RS485_RECV_0;

uint16_t rs485_wait_time(void) {
	uint16_t t = 4*1000*64*8/9600;
	if(t < 3) {
		return 3;
	}

	return t;
}

void rs485_init_masterslave(uint8_t extension) {
	rs485_address = extension_get_address(extension);
	for(uint8_t i = 0; i < RS485_NUM_SLAVE_ADDRESS; i++) {
		rs485_slave_address[i] = extension_get_slave_address(extension, i);
		logrsi("For slave %d found address %d\n\r", i, rs485_slave_address[i]);
		if(rs485_slave_address[i] == 0) {
			break;
		}
	}

	extension_i2c_read(extension, EXTENSION_POS_ANY, (char*)&rs485_config, 6);
	if(rs485_config.speed < 9600) {
		rs485_config.speed = 9600;
	} else if(rs485_config.speed > 4000000) {
		rs485_config.speed = RS485_BAUDRATE;
	}

	logrsi("config %lu, %c, %d\n\r", rs485_config.speed,
	                                rs485_config.parity,
	                                rs485_config.stopbits);

	if(extension == 0) {
		RS485_RECV = RS485_RECV_0;
	} else if(extension == 1) {
		RS485_RECV = RS485_RECV_1;
	}

    const Pin pins_rs485[] = PINS_RS485;
    PIO_Configure(pins_rs485, PIO_LISTSIZE(pins_rs485));

    extension_pins[RS485_RECV].type = PIO_OUTPUT_0;
    PIO_Configure(&extension_pins[RS485_RECV], 1);

	if(rs485_address == 0) {
		rs485_master_init();
	} else {
		rs485_slave_init();
	}
}

bool rs485_init(void) {
    uint32_t mode = US_MR_USART_MODE_RS485 |
                    US_MR_USCLKS_MCK       |
                    US_MR_CHRL_8_BIT       |
                    US_MR_CHMODE_NORMAL;

    if(rs485_config.stopbits == 2) {
    	mode |= US_MR_NBSTOP_2_BIT;
    } else {
    	mode |= US_MR_NBSTOP_1_BIT;
    }

    if((char)tolower((int)rs485_config.parity) == 'e') {
    	mode |= US_MR_PAR_EVEN;
    } else if((char)tolower((int)rs485_config.parity) == 'o') {
    	mode |= US_MR_PAR_ODD;
    } else {
    	mode |= US_MR_PAR_NO;
    }

    PMC->PMC_PCER0 = 1 << ID_RS485;
    USART_Configure(USART_RS485, mode, rs485_config.speed, BOARD_MCK);
    USART1->US_IDR = 0xFFFFFFFF;
    NVIC_EnableIRQ(IRQ_RS485);

    USART_SetReceiverEnabled(USART_RS485, 1);
    USART_SetTransmitterEnabled(USART_RS485, 1);

    logrsi("RS485 initialized\n\r");
    return true;
}

uint16_t rs485_send(const void *data, const uint16_t length, uint32_t *options) {
	if(rs485_buffer_size_send > 0) {
		return 0;
	}

	led_rxtx++;

	uint16_t send_length = MIN(length, RS485_BUFFER_SIZE);

	memcpy(rs485_buffer_send, data, send_length);
	rs485_buffer_size_send = send_length;

	return send_length;
}

uint16_t rs485_recv(void *data, const uint16_t length, uint32_t *options) {
	if(rs485_buffer_size_recv == 0) {
		return 0;
	}
	led_rxtx++;

	static uint16_t recv_pointer = 0;

	uint16_t recv_length = MIN(length, rs485_buffer_size_recv);

	memcpy(data, rs485_buffer_recv + recv_pointer, recv_length);

	if(rs485_buffer_size_recv - recv_length == 0) {
		recv_pointer = 0;
	} else {
		recv_pointer += recv_length;
	}

	rs485_buffer_size_recv -= recv_length;

	return recv_length;
}

uint8_t rs485_get_receiver_address(uint8_t stack_id) {
	if(rs485_type == RS485_TYPE_MASTER) {
		return master_routing_table[stack_id];
	} else if(rs485_type == RS485_TYPE_SLAVE) {
		return rs485_address;
	}

	return 0;
}
