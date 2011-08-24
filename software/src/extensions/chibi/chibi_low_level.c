/* master-brick
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * chibi_low_level.c: Low-level chibi protocol implementation
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

// The low level implementation of the chibi protocol is based on the
// original implementation from freaklabs.org. It is basically a port
// from 8 bit AVR to 32 bit ARM.

#include "chibi_low_level.h"

#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/drivers/pio/pio_it.h"

#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/led.h"
#include "bricklib/logging/logging.h"
#include "config.h"

#include <FreeRTOS.h>
#include <task.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

extern uint8_t chibi_address;

uint16_t chibi_underrun = 0;
uint16_t chibi_crc_error = 0;
uint16_t chibi_no_ack = 0;
uint16_t chibi_overflow = 0;
uint8_t chibi_receiver_input_power = 0;

uint8_t chibi_type = CHIBI_TYPE_NONE;
uint8_t chibi_buffer_recv[CHIBI_MAX_DATA_LENGTH] = {0};
uint8_t chibi_buffer_size_recv = 0;
uint16_t chibi_wait_for_recv = 0;

bool chibi_transfer_status = CHIBI_ERROR_OK;
bool chibi_transfer_end = true;
Pin chibi_reset_pin = PIN_CHIBI_RESET;
Pin chibi_sleep_pin = PIN_CHIBI_SLP_TR;
Pin chibi_interrupt_pin = PIN_CHIBI_INT;
Pin chibi_select_pin = PIN_CHIBI_SELECT;

void chibi_select(void) {
	// Disable IRQ between chibi selects to make sure that there is no
	// chibi interrupt between the select.
	__disable_irq();

	PIO_Clear(&chibi_select_pin);
//	USART0->US_CR = US_CR_FCS;
}

void chibi_deselect(void) {
//	USART0->US_CR = US_CR_RCS;
	PIO_Set(&chibi_select_pin);
	__enable_irq();
	SLEEP_NS(CHIBI_TIME_BETWEEN_SELECT);
}

uint8_t chibi_transceive_byte(const uint8_t value) {
	// Wait for transfer buffer to be empty
	while((USART0->US_CSR & US_CSR_TXEMPTY) == 0);
	USART0->US_THR = value;

	// Wait until receive buffer has new data
	while((USART0->US_CSR & US_CSR_RXRDY) == 0);
	return USART0->US_RHR;
}

uint8_t chibi_read_register(const uint8_t address) {
	chibi_select();

	chibi_transceive_byte(address | CHIBI_SPI_RR);
	const uint8_t value = chibi_transceive_byte(0);

	chibi_deselect();

	return value;
}

void chibi_write_register(const uint8_t address, const uint8_t value) {
	chibi_select();

	chibi_transceive_byte(address | CHIBI_SPI_RW);
	chibi_transceive_byte(value);

	chibi_deselect();
}

uint8_t chibi_read_register_buffer(const uint8_t address,
                                   uint8_t *buffer,
                                   const uint8_t length) {
	const uint8_t min = MIN(length, CHIBI_REGISTER_NUM);

	for(uint8_t i = 0; i < min; i++) {
		buffer[i] = chibi_read_register(address + i);
	}

	return min;
}

uint8_t chibi_write_register_buffer(const uint8_t address,
                                    const uint8_t *buffer,
                                    const uint8_t length) {
	const uint8_t min = MIN(length, CHIBI_REGISTER_NUM);

	for(uint8_t i = 0; i < min; i++) {
		chibi_write_register(address + i, buffer[i]);
	}

	return min;
}

void chibi_write_register_mask(const uint8_t address,
                               const uint8_t value,
                               const uint8_t mask) {
	uint8_t current_value = chibi_read_register(address) & (~mask);
	chibi_write_register(address, current_value | (value & mask));
}

uint8_t chibi_read_register_mask(const uint8_t address, const uint8_t mask) {
	return chibi_read_register(address) & mask;
}

void chibi_write_frame(const uint8_t *header,
                       const uint8_t *data,
                       const uint8_t data_length) {
    // Don't allow transmission longer than max frame size
    if ((CHIBI_HEADER_LENGTH + data_length) > 127) {
    	logchibie("Header too long\n\r");
        return;
    }

    // Initiate spi transaction
	chibi_select();

    // Send fifo write command
    chibi_transceive_byte(CHIBI_SPI_FW);

    // Write length
    chibi_transceive_byte(data_length +
                          CHIBI_HEADER_LENGTH +
                          2);

    // Write header contents
    for(uint8_t i = 0; i < CHIBI_HEADER_LENGTH; i++) {
        chibi_transceive_byte(header[i]);
    }

    // Write data contents to fifo
    for(uint8_t i = 0; i < data_length; i++) {
    	chibi_transceive_byte(data[i]);
    }

    chibi_transceive_byte(0);
    chibi_transceive_byte(0);

    // Terminate spi transaction
    chibi_deselect();
}

void chibi_start_tx(void) {
	// Toggle sleep pin to start transfer
	PIO_Set(&chibi_sleep_pin);
	SLEEP_NS(CHIBI_TIME_SLEEP_PULSE);
	PIO_Clear(&chibi_sleep_pin);
}

bool chibi_is_sleeping(void) {
	return PIO_Get(&chibi_sleep_pin);
}

void chibi_sleep(void) {
	PIO_Set(&chibi_sleep_pin);
	SLEEP_NS(CHIBI_TIME_SLEEP_PULSE);
}

void chibi_wakeup(void) {
	PIO_Clear(&chibi_sleep_pin);
	SLEEP_NS(CHIBI_TIME_SLEEP_PULSE);
}

void chibi_reset_enable(void) {
	PIO_Clear(&chibi_reset_pin);
	SLEEP_NS(CHIBI_TIME_RESET_PULSE);
}

void chibi_reset_disable(void) {
	PIO_Set(&chibi_reset_pin);
	SLEEP_NS(CHIBI_TIME_RESET_PULSE);
}

void chibi_reset(void) {
	chibi_reset_disable();
	chibi_wakeup();

	chibi_reset_enable();
	chibi_reset_disable();
}

void chibi_set_mode(const uint8_t mode) {
    switch (mode) {
		case OQPSK_868MHZ_EUROPE:
			// 802.15.4-2006, channel page 2, channel 0 (868 MHz, Europe)
			chibi_write_register_mask(CHIBI_REGISTER_TRX_CTRL_2,
									  CHIBI_TRX_CTRL_2_BPSK_OQPSK |
									  CHIBI_TRX_CTRL_2_RX_SAFE_MODE,
									  0x3F | CHIBI_TRX_CTRL_2_RX_SAFE_MODE);

			chibi_write_register_mask(CHIBI_REGISTER_RF_CTRL_0,
									  CHIBI_OQPSK_TX_OFFSET,
									  CHIBI_RF_CTRL_0_GC_TX_OFFS_MASK);
			break;
		case OQPSK_915MHZ_US:
			 // 802.15.4-2006, channel page 2, channels 1-10 (915 MHz, US)
			chibi_write_register_mask(CHIBI_REGISTER_TRX_CTRL_2,
									  CHIBI_TRX_CTRL_2_SUB_MODE |
									  CHIBI_TRX_CTRL_2_BPSK_OQPSK |
									  CHIBI_TRX_CTRL_2_RX_SAFE_MODE,
									  0x3F | CHIBI_TRX_CTRL_2_RX_SAFE_MODE);

			chibi_write_register_mask(CHIBI_REGISTER_RF_CTRL_0,
									  CHIBI_OQPSK_TX_OFFSET,
									  CHIBI_RF_CTRL_0_GC_TX_OFFS_MASK);
			break;
		case OQPSK_780MHZ_CHINA:
			// 802.15.4-2006, channel page 5, channel 0-3 (780 MHz, China)
			chibi_write_register_mask(CHIBI_REGISTER_TRX_CTRL_2,
									  CHIBI_TRX_CTRL_2_SUB_MODE |
									  CHIBI_TRX_CTRL_2_BPSK_OQPSK |
									  CHIBI_TRX_CTRL_2_OQPSK_SUB1_RC_EN |
									  CHIBI_TRX_CTRL_2_RX_SAFE_MODE,
									  0x3F | CHIBI_TRX_CTRL_2_RX_SAFE_MODE);

			chibi_write_register_mask(CHIBI_REGISTER_RF_CTRL_0,
									  CHIBI_OQPSK_TX_OFFSET,
									  CHIBI_RF_CTRL_0_GC_TX_OFFS_MASK);
			break;
		case BPSK40_915MHZ:
			// 802.15.4-2006, BPSK, 40 kbps
			chibi_write_register_mask(CHIBI_REGISTER_TRX_CTRL_2,
			                          CHIBI_TRX_CTRL_2_RX_SAFE_MODE,
									  0x3F | CHIBI_TRX_CTRL_2_RX_SAFE_MODE);

			chibi_write_register_mask(CHIBI_REGISTER_RF_CTRL_0,
									  CHIBI_BPSK_TX_OFFSET,
									  CHIBI_RF_CTRL_0_GC_TX_OFFS_MASK);
			break;
    }
}

void chibi_set_channel(const uint8_t channel) {
	// TODO: china only stuff


    chibi_write_register_mask(CHIBI_REGISTER_PHY_CC_CCA,
                              channel,
                              CHIBI_PHY_CC_CCA_CHANNEL_MASK);

    // Add delay to allow the PLL to lock if in active mode.
    const uint8_t status = chibi_read_register_mask(CHIBI_REGISTER_TRX_STATUS,
                                                   CHIBI_STATUS_MASK);

    if ((status == CHIBI_STATUS_RX_ON) || (status == CHIBI_STATUS_PLL_ON)) {
        SLEEP_NS(CHIBI_TIME_PLL_LOCK_TIME);
    }
}

uint8_t chibi_get_status() {
	return chibi_read_register_mask(CHIBI_REGISTER_TRX_STATUS,
	                                CHIBI_STATUS_MASK);
}


uint8_t chibi_set_state(const uint8_t state) {
    // If we're sleeping then don't allow transition
	if(chibi_is_sleeping()) {
        return CHIBI_ERROR_WRONG_STATE;
	}

    // if we're in a transition status, wait for the status to become stable
    const uint8_t current_status = chibi_get_status();
    if((current_status == CHIBI_STATUS_BUSY_TX_ARET) ||
       (current_status == CHIBI_STATUS_BUSY_RX_AACK) ||
       (current_status == CHIBI_STATUS_BUSY_RX) ||
       (current_status == CHIBI_STATUS_BUSY_TX)) {
        while(chibi_get_status() == current_status);
    }

    // At this point it is clear that the requested new_state is:
    // TRX_OFF, RX_ON, PLL_ON, RX_AACK_ON or TX_ARET_ON.
    // We need to handle some special cases before we
    // transition to the new state
    switch (state) {
		case CHIBI_STATE_TRX_OFF:
			// Go to TRX_OFF from any state.
			chibi_wakeup();
			chibi_write_register_mask(CHIBI_REGISTER_TRX_STATE,
			                          CHIBI_STATE_FORCE_TRX_OFF,
			                          CHIBI_STATE_MASK);
			SLEEP_NS(CHIBI_TIME_ALL_STATES_TRX_OFF);
			break;

		case CHIBI_STATE_TX_ARET_ON:
			if(current_status == CHIBI_STATUS_RX_AACK_ON) {
				// First do intermediate state transition to PLL_ON,
				// then to TX_ARET_ON.
				chibi_write_register_mask(CHIBI_REGISTER_TRX_STATE,
				                          CHIBI_STATE_PLL_ON,
				                          CHIBI_STATE_MASK);
				SLEEP_NS(CHIBI_TIME_RX_ON_PLL_ON);
			}
			break;

		case CHIBI_STATE_RX_AACK_ON:
			if(current_status == CHIBI_STATUS_TX_ARET_ON) {
				// First do intermediate state transition to RX_ON,
				// then to RX_AACK_ON.
				chibi_write_register_mask(CHIBI_REGISTER_TRX_STATE,
				                          CHIBI_STATE_PLL_ON,
				                          CHIBI_STATE_MASK);
				SLEEP_NS(CHIBI_TIME_RX_ON_PLL_ON);
			}
			break;
    }

    // Now we're okay to transition to any new state.
    chibi_write_register_mask(CHIBI_REGISTER_TRX_STATE,
                              state,
                              CHIBI_STATE_MASK);

    // When the PLL is active most states can be reached in 1us.
    // However, from TRX_OFF the PLL needs 200us to activate.
    if(current_status == CHIBI_STATUS_TRX_OFF) {
    	SLEEP_NS(CHIBI_TIME_TRX_OFF_PLL_ON);
    } else {
    	SLEEP_NS(CHIBI_TIME_RX_ON_PLL_ON);
    }

    if (chibi_get_status() == state) {
        return CHIBI_ERROR_OK;
    }

    return CHIBI_ERROR_TIMEOUT;
}

uint8_t chibi_transfer(const uint8_t *header,
                       const uint8_t *data,
                       const uint8_t length) {
	// If we are currently waiting for a receive (i.e. we are chibi slave,
	// received a no ack and are waiting that the master sends data) don't
	// interfere with a transfer
	if(chibi_wait_for_recv) {
		chibi_wait_for_recv--;
		return CHIBI_ERROR_NO_ACK;
	}

    // Transition to off state, then go to tx_aret_on
    chibi_set_state(CHIBI_STATE_TRX_OFF);
    chibi_set_state(CHIBI_STATE_TX_ARET_ON);

    // Write frame to AT86RF212 buffer
    chibi_write_frame(header, data, length);

    // Do frame transmission
    chibi_transfer_end = false;
    chibi_start_tx();

    // Wait for transfer to end
    while(!chibi_transfer_end) {
    	//taskYIELD();
    }

    return chibi_transfer_status;
}

void chibi_read_frame(void) {
	chibi_select();

    // Send frame read command and read length
    chibi_transceive_byte(CHIBI_SPI_FR);
    const uint8_t length = chibi_transceive_byte(0);
    const uint8_t data_length = length - CHIBI_HEADER_LENGTH - 2;

    // Check frame length
    if((length >= CHIBI_MIN_FRAME_LENGTH) &&
       (length <= CHIBI_MAX_FRAME_LENGTH)) {
    	if(chibi_buffer_size_recv > 0) {
    		logchibie("Unexpected chibi overflow\n\r");
    	}

    	uint16_t chibi_read_fcs;
    	ChibiHeaderMPDU chibi_read_header;

    	for(uint8_t i = 0; i < CHIBI_HEADER_LENGTH; i++) {
    		((uint8_t*)&chibi_read_header)[i] = chibi_transceive_byte(0);
    	}

    	for(uint8_t i = 0; i < data_length; i++) {
    		chibi_buffer_recv[i] = chibi_transceive_byte(0);
    	}

    	for(int i = 0; i < CHIBI_FCS_LENGTH; i++) {
    		((uint8_t*)&chibi_read_fcs)[i] = chibi_transceive_byte(0);
    	}

    	chibi_buffer_size_recv = data_length;


    	/*logchibid("header: %d %d %d %d %d\n\r",
    	          chibi_read_header.frame_control_field,
    	          chibi_read_header.sequence,
    	          chibi_read_header.pan_id,
    	          chibi_read_header.short_destination_address,
    	          chibi_read_header.short_source_address);
    	logchibid("data: %d %d %d %d %d %d\n\r",
    			  chibi_buffer_recv[0],
    			  chibi_buffer_recv[1],
    			  chibi_buffer_recv[2],
    			  chibi_buffer_recv[3],
    			  chibi_buffer_recv[4],
    			  chibi_buffer_recv[5]);
    	logchibid("fcs: %d\n\r", chibi_read_fcs);*/
    }

    chibi_deselect();
}

