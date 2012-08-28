/* master-brick
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * wifi.c: High Level WIFI protocol implementation
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


#include "wifi.h"

#include <string.h>

#include "config.h"
#include "wifi_config.h"
#include "wifi_command.h"
#include "wifi_data.h"
#include "wifi_low_level.h"

#include "bricklib/com/com.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/led.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/pearson_hash.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/bricklet/bricklet_config.h"
#include "extensions/extension_init.h"
#include "extensions/extension_i2c.h"


extern ComType com_ext[];
extern uint32_t led_rxtx;

uint8_t wifi_state = WIFI_STATE_COMMAND_IDLE;

uint8_t wifi_buffer_recv[WIFI_BUFFER_SIZE] = {0};
uint16_t wifi_buffer_size_recv = 0;

extern ComType com_current;

extern uint8_t com_stack_id;
extern uint8_t com_last_spi_stack_id;

extern BrickletSettings bs[];
extern const BrickletAddress baddr[];

WifiConfiguration wifi_configuration;
WifiStatus wifi_status;

bool wifi_init(void) {
    Pin pins_wifi_spi[] = {PINS_WIFI_SPI};
    PIO_Configure(pins_wifi_spi, PIO_LISTSIZE(pins_wifi_spi));
    pins_wifi_spi[4].type = PIO_OUTPUT_1;
    pins_wifi_spi[5].type = PIO_OUTPUT_1;
    pins_wifi_spi[6].type = PIO_INPUT;

    wifi_low_level_deselect();

    wifi_read_config((char *)&wifi_configuration, sizeof(WifiConfiguration), 0);

    uint32_t mode = US_MR_USART_MODE_SPI_MASTER |
                    US_MR_USCLKS_MCK |
                    US_MR_CHRL_8_BIT |
                    US_MR_PAR_NO |
                    US_MR_CHMODE_NORMAL |
                    US_MR_CLKO |
                    US_SPI_BPMODE_1;

    PMC->PMC_PCER0 = 1 << ID_WIFI_SPI;
    USART_Configure(USART_WIFI_SPI, mode, WIFI_SPI_CLOCK, BOARD_MCK);
    NVIC_EnableIRQ(IRQ_WIFI_SPI);


    USART_SetTransmitterEnabled(USART_WIFI_SPI, 1);
    USART_SetReceiverEnabled(USART_WIFI_SPI, 1);

    wifi_command_flush();

    printf("ate0\n\r");
    wifi_command_send(WIFI_COMMAND_ID_AT_ATE0);
    wifi_command_flush();

    printf("atv0\n\r");
    wifi_command_send(WIFI_COMMAND_ID_AT_ATV0);
    wifi_command_flush();

    // TOOD: Is it worth to use PHASE=1?
    // With PHASE=1 we need only one select per package.
    // But we would need every write command function two times!
    /*wifi_command_send(WIFI_COMMAND_ID_AT_SPICONF);

    // Reconfigure with phase=1
    mode |= US_MR_CPHA;

    PMC->PMC_PCER0 = 1 << ID_WIFI_SPI;
    USART_Configure(USART_WIFI_SPI, mode, WIFI_SPI_CLOCK, BOARD_MCK);
    NVIC_EnableIRQ(IRQ_WIFI_SPI);


    USART_SetTransmitterEnabled(USART_WIFI_SPI, 1);
    USART_SetReceiverEnabled(USART_WIFI_SPI, 1);*/

    printf("at\n\r");
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT);
    printf("at+wd\n\r");
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WD);
//	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXPS_OFF);

	// Wifi module always on (no sleep)
    printf("at+wrxactive\n\r");
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXACTIVE_ON);

	// Bulk mode data transfer
    printf("at+bdata\n\r");
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_BDATA_ON);

	if(wifi_configuration.encryption == ENCRYPTION_WPA) {
		printf("at+wwpa\n\r");
		wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WWPA);
	} else {
		// TODO
	}

	if(wifi_configuration.connection == CONNECTION_DHCP) {
		printf("at+ndhcp on\n\r");
		wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NDHCP_ON);
	} else {
		wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NSET);
	}

	// TODO: AT+WPAPSK

	printf("at+wa\n\r");
	uint8_t ret = wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WA);
	if(ret == WIFI_ANSWER_OK) {
		//while(wifi_command_recv_and_parse() == WIFI_ANSWER_NO_ANSWER);
		printf("after parse\n\r");
		wifi_refresh_status();

		wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NSTCP);
		PIO_Clear(&pins_wifi_spi[5]);
	} else {
		printf("answer: NOK\n\r");
		// TODO
	}

	wifi_command_flush();

	xTaskCreate(wifi_message_loop,
				(signed char *)"wif_ml",
				MESSAGE_LOOP_SIZE,
				NULL,
				1,
				(xTaskHandle *)NULL);

    logwifii("WIFI initialized\n\r");
    return true;
}

