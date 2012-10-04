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
#include "wifi_brickd.h"
#include "wifi_low_level.h"

#include "bricklib/com/com.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/led.h"
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/pearson_hash.h"
#include "bricklib/utility/init.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/bricklet/bricklet_config.h"
#include "extensions/extension_init.h"
#include "extensions/extension_i2c.h"

extern ComType com_ext[];
extern uint32_t led_rxtx;
extern uint32_t led_ext3_rxtx;
extern ComType com_current;

extern uint8_t com_stack_id;
extern uint8_t com_last_spi_stack_id;

extern BrickletSettings bs[];
extern const BrickletAddress baddr[];

extern uint8_t eap_type;

int8_t wifi_new_cid = -1;

uint8_t wifi_power_mode = 0;
bool wifi_task_created = false;
uint8_t wifi_buffer_recv[WIFI_BUFFER_SIZE] = {0};
uint16_t wifi_buffer_size_recv = 0;

Pin pins_wifi_spi[] = {PINS_WIFI_SPI};

WifiConfiguration wifi_configuration = {
	"TinkerforgeWLAN",
	0,
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	4223,
	0,
	"3164429505265866",
	0,
	0,
	0,
	0,
	0,
	1
};

WifiStatus wifi_status = {
	{0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0},
	0,
	0,
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	0,
	0,
	WIFI_STATE_NO_STARTUP,
};