void chibi_interrupt(const Pin *pin) {
	// Get IRQ Status
	const uint8_t irq_status = chibi_read_register(CHIBI_REGISTER_IRQ_STATUS);

	if(irq_status & CHIBI_IRQ_MASK_TRX_END)	{
		const uint8_t status = chibi_get_status();

		if((status == CHIBI_STATUS_RX_ON) ||
		   (status == CHIBI_STATUS_RX_AACK_ON) ||
		   (status == CHIBI_STATUS_BUSY_RX_AACK)) {
			chibi_select();
			// Get PHY RSSI register for crc
			// with SPI_CMD_MODE_PHY feature
			const uint8_t crc = chibi_transceive_byte(
				CHIBI_REGISTER_PHY_ED_LEVEL | CHIBI_SPI_RR
			);

			// Get ED measurement
			// To calculate Receiver Input Power [dBm]:
			// RSSI_BASE_VAL + 1.03 * ED_LEVEL
			chibi_receiver_input_power = chibi_transceive_byte(0);
			chibi_deselect();

			//logchibid("power: %d\n\r", chibi_receiver_input_power);

			// Check CRC
			if(chibi_buffer_size_recv > 0) {
				chibi_overflow++;
				logchibie("Buffer Overflow: %d\n\r", chibi_overflow);
			} else if((crc & CHIBI_PHY_RSSI_RX_CRC_VALID) != 0) {
				chibi_read_frame();
				chibi_wait_for_recv = 0;
			} else {
				logchibiw("CRC Error: %d\n\r", chibi_crc_error);
				chibi_crc_error++;
			}
		} else {
			uint8_t trac_status = CHIBI_STATE_TRAC_STATUS(
				chibi_read_register(CHIBI_REGISTER_TRX_STATE));

			// Look if transfer was successful
			switch(trac_status) {
				case CHIBI_TRAC_STATUS_SUCCESS:
				case CHIBI_TRAC_STATUS_SUCCESS_DATA_PENDING:
					chibi_transfer_status = CHIBI_ERROR_OK;
					break;

				case CHIBI_TRAC_STATUS_CHANNEL_ACCESS_FAILURE:
				case CHIBI_TRAC_STATUS_NO_ACK:
					// If no ack was received, try again if we are master and
					// change state to receive if we are slave
					chibi_no_ack++;
					if(chibi_type == CHIBI_TYPE_SLAVE) {
						chibi_transfer_status = CHIBI_ERROR_NO_ACK;
						chibi_wait_for_recv = 1000;
					} else if(chibi_type == CHIBI_TYPE_MASTER) {
						chibi_start_tx();
						return;
					} else {
						logchibie("Unexpected chibi type: %d\n\r", chibi_type);
					}
					//logchibii("Did not receieve ACK: %d\n\r", chibi_no_ack);
					break;

				case CHIBI_TRAC_STATUS_SUCCESS_WAIT_FOR_ACK:
				case CHIBI_TRAC_STATUS_INVALID:
				default:
					chibi_transfer_status = CHIBI_ERROR_UNEXPECTED;
					logchibie("Unexpected trac status: %d\n\r", trac_status);
					break;
			}

			// Chibi transfer ended, chibi_transfer can be called again
			chibi_transfer_end = true;
		}

		uint8_t error;
		do {
#ifdef CHIBI_USE_PROMISCUOUS_MODE
				error = chibi_set_state(CHIBI_STATE_RX_ON);
#else
				error = chibi_set_state(CHIBI_STATE_RX_AACK_ON);
#endif
		} while(error != CHIBI_ERROR_OK);
	}

	// Frame buffer access conflict or trying to send more then 127 bytes
	if(irq_status & CHIBI_IRQ_MASK_TRX_UR) {
		chibi_underrun++;
		logchibiw("Underrun: %d\n\r", chibi_underrun);
	}
}