void wifi_refresh_status(void) {
	char data[100];
	wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NSTAT);

	while(true) {
		uint8_t length = wifi_command_recv(data, 100);
		if(wifi_command_parse(data, length) != WIFI_ANSWER_NO_ANSWER) {
			return;
		}

		data[length] = '\0';

		char *ptr;
		if((ptr = strcasestr(data, "MAC=")) != NULL) {
			ptr += strlen("MAC=");
			for(int8_t i = 5; i >= 0; i--) {
				wifi_status.mac_address[i] = wifi_data_hex_to_int(*ptr) << 4;
				ptr++;
				wifi_status.mac_address[i] |= wifi_data_hex_to_int(*ptr);
				ptr+=2;
			}

			logwifii("mac: %x:%x:%x:%x:%x:%x\n\r", wifi_status.mac_address[5], wifi_status.mac_address[4], wifi_status.mac_address[3], wifi_status.mac_address[2], wifi_status.mac_address[1], wifi_status.mac_address[0]);
		}

		if((ptr = strcasestr(data, "BSSID=")) != NULL) {
			ptr += strlen("BSSID=");
			for(int8_t i = 5; i >= 0; i--) {
				wifi_status.bssid[i] = wifi_data_hex_to_int(*ptr) << 4;
				ptr++;
				wifi_status.bssid[i] |= wifi_data_hex_to_int(*ptr);
				ptr+=2;
			}

			logwifii("bssid: %x:%x:%x:%x:%x:%x\n\r", wifi_status.bssid[5], wifi_status.bssid[4], wifi_status.bssid[3], wifi_status.bssid[2], wifi_status.bssid[1], wifi_status.bssid[0]);
		}

		if((ptr = strcasestr(data, "CHANNEL=")) != NULL) {
			ptr += strlen("CHANNEL=");
			wifi_status.channel = atoi(ptr);

			logwifii("channel: %d\n\r", wifi_status.channel);
		}

		if((ptr = strcasestr(data, "RSSI=")) != NULL) {
			ptr += strlen("RSSI=");
			wifi_status.rssi = atoi(ptr);

			logwifii("rssi: %d\n\r", wifi_status.rssi);
		}

		if((ptr = strcasestr(data, "IP addr=")) != NULL) {
			ptr += strlen("IP addr=");
			char *ptr_tmp = ptr;
			for(int8_t j = 3; j >= 0; j--) {
				for(uint8_t i = 0; i < 4; i++) {
					ptr_tmp++;
					if(*ptr_tmp == '.' || *ptr_tmp == ' ') {
						wifi_status.ip[j] = atoi(ptr);
						ptr = ptr_tmp+1;
						break;
					}
				}
			}

			logwifii("ip: %d.%d.%d.%d\n\r", wifi_status.ip[3], wifi_status.ip[2], wifi_status.ip[1], wifi_status.ip[0]);
		}

		if((ptr = strcasestr(data, "SubNet=")) != NULL) {
			ptr += strlen("SubNetr=");
			char *ptr_tmp = ptr;
			for(int8_t j = 3; j >= 0; j--) {
				for(uint8_t i = 0; i < 4; i++) {
					ptr_tmp++;
					if(*ptr_tmp == '.' || *ptr_tmp == ' ') {
						wifi_status.subnet_mask[j] = atoi(ptr);
						ptr = ptr_tmp+1;
						break;
					}
				}
			}

			logwifii("Subnet mask: %d.%d.%d.%d\n\r", wifi_status.subnet_mask[3], wifi_status.subnet_mask[2], wifi_status.subnet_mask[1], wifi_status.subnet_mask[0]);
		}

		if((ptr = strcasestr(data, "Gateway=")) != NULL) {
			ptr += strlen("Gateway=");
			char *ptr_tmp = ptr;
			for(int8_t j = 3; j >= 0; j--) {
				for(uint8_t i = 0; i < 4; i++) {
					ptr_tmp++;
					if(*ptr_tmp == '.' || *ptr_tmp == ' ') {
						wifi_status.gateway[j] = atoi(ptr);
						ptr = ptr_tmp+1;
						break;
					}
				}
			}

			logwifii("Gateway: %d.%d.%d.%d\n\r", wifi_status.gateway[3], wifi_status.gateway[2], wifi_status.gateway[1], wifi_status.gateway[0]);
		}

		if((ptr = strcasestr(data, "Rx Count=")) != NULL) {
			ptr += strlen("Rx Count=");
			wifi_status.rx_count = atoi(ptr);

			logwifii("rx_count: %d\n\r", wifi_status.rx_count);
		}

		if((ptr = strcasestr(data, "Tx Count=")) != NULL) {
			ptr += strlen("Tx Count=");
			wifi_status.tx_count = atoi(ptr);

			logwifii("tx_count: %d\n\r", wifi_status.tx_count);
		}
	}
}