bool wifi_init(void) {
    pins_wifi_spi[WIFI_RESET].type = PIO_INPUT;
    pins_wifi_spi[WIFI_LED].type = PIO_OUTPUT_1;
    pins_wifi_spi[WIFI_DATA_RDY].type = PIO_INPUT;

    PIO_Configure(pins_wifi_spi, PIO_LISTSIZE(pins_wifi_spi));

    led_off(LED_EXT_BLUE_3);

    wifi_brickd_init();
    wifi_low_level_deselect();

    wifi_read_config((char *)&wifi_configuration, sizeof(WifiConfiguration), WIFI_CONFIGURATION_POS);

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

    uint32_t i = 0;
    for(i = 0; i < 500000; i++) {
    	if(PIO_Get(&pins_wifi_spi[WIFI_DATA_RDY])) {
    		break;
    	}
    }

    wifi_command_flush();

	pins_wifi_spi[WIFI_RESET].type = PIO_OUTPUT_0;
	PIO_Configure(&pins_wifi_spi[WIFI_RESET], 1);
	SLEEP_MS(10);
	pins_wifi_spi[WIFI_RESET].type = PIO_INPUT;
	PIO_Configure(&pins_wifi_spi[WIFI_RESET], 1);

	SLEEP_MS(1);
    while(!PIO_Get(&pins_wifi_spi[WIFI_DATA_RDY]));

    wifi_command_flush();

    SLEEP_MS(1);

    logwifid("AT+ATE0\n\r");
    wifi_command_send(WIFI_COMMAND_ID_AT_ATE0);
    SLEEP_MS(1);
    wifi_command_flush();

    logwifid("AT+ATV0\n\r");
    wifi_command_send(WIFI_COMMAND_ID_AT_ATV0);
    SLEEP_MS(1);
    wifi_command_flush();

    bool startup = true;
    logwifid("AT\n\r");
	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT) != WIFI_ANSWER_OK) {
		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
		startup = false;
	}

 /*   if(startup) {
    	logwifid("AT&F\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_F) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }*/

    if(startup) {
    	logwifid("AT+WD\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WD) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }

    if(startup) {
    	logwifid("AT+WRXPS=0\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXPS_OFF) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }

	// Wifi module always on (no sleep)
    if(startup) {
    	logwifid("AT+WRXACTIVE=1\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXACTIVE_ON) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }

    if(startup) {
    	logwifid("AT+WSYNCINTRL\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WSYNCINTRL) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }

    if(startup) {
    	logwifid("AT+WRETRY=7\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRETRY) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }

/*    if(startup) {
    	logwifid("ATS\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_ATS) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }*/

/*	if(startup) {
		logwifid("AT+PSPOLLINTRL=0\n\r");
		if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_PSPOLLINTRL) != WIFI_ANSWER_OK) {
			wifi_status.state = WIFI_STATE_STARTUP_ERROR;
			startup = false;
		}
	}*/

    if(startup) {
    	logwifid("AT+ASYNCMSGFMT=1\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_ASYNCMSGFMT) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }

    if(startup) {
    	logwifid("AT+WREGDOMAIN\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WREGDOMAIN) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }

	// Bulk mode data transfer
    if(startup) {
    	logwifid("AT+BDATA=1\n\r");
    	if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_BDATA_ON) != WIFI_ANSWER_OK) {
    		wifi_status.state = WIFI_STATE_STARTUP_ERROR;
    		startup = false;
    	}
    }


	if(wifi_configuration.encryption == ENCRYPTION_WPA) {
		if(startup) {
			logwifid("AT+WWPA\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WWPA) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
	} else if(wifi_configuration.encryption == ENCRYPTION_WPA_ENTERPRISE) {
		if(startup) {
			logwifid("AT+WEAPCONF\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WEAPCONF) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}

		if(startup) {
			logwifid("AT+WEAP\n\r");
			if(wifi_configuration.ca_certificate_length > 0) {
				eap_type = 0;
				if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WEAP) != WIFI_ANSWER_OK) {
					wifi_status.state = WIFI_STATE_STARTUP_ERROR;
					startup = false;
				} else {
					wifi_write_eap();
				}
			}

			if(wifi_configuration.client_certificate_length > 0) {
				eap_type = 1;
				if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WEAP) != WIFI_ANSWER_OK) {
					wifi_status.state = WIFI_STATE_STARTUP_ERROR;
					startup = false;
				} else {
					wifi_write_eap();
				}
			}

			if(wifi_configuration.private_key_length > 0) {
				eap_type = 2;
				if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WEAP) != WIFI_ANSWER_OK) {
					wifi_status.state = WIFI_STATE_STARTUP_ERROR;
					startup = false;
				} else {
					wifi_write_eap();
				}
			}
		}
	} else if(wifi_configuration.encryption == ENCRYPTION_WEP) {
		if(startup) {
			logwifid("AT+WAUTH (WEP)\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WAUTH_WEP) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
		if(startup) {
			logwifid("AT+WWEP\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WWEP) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
	} else if(wifi_configuration.encryption == ENCRYPTION_OPEN) {
		if(startup) {
			logwifid("AT+WSEC=1\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WSEC) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
	}

	if(wifi_configuration.connection == CONNECTION_DHCP ||
	   wifi_configuration.connection == CONNECTION_AP_DHCP ||
	   wifi_configuration.connection == CONNECTION_ADHOC_DHCP) {
		if(startup) {
			logwifid("AT+NDHCP=1\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NDHCP_ON) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
	} else {
		if(startup) {
			logwifid("AT+NSET\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NSET) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
	}

	if(wifi_configuration.connection == CONNECTION_AP_DHCP ||
	   wifi_configuration.connection == CONNECTION_ADHOC_DHCP) {
		if(startup) {
			logwifid("AT+WM=2\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WM_AP) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
	} else if(wifi_configuration.connection == CONNECTION_AP_STATIC_IP ||
	          wifi_configuration.connection == CONNECTION_ADHOC_STATIC_IP) {
		if(startup) {
			logwifid("AT+WM=1\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WM_ADHOC) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
	} else {
		if(startup) {
			logwifid("AT+WM=0\n\r");
			if(wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WM_IFACE) != WIFI_ANSWER_OK) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				startup = false;
			}
		}
	}

	// TODO: AT+WPAPSK to save recalculation

	if(startup) {
		logwifid("AT+WA\n\r");
		wifi_command_send(WIFI_COMMAND_ID_AT_WA);
		wifi_status.state = WIFI_STATE_ASSOCIATING;
	}

	if(!wifi_task_created) {
		wifi_task_created = true;
		xTaskCreate(wifi_message_loop,
					(signed char *)"wif_ml",
					MESSAGE_LOOP_SIZE,
					NULL,
					1,
					(xTaskHandle *)NULL);

		logwifii("WIFI initialized\n\r");
	}

    return true;
}

bool wifi_parse_ip(const char *data, const char *search_str, uint8_t *result) {
	char *ptr;
	if((ptr = strcasestr(data, search_str)) != NULL) {
		ptr += strlen(search_str);
		char *ptr_tmp = ptr;
		for(int8_t j = 3; j >= 0; j--) {
			for(uint8_t i = 0; i < 4; i++) {
				ptr_tmp++;
				if(!(*ptr_tmp >= '0' && *ptr_tmp <= '9')) {
					result[j] = atoi(ptr);
					ptr = ptr_tmp+1;
					break;
				}
			}
		}

		return true;
	}

	return false;
}

bool wifi_parse_mac(const char *data, const char *search_str, uint8_t *result) {
	char *ptr;
	if((ptr = strcasestr(data, search_str)) != NULL) {
		ptr += strlen(search_str);
		for(int8_t i = 5; i >= 0; i--) {
			result[i] = wifi_data_hex_to_int(*ptr) << 4;
			ptr++;
			result[i] |= wifi_data_hex_to_int(*ptr);
			ptr+=2;
		}

		return true;
	}

	return false;
}

void wifi_refresh_status(void) {
	if(wifi_status.state != WIFI_STATE_ASSOCIATED) {
		logwifii("Refresh status: not associated\n\r");
		return;
	}

	logwifii("Refresh status\n\r");
	char data[100];
	wifi_command_send(WIFI_COMMAND_ID_AT_NSTAT);

	while(true) {
		uint8_t length = 0;
		while(length == 0) {
			length = wifi_command_recv(data, 100, WIFI_COMMAND_RECV_TIMEOUT);
		}
		uint8_t ret = wifi_command_parse(data, length);
		if(ret == WIFI_ANSWER_OK || ret == WIFI_ANSWER_ERROR) {
			return;
		}

		data[length] = '\0';

		char *ptr;
		if(wifi_parse_mac(data, "MAC=", wifi_status.mac_address)) {
			logwifii("mac: %x:%x:%x:%x:%x:%x\n\r", wifi_status.mac_address[5], wifi_status.mac_address[4], wifi_status.mac_address[3], wifi_status.mac_address[2], wifi_status.mac_address[1], wifi_status.mac_address[0]);
		}

		if(wifi_parse_mac(data, "BSSID=", wifi_status.bssid)) {
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

		if(wifi_parse_ip(data, "IP addr=", wifi_status.ip)) {
			logwifii("ip: %d.%d.%d.%d\n\r", wifi_status.ip[3], wifi_status.ip[2], wifi_status.ip[1], wifi_status.ip[0]);
		}

		if(wifi_parse_ip(data, "SubNet=", wifi_status.subnet_mask)) {
			logwifii("Subnet mask: %d.%d.%d.%d\n\r", wifi_status.subnet_mask[3], wifi_status.subnet_mask[2], wifi_status.subnet_mask[1], wifi_status.subnet_mask[0]);
		}

		if(wifi_parse_ip(data, "Gateway=", wifi_status.gateway)) {
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
	if(wifi_status.state != WIFI_STATE_ASSOCIATED) {
		return 0;
	}

	led_rxtx++;
	led_ext3_rxtx++;

	uint16_t send_length = MIN(length, WIFI_BUFFER_SIZE);

	wifi_data_send_escape(data, send_length);

	return send_length;
}

uint16_t wifi_recv(void *data, const uint16_t length) {
	if(wifi_status.state != WIFI_STATE_ASSOCIATED) {
		return 0;
	}

	if(wifi_buffer_size_recv == 0) {
		wifi_data_poll();
		return 0;
	}

	led_rxtx++;
	led_ext3_rxtx++;

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

uint32_t wifi_read_key(void) {
	uint8_t extension;
	if(com_ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_ext[1] == COM_WIFI) {
		extension = 1;
	} else {
		// TODO: Error?
		return 0;
	}

	uint32_t key = 0;
	extension_i2c_read(extension,
					   WIFI_KEY_POS,
					   (char*)&key,
					   4);
	return key;
}

void wifi_write_key(void) {
	uint8_t extension;
	if(com_ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_ext[1] == COM_WIFI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	uint32_t key = WIFI_KEY;
	extension_i2c_write(extension,
					    WIFI_KEY_POS,
					    (char*)&key,
					    4);
}

void wifi_read_config(char *data, const uint8_t length, const uint16_t position) {
	uint8_t extension;
	if(com_ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_ext[1] == COM_WIFI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	if(wifi_read_key() != WIFI_KEY) {
		wifi_write_config(data, length, position);
		return;
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
}

void wifi_write_config(const char *data, const uint8_t length, const uint16_t position) {
	uint8_t extension;
	if(com_ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_ext[1] == COM_WIFI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	uint8_t reminder = 32 - (position % 32);
	if(reminder != 32) {
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

	wifi_write_key();
}

void wifi_tick(uint8_t tick_type) {
	static uint32_t wifi_counter = 0;

	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		return;
	}

	switch(wifi_status.state) {
		case WIFI_STATE_STARTUP_ERROR: {
			led_off(LED_EXT_BLUE_3);
			PIO_Set(&pins_wifi_spi[WIFI_LED]);
			wifi_counter++;
			if(wifi_counter >= 2000) {
				wifi_counter = 0;
				wifi_init();
			}
			break;
		}

		case WIFI_STATE_ASSOCIATING: {
			char data[100];

			wifi_counter++;
			// We wait 3 minutes before we assume that the GS1011 hang up
			if(wifi_counter == 60000*3) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				return;
			}

			const uint8_t wifi_counter_mod =  wifi_counter % 200;
			if(wifi_counter_mod == 0) {
				PIO_Clear(&pins_wifi_spi[WIFI_LED]);
			} else if(wifi_counter_mod == 100) {
				PIO_Set(&pins_wifi_spi[WIFI_LED]);
			}

			led_off(LED_EXT_BLUE_3);

			const uint8_t length = wifi_command_recv(data, 100, 500);
			uint8_t ret = wifi_command_parse(data, length);

			if(ret == WIFI_ANSWER_OK) {
				ret = wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_NSTCP);
				if(ret == WIFI_ANSWER_CONNECT) {
					ret = wifi_command_recv_and_parse();
					if(ret == WIFI_ANSWER_OK) {
						wifi_counter = 0;
						wifi_status.state = WIFI_STATE_ASSOCIATED;
						wifi_refresh_status();
					}
				}
			} else if(ret == WIFI_ANSWER_ERROR) {
				logwifid("Could not associate\n\r");
				wifi_status.state = WIFI_STATE_DISASSOCIATED;
				wifi_counter = 0;
			} else {
				if(ret != WIFI_ANSWER_NO_ANSWER && ret != WIFI_ANSWER_TIMEOUT) {
					wifi_command_send(WIFI_COMMAND_ID_AT_WA);
				}
			}
			break;
		}

		case WIFI_STATE_ASSOCIATED: {
			/*if(wifi_new_cid != -1) {
				wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_SETSOCKOPT_SO);
				wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_SETSOCKOPT_TC);
				wifi_new_cid = -1;
			}*/
			PIO_Clear(&pins_wifi_spi[WIFI_LED]);
			break;
		}

		case WIFI_STATE_DISASSOCIATED: {
			led_off(LED_EXT_BLUE_3);
			PIO_Set(&pins_wifi_spi[WIFI_LED]);

			wifi_status.bssid[0] = 0;
			wifi_status.bssid[1] = 0;
			wifi_status.bssid[2] = 0;
			wifi_status.bssid[3] = 0;
			wifi_status.bssid[4] = 0;
			wifi_status.bssid[5] = 0;
			wifi_status.channel = 0;
			wifi_status.gateway[0] = 0;
			wifi_status.gateway[1] = 0;
			wifi_status.gateway[2] = 0;
			wifi_status.gateway[3] = 0;
			wifi_status.ip[0] = 0;
			wifi_status.ip[1] = 0;
			wifi_status.ip[2] = 0;
			wifi_status.ip[3] = 0;
			wifi_status.subnet_mask[0] = 0;
			wifi_status.subnet_mask[1] = 0;
			wifi_status.subnet_mask[2] = 0;
			wifi_status.subnet_mask[3] = 0;
			wifi_status.rssi = 0;

			wifi_command_send(WIFI_COMMAND_ID_AT_WA);
			wifi_status.state = WIFI_STATE_ASSOCIATING;
			logwifid("State: Disassociated\n\r");
			break;
		}

		default: {
			break;
		}
	}

}

void wifi_set_power_mode(const uint8_t mode) {
	if(mode == WIFI_POWER_MODE_LOW_POWER) {
		wifi_power_mode = WIFI_POWER_MODE_LOW_POWER;
		wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXPS_ON);
		wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXACTIVE_OFF);
	} else {
		wifi_power_mode = WIFI_POWER_MODE_FULL_SPEED;
		wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXACTIVE_ON);
		wifi_command_send_recv_and_parse(WIFI_COMMAND_ID_AT_WRXPS_OFF);
	}
}