void chibi_low_level_init(void) {
	Pin chibi_select = PIN_EXT_0_SELECT;
	PIO_Clear(&chibi_select);

	chibi_reset();

	// Configure chibi interrupt pin
	chibi_interrupt_pin.attribute = PIO_IT_RISE_EDGE;
    PIO_Configure(&chibi_interrupt_pin, 1);
    PIO_ConfigureIt(&chibi_interrupt_pin, chibi_interrupt);

	// Disable chibi Interrupts
    uint32_t test = 0;
	chibi_write_register(CHIBI_REGISTER_IRQ_MASK, 0);
	test = chibi_read_register(CHIBI_REGISTER_IRQ_MASK);
	printf("irq: %d\n\r", test);
	test = chibi_read_register(CHIBI_REGISTER_IRQ_MASK);
	printf("irq: %d\n\r", test);
	test = chibi_read_register(CHIBI_REGISTER_IRQ_MASK);
	printf("irq: %d\n\r", test);

	// Turn transceiver off
	chibi_write_register_mask(CHIBI_REGISTER_TRX_STATE,
	                          CHIBI_STATE_FORCE_TRX_OFF,
	                          CHIBI_STATE_MASK);

	// Wait for transceiver to turn off
    printf("before write\n\r");

	/*while(chibi_read_register_mask(CHIBI_REGISTER_TRX_STATUS,
	                               CHIBI_STATUS_MASK) != CHIBI_STATUS_TRX_OFF);*/
    while(test != CHIBI_STATUS_TRX_OFF) {
    	test = chibi_read_register_mask(CHIBI_REGISTER_TRX_STATUS, CHIBI_STATUS_MASK);
    	printf("test: %d\n\r", test);
    }
	printf("after write\n\r");

	// Accept 802.15.4 frames ver 0 or 1
	chibi_write_register_mask(CHIBI_REGISTER_CSMA_SEED_1,
	                          CHIBI_CSMA_SEED_1_AACK_FVN_MODE_01,
	                          CHIBI_CSMA_SEED_1_AACK_FVN_MODE_MASK);

	// Write CSMA and frame retries
	chibi_write_register(CHIBI_REGISTER_XAH_CTRL_0,
	                     CHIBI_XAH_CTRL_0_CSMA_RETRIES(CHIBI_CSMA_RETRIES) |
	                     CHIBI_XAH_CTRL_0_FRAME_RETRIES(CHIBI_FRAME_RETRIES));

	// Set mode and channel
	chibi_set_mode(OQPSK_868MHZ_EUROPE);
	chibi_set_channel(CHIBI_PHY_CC_CCA_CHANNEL_868);

	// Enable auto crc mode
	chibi_write_register_mask(CHIBI_REGISTER_TRX_CTRL_1,
#ifdef CHIBI_USE_CRC
		                      CHIBI_TRX_CTRL_1_TX_AUTO_CRC_ON |
#endif
		                      CHIBI_TRX_CTRL_1_SPI_CMD_MODE_PHY |
		                      CHIBI_TRX_CTRL_1_IRQ_MASK_MODE,
#ifdef CHIBI_USE_CRC
		                      CHIBI_TRX_CTRL_1_TX_AUTO_CRC_ON |
#endif
		                      CHIBI_TRX_CTRL_1_SPI_CMD_MODE_MASK |
		                      CHIBI_TRX_CTRL_1_IRQ_MASK_MODE);

    // Set Pan ID
	uint16_t pan_id = CHIBI_PAN_ID;
    chibi_write_register_buffer(CHIBI_REGISTER_PAN_ID_0,
                                (uint8_t*)&pan_id,
                                sizeof(uint16_t));

    // Set Short Address
    uint16_t short_address = chibi_address;
    chibi_write_register_buffer(CHIBI_REGISTER_SHORT_ADDR_0,
                                (uint8_t*)&short_address,
                                sizeof(uint16_t));

	// Interrupts on tx end
	chibi_write_register(CHIBI_REGISTER_IRQ_MASK, CHIBI_IRQ_MASK_TRX_END);
    PIO_EnableIt(&chibi_interrupt_pin);

	// Set state according to usage of CRC
#ifdef CHIBI_USE_PROMISCUOUS_MODE
	chibi_set_state(CHIBI_STATE_RX_ON);
#else
	chibi_set_state(CHIBI_STATE_RX_AACK_ON);
#endif
}