void wifi_init_extension(uint8_t extension) {
	wifi_init();
}

uint16_t wifi_send(const void *data, const uint16_t length) {
	led_rxtx++;

	uint16_t send_length = MIN(length, WIFI_BUFFER_SIZE);

	wifi_data_send_escape(data, send_length);

	return send_length;
}

uint16_t wifi_recv(void *data, const uint16_t length) {
	if(wifi_buffer_size_recv == 0) {
		wifi_data_poll();
		return 0;
	}

	led_rxtx++;

	static uint16_t recv_pointer = 0;

	uint16_t recv_length = MIN(length, wifi_buffer_size_recv);

	memcpy(data, wifi_buffer_recv + recv_pointer, recv_length);

	if(wifi_buffer_size_recv - recv_length == 0) {
		recv_pointer = 0;
	} else {
		recv_pointer += recv_length;
	}

	wifi_buffer_size_recv -= recv_length;

	return recv_length;
}

void wifi_message_loop(void *parameters) {
	MessageLoopParameter mlp;
	mlp.buffer_size = WIFI_MAX_DATA_LENGTH;
	mlp.com_type    = COM_WIFI;
	mlp.return_func = wifi_message_loop_return;
	com_message_loop(&mlp);
}

void wifi_message_loop_return(char *data, uint16_t length) {
	const uint8_t stack_id = get_stack_id_from_data(data);

	if(stack_id == com_stack_id || stack_id == 0) {
		const ComMessage *com_message = get_com_from_data(data);
		if(com_message->reply_func != NULL) {
			com_message->reply_func(COM_WIFI, (void*)data);
		}

		return;
	}
	for(uint8_t i = 0; i < BRICKLET_NUM; i++) {
		if(bs[i].stack_id == stack_id) {
			baddr[i].entry(BRICKLET_TYPE_INVOCATION, COM_WIFI, (void*)data);
			return;
		}
	}

	if(stack_id <= com_last_spi_stack_id) {
		send_blocking_with_timeout(data, length, COM_SPI_STACK);
		return;
	}
}

void wifi_read_config(char *data, uint8_t length, uint8_t position) {
	uint8_t extension;
	if(com_ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_ext[1] == COM_WIFI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	uint8_t i;
	for(i = 0; i < length/32; i++) {
		extension_i2c_read(extension,
						   EXTENSION_POS_ANY + position + i*32,
						   data + i*32,
						   32);
	}

	uint8_t reminder = length - i*32;
	if(reminder != 0) {
		extension_i2c_read(extension,
						   EXTENSION_POS_ANY + position + i*32,
						   data + i*32,
						   reminder);
	}
}

void wifi_write_config(char *data, uint8_t length, uint8_t position) {
	uint8_t extension;
	if(com_ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_ext[1] == COM_WIFI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	uint8_t reminder = 32 - ((EXTENSION_POS_ANY + position) % 32);
	if(reminder != 32) {
		extension_i2c_write(extension,
						    EXTENSION_POS_ANY + position,
						    data,
						    reminder);
	} else {
		reminder = 0;
	}

	uint8_t i = 0;
	for(i = 0; i < (length - reminder)/32; i++) {
		extension_i2c_write(extension,
						    EXTENSION_POS_ANY + position + reminder + i*32,
						    data + reminder + i*32,
						    32);
	}

	uint8_t last = (length - reminder) - i*32;
	if(last != 0) {
		extension_i2c_write(extension,
						    EXTENSION_POS_ANY + position + reminder + i*32,
						    data + reminder + i*32,
						    last);
	}
}
