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

extern uint32_t led_rxtx;

// Recv and send buffer for RS485
uint8_t rs485_buffer_recv[RS485_BUFFER_SIZE] = {0};
uint8_t rs485_buffer_send[RS485_BUFFER_SIZE] = {0};

// Recv and send buffer size for RS485
uint16_t rs485_buffer_size_send = 0;
uint16_t rs485_buffer_size_recv = 0;

uint8_t rs485_mode = RS485_MODE_NONE;
uint8_t rs485_type = RS485_TYPE_NONE;

uint8_t rs485_slave_address[RS485_NUM_SLAVE_ADDRESS] = {0};

uint8_t rs485_state = 0;
uint8_t rs485_id = 1;
uint16_t rs485_recv_length = 0;
uint16_t rs485_to_recv = 0;

uint8_t rs485_checksum = 0;

bool rs485_init(void) {
    const Pin pins_rs485[] = PINS_RS485;
    PIO_Configure(pins_rs485, PIO_LISTSIZE(pins_rs485));

    uint32_t mode = US_MR_USART_MODE_RS485 |
                    US_MR_USCLKS_MCK       |
                    US_MR_CHRL_8_BIT       |
                    US_MR_PAR_NO           |
                    US_MR_NBSTOP_1_BIT     |
                    US_MR_CHMODE_NORMAL    |
                    US_MR_SYNC;

    PMC->PMC_PCER0 = 1 << ID_RS485;
    USART_Configure(USART_RS485, mode, BAUDRATE_RS485, BOARD_MCK);
    USART1->US_IDR = 0xFFFFFFFF;
    NVIC_EnableIRQ(IRQ_RS485);
    USART_SetTransmitterEnabled(USART1, 1);

    return true;
}

uint16_t rs485_send(const void *data, const uint16_t length) {
	if(rs485_buffer_size_send > 0) {
		return 0;
	}
	led_rxtx++;

	uint16_t send_length = MIN(length, RS485_BUFFER_SIZE);

	memcpy(rs485_buffer_send, data, send_length);
	rs485_buffer_size_send = send_length;

	return send_length;
}

uint16_t rs485_recv(void *data, const uint16_t length) {
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

void rs485_set_mode_receive(void) {
    Pin pin_rs485_recv = PIN_RS485_RECV;
    PIO_Configure(&pin_rs485_recv, 1);
    USART_SetReceiverEnabled(USART1, 1);
    USART_GetStatus(USART1);

    USART1->US_IER = US_IER_RXRDY;
    NVIC_EnableIRQ(USART1_IRQn);

    rs485_mode = RS485_MODE_RECEIVE;
}


void rs485_set_mode_send(void) {
    Pin pin_rs485_recv = PIN_RS485_RECV;
    pin_rs485_recv.type = PIO_OUTPUT_1;
    PIO_Configure(&pin_rs485_recv, 1);
    USART_SetReceiverEnabled(USART1, 0);

    rs485_mode = RS485_MODE_SEND;
}

void USART1_IrqHandler() {
	led_on(LED_EXT_BLUE_0);
    uint32_t status = USART_GetStatus(USART1);
    if((status & US_CSR_RXRDY) == US_CSR_RXRDY) {
    	switch(rs485_state) {
			case 0:
			case 1:
			case 2: {
				led_on(LED_EXT_BLUE_1);
				if(USART1->US_RHR == 0xFF) {
					rs485_state++;
				}
				break;
			}

			case 3: {
				rs485_checksum = 0;
				uint8_t address = USART1->US_RHR;
				PEARSON(rs485_checksum, address);
				if((address & (~(1 << 7))) == rs485_id) {
					if(address & (1 << 7)) {
						rs485_set_mode_send();
						rs485_state = 0;
					} else {
						led_on(LED_EXT_BLUE_2);
						rs485_state++;
					}
				} else {
					rs485_state = 0;
				}
				break;
			}

			case 4:
			case 5:
			case 6: {
				rs485_buffer_recv[rs485_state - 4] = USART1->US_RHR;
				PEARSON(rs485_checksum, rs485_buffer_recv[rs485_state - 4]);
				rs485_state++;
				break;
			}

			case 7: {
				rs485_buffer_recv[rs485_state - 4] = USART1->US_RHR;
				PEARSON(rs485_checksum, rs485_buffer_recv[rs485_state - 4]);
				rs485_state++;

				rs485_recv_length = *((uint16_t*)&rs485_buffer_recv[2]);
				if(rs485_recv_length == 5) {
					led_on(LED_EXT_BLUE_3);
				}
				rs485_to_recv = rs485_recv_length - 4;
				if(rs485_to_recv == 0) {
					rs485_state = 0;
					rs485_set_mode_send();
				}
				break;
			}

			case 8: {
				rs485_buffer_recv[rs485_recv_length - rs485_to_recv] = USART1->US_RHR;
				PEARSON(rs485_checksum,
				        rs485_buffer_recv[rs485_recv_length - rs485_to_recv]);
				rs485_to_recv--;
				if(rs485_to_recv == 0) {
					rs485_state++;
				}
				break;
			}

			case 9: {
				uint8_t checksum = USART1->US_RHR;
				rs485_state = 0;
				rs485_buffer_size_recv = rs485_recv_length;
				rs485_set_mode_send();
			}
    	}
    }
}
