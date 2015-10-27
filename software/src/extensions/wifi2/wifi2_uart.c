/* master-brick
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi2_uart.c: Low level WIFI2 communication with ESP8266
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

#include "wifi2_uart.h"

#include <string.h>

#include "bricklib/free_rtos/include/FreeRTOS.h"
#include "bricklib/free_rtos/include/task.h"

#include "config.h"
#include "wifi2_config.h"
#include "wifi2_uart.h"

#include "bricklib/com/com.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/init.h"
#include "bricklib/utility/pearson_hash.h"

#include "bricklib/drivers/usart/usart.h"
#include "bricklib/drivers/pio/pio.h"

extern ComInfo com_info;
extern Pin extension_pins[];

// We steal the buffer from WIFI Extension 1.0.
// Both WIFI Extensions are never used at the same time.
extern char wifi_ringbuffer[];
Wifi2 *w2 = (Wifi2*)wifi_ringbuffer;


static uint8_t wifi2_uart_checksum(const uint8_t *data, const uint8_t length) {
	uint8_t checksum = 0;
	for(uint8_t i = 0; i < length; i++) {
		PEARSON(checksum, data[i]);
	}

	return checksum;
}

// Just peak at length bytes
static bool wifi2_uart_rx_peak(uint8_t *data, uint8_t length) {
	// Copy current receive counter and index, so we can work on a temporary copy.
	uint8_t last_rcr_tmp = w2->recv_dma_last_rcr;
	uint8_t index = w2->recv_dma_index;

	// Go through length many bytes.
	for(uint8_t i = 0; i < length; i++) {
		// If more data is available
		if(last_rcr_tmp > USART_WIFI2->US_RCR || USART_WIFI2->US_RNCR == 0) {
			// Save data at appropriate position
			data[i] = w2->recv_dma_buffer[w2->recv_dma_index][WIFI2_UART_BUFFER_SIZE-last_rcr_tmp];
			// Change buffer if receive counter goes to 0.
			last_rcr_tmp--;
			if(last_rcr_tmp == 0) {
				index = 1 - index;
				last_rcr_tmp = WIFI2_UART_BUFFER_SIZE;
			}
		} else {
			// If there are not enough bytes available we return false.
			return false;
		}
	}

	// We were able to peak at length bytes.
	return true;
}

bool wifi2_uart_rx(uint8_t *data) {
	// Have we received new data since last call?
	if(w2->recv_dma_last_rcr > USART_WIFI2->US_RCR || USART_WIFI2->US_RNCR == 0) {
		// Save byte
		*data = w2->recv_dma_buffer[w2->recv_dma_index][WIFI2_UART_BUFFER_SIZE-w2->recv_dma_last_rcr];

		// Save current counter value
		w2->recv_dma_last_rcr--;
		if(w2->recv_dma_last_rcr == 0) {
			// If buffer is read completely, we change to second buffer
			// and re-set buffer as next buffer.
			USART_WIFI2->US_RNPR = (uint32_t)w2->recv_dma_buffer[w2->recv_dma_index];
			w2->recv_dma_index = 1 - w2->recv_dma_index;
			USART_WIFI2->US_RNCR = WIFI2_UART_BUFFER_SIZE;
			w2->recv_dma_last_rcr = WIFI2_UART_BUFFER_SIZE;
		}

		// We do return new data
		return true;
	}

	// No new data
	return false;
}

void wifi2_clear_dma_recv_buffer(void) {
	// Clear DMA recv buffer and reset corresponding variables.
	// This function is called in case of bit errors or similar.
    USART_WIFI2->US_PTCR = US_PTCR_RXTDIS;

	w2->recv_dma_last_rcr = WIFI2_UART_BUFFER_SIZE;
	w2->recv_dma_index = 0;

	w2->recv_buffer_current_index = 0;
	w2->recv_buffer_expected_length = 3;

    USART_WIFI2->US_RPR = (uint32_t)w2->recv_dma_buffer[0];
    USART_WIFI2->US_RCR = WIFI2_UART_BUFFER_SIZE;
    USART_WIFI2->US_RNPR = (uint32_t)w2->recv_dma_buffer[1];
    USART_WIFI2->US_RNCR = WIFI2_UART_BUFFER_SIZE;
    USART_WIFI2->US_PTCR = US_PTCR_RXTEN;
}

void ack_last_message(void) {
	// Second buffer should always be free here.
	// If it is not we just wait it out.
	while(USART_WIFI2->US_TNCR != 0) {
		logwifi2w("Second buffer not empty!\n\r");
		__NOP();
	}

	// In an ACK message we send the last sequence number seen,
	// but not a new sequence number ourself. So this message does not have to be ACKed.
	// If the WIFI Extension misses to ACK, we will recover by re-sending the message.
	w2->ack_buffer[0] = WIFI2_UART_HEADER_SIZE;
	w2->ack_buffer[1] = 0 | (w2->uart_last_sequence_number_seen << 4);
	w2->ack_buffer[2] = wifi2_uart_checksum(w2->ack_buffer, WIFI2_UART_HEADER_SIZE-1);

	USART_WIFI2->US_TNPR = (uint32_t)w2->ack_buffer;
	USART_WIFI2->US_TNCR = 3;
	USART_WIFI2->US_PTCR = US_PTCR_TXTEN;
}

void wifi2_uart_peak_for_ack(void) {
	// Peak for data
	uint8_t data[3] = {0, 0, 0};
	if(wifi2_uart_rx_peak(data, 3)) {
		if(data[UART_CON_INDEX_LENGTH] == 3) {
			const uint8_t seq_seen_by_extension = data[UART_CON_INDEX_SEQUENCE] >> 4;
			if(w2->ack_wait) {
				if(seq_seen_by_extension == w2->uart_sequence_number) {
					// If we do find an ACK message and the sequence number fits,
					// We reset timeout and increase sequence number.
					w2->send_timeout = 0;
					w2->ack_wait = false;
					w2->uart_sequence_number++;
					if(w2->uart_sequence_number >= 0x10) {
						w2->uart_sequence_number = 1;
					}

					// Set buffer length to 0, so wifi_send can write to it again
					w2->send_buffer_message_length = 0;
				}
			}

			// Read data if it was indeed an ACK.
			uint8_t dummy;
			wifi2_uart_rx(&dummy);
			wifi2_uart_rx(&dummy);
			wifi2_uart_rx(&dummy);
		} else {
			// In theory we only peak for an ACK if one should be available.
			// However, if there is a bit error or similar
			// we recover by clearing the whole buffer. The WIFI Extension
			// will re-send the last message and we can ACK it then.
			logwifi2f("Peak failed: %d %d %d\n\r", data[0], data[1], data[2]);
			wifi2_clear_dma_recv_buffer();
		}
	}
}

void wifi2_uart_recv(void) {
	if(w2->recv_buffer_tfp_wait) {
		// All buffers are full, we have to wait until the recv buffer is read out.
		if(w2->recv_buffer_tfp_length == 0) {
			// The receive buffer is read out. We can now copy the already saved buffer
			// and the recv buffer can be filled again afterwards.
			const uint8_t length = w2->recv_buffer[UART_CON_INDEX_LENGTH] - 3;
			memcpy(w2->recv_buffer_tfp, &w2->recv_buffer[2], length);
			w2->recv_buffer_tfp_length = length;
			w2->recv_buffer_tfp_wait = false;
		} else {
			// Lets see if an ACK is available, we need to make sure we see ACKs
			// for the packets we send. Even if all of the recv buffers are full.
			// Otherwise we can reach a deadlock situation
			wifi2_uart_peak_for_ack();
		}
		return;
	}

	// Read data one byte at a time as long as new data is available.
	uint8_t rx_data = 0;
	while(wifi2_uart_rx(&rx_data)) {
		// Save data to current index
		w2->recv_buffer[w2->recv_buffer_current_index] = rx_data;

		if(w2->recv_buffer_current_index == UART_CON_INDEX_LENGTH) {
			// The first byte in the WIFI2 uart protocol is the length
			w2->recv_buffer_expected_length = w2->recv_buffer[UART_CON_INDEX_LENGTH];

			// Sanity check the length. Allowed sizes are 3 and 11-83.
			if(w2->recv_buffer_expected_length > WIFI2_UART_BUFFER_SIZE || (w2->recv_buffer_expected_length < 11 && w2->recv_buffer_expected_length != 3)) {
				// If the length is outside of the allowed ranges, we clear
				// all of the recv buffers and reset all of the variables.
				// The WIFI2 Extension will resend the data, so nothing is lost.

				logwifi2e("Length is malformed: %d (not 3 || 11-83)\n\r", w2->recv_buffer_expected_length);
				wifi2_clear_dma_recv_buffer();
				return;
			}
		}

		w2->recv_buffer_current_index++;
		if(w2->recv_buffer_current_index >= w2->recv_buffer_expected_length) {
			// We read a complete packet, lets check the checksum!
			const uint8_t checksum_calculated = wifi2_uart_checksum(w2->recv_buffer, w2->recv_buffer_current_index-1);
			const uint8_t checksum_uart = w2->recv_buffer[w2->recv_buffer_current_index-1];
			if(checksum_calculated != checksum_uart) {
				logwifi2w("Checksum error: %d (calc) != %d (uart)\n\r", checksum_calculated, checksum_uart);
				logwifi2d("Uart data (len, seq): %d, %d\n\r", w2->recv_buffer[0], w2->recv_buffer[1]);
				com_debug_message((const MessageHeader *)&w2->recv_buffer[UART_CON_INDEX_TFP_START]);

				// In case of checksum failure we clear
				// all of the recv buffers and reset all of the variables again.
				wifi2_clear_dma_recv_buffer();
				return;
			}

			// Is the packaet a ACK packet or a data packet? The ACK packet only consists
			// of the UART protocol header and footer
			if(w2->recv_buffer_expected_length != WIFI2_UART_HEADER_SIZE) {

				// Compare sequence number, have we already handled this packet?
				// If we did, we ignore it. But we have to send the ACK again.
				const uint8_t seq = w2->recv_buffer[UART_CON_INDEX_SEQUENCE] & 0x0F;
				if(w2->uart_last_sequence_number_seen != seq) {
					w2->uart_last_sequence_number_seen = seq;

					// If the recv buffer for the TFP data is empty we can write to it.
					// Oterwise we set the wait flag and the data is copied after the
					// recv buffer is read out (see top of this function)
					if(w2->recv_buffer_tfp_length == 0) {
						memcpy(w2->recv_buffer_tfp, &w2->recv_buffer[2], w2->recv_buffer_expected_length-3);
						w2->recv_buffer_tfp_length = w2->recv_buffer_expected_length-3;
					} else {
						w2->recv_buffer_tfp_wait = true;
					}
				} else {
					ack_last_message();
					logwifi2i("Got sequence number twice: %d\n\r", seq);
				}
			}

			// Are we waiting for an ACK?
			if(w2->ack_wait) {
				const uint8_t seq_seen_by_extension = w2->recv_buffer[UART_CON_INDEX_SEQUENCE] >> 4;

				// Does the last sequence number the WIFI2 Extension has seen match the last message we send?
				if(seq_seen_by_extension == w2->uart_sequence_number) {
					// If the sequence number does match we can stop the timeout and increase the sequence number
					// so it can be used for the next data that is send to the extension.
					w2->send_timeout = 0;
					w2->ack_wait = false;
					w2->uart_sequence_number++;
					if(w2->uart_sequence_number >= 0x10) {
						w2->uart_sequence_number = 1;
					}

					// Set buffer length to 0, so wifi_send can write to it again.
					w2->send_buffer_message_length = 0;
				}
			}

			// We read one full message, reset index to 0 and expected length to smallest
			// length possible. It will be updated as soon as we know the correct length.
			w2->recv_buffer_current_index = 0;
			w2->recv_buffer_expected_length = WIFI2_UART_HEADER_SIZE;
			break;
		}
	}
}

void wifi2_uart_send(void) {
	// if the timoeut is > 0, we are currently waiting on an ack for
	// a already send message, we have to wait until the ack is received.
	// or the timeout runs out.
	if(w2->send_timeout > 0) {
		w2->send_timeout--;
		return;
	}

	if(w2->ack_wait) {
		// There was no ACK for the last message.
		// Something must have gone wrong, lets re-send it!
		USART_WIFI2->US_TPR  = (uint32_t)w2->send_dma_buffer;
		USART_WIFI2->US_TCR  = w2->send_dma_buffer_length;
		USART_WIFI2->US_PTCR = US_PTCR_TXTEN;
		w2->send_timeout = WIFI2_ACK_TIMEOUT_MS;

		w2->ack_wait = true;
		return;
	}

	// If there is nothing to send there is nothing to do here.
	if(w2->send_buffer_message_length == 0) {
		return;
	}

	// So there is a message to send. We can send it if the DMA
	// send buffer is not currently still sending.
	if(USART_WIFI2->US_CSR & US_CSR_TXBUFE) {

		// Set the length of the message plus the length of the
		// UART protocol header/footer
		w2->send_dma_buffer[UART_CON_INDEX_LENGTH] = w2->send_buffer_message_length + WIFI2_UART_HEADER_SIZE;

		// Add current sequence number (which is increased after we see the ACK)
		// and also add the last sequence number we have seen from the WIFI Extension.
		w2->send_dma_buffer[UART_CON_INDEX_SEQUENCE] = w2->uart_sequence_number | (w2->uart_last_sequence_number_seen << 4);

		// Set data.
		for(uint8_t i = 0; i < w2->send_buffer_message_length; i++) {
			w2->send_dma_buffer[UART_CON_INDEX_TFP_START+i] = ((uint8_t *)w2->send_buffer)[i];
		}

		// Calculate and set checksum (last byte of UART packet)
		w2->send_dma_buffer[UART_CON_INDEX_CHECKSUM(w2->send_dma_buffer[UART_CON_INDEX_LENGTH])] = wifi2_uart_checksum(w2->send_dma_buffer, w2->send_dma_buffer[UART_CON_INDEX_LENGTH]-1);

		// Save the buffer length, so we can reuse it if we don't receive
		// and ACK and have to resend the message
		w2->send_dma_buffer_length = w2->send_buffer_message_length + WIFI2_UART_HEADER_SIZE;

		// Set timeout, now we have to wait for the ACK.
		w2->send_timeout = WIFI2_ACK_TIMEOUT_MS;
		w2->ack_wait = true;

		// Start DMA transfer
		USART_WIFI2->US_TPR  = (uint32_t)w2->send_dma_buffer;
		USART_WIFI2->US_TCR  = w2->send_dma_buffer_length;
		USART_WIFI2->US_PTCR = US_PTCR_TXTEN;
	}
}

void wifi2_uart_init(const uint8_t extension) {
	// First set everything to 0
	for(uint32_t i = 0; i < sizeof(Wifi2); i++) {
		wifi_ringbuffer[i] = 0;
	}

	// Initialize variables
	w2->enable = WIFI2_ENABLE_0;
	w2->nreset = WIFI2_NRESET_0;
	w2->flashc = WIFI2_FLASHC_0;
	w2->recv_dma_last_rcr = WIFI2_UART_BUFFER_SIZE;
	w2->recv_dma_index = 0;

	w2->send_buffer_message_length = 0;

	w2->recv_buffer_current_index = 0;
	w2->recv_buffer_expected_length = WIFI2_UART_HEADER_SIZE;

	w2->recv_buffer_tfp_length = 0;
	w2->recv_buffer_tfp_wait = false;

	w2->uart_sequence_number = 1;
	w2->uart_last_sequence_number_seen = 0;

	w2->send_timeout = 0;

	w2->ack_wait = false;
	w2->bootloader_mode = false;

	if(extension == 0) {
		w2->enable = WIFI2_ENABLE_0;
		w2->nreset = WIFI2_NRESET_0;
		w2->flashc = WIFI2_FLASHC_0;
	} else if(extension == 1) {
		w2->enable = WIFI2_ENABLE_1;
		w2->nreset = WIFI2_NRESET_1;
		w2->flashc = WIFI2_FLASHC_1;
	}

	SLEEP_MS(200); // TODO: Find minimum sleep time

	// Enable ESP8266 and restart it.
	extension_pins[w2->enable].type      = PIO_OUTPUT_1;
	extension_pins[w2->enable].attribute = PIO_DEFAULT;
	extension_pins[w2->nreset].type      = PIO_OUTPUT_0;
	extension_pins[w2->nreset].attribute = PIO_DEFAULT;
	extension_pins[w2->flashc].type      = PIO_OUTPUT_1;
	extension_pins[w2->flashc].attribute = PIO_DEFAULT;
	PIO_Configure(&extension_pins[w2->nreset], 1);
	PIO_Configure(&extension_pins[w2->enable], 1);
	PIO_Configure(&extension_pins[w2->flashc], 1);

	SLEEP_MS(200); // TODO: Find minimum sleep time

	// Restart done.
	extension_pins[w2->nreset].type = PIO_OUTPUT_1;
	PIO_Configure(&extension_pins[w2->nreset], 1);
	SLEEP_MS(200); // TODO: Find minimum sleep time

	// Configure WIFI2 pins.
    const Pin pins_wifi2[] = PINS_WIFI2;
	PIO_Configure(pins_wifi2, PIO_LISTSIZE(pins_wifi2));
}

void wifi2_uart_tick(const uint8_t tick_type) {
	// In bootloader mode we want nothing else to interfere
	if(w2->bootloader_mode) {
		return;
	}

	// There are no callback messages or similar to implement
	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		return;
	}

	// Run recv and send state machines.
	wifi2_uart_recv();
	wifi2_uart_send();
}
