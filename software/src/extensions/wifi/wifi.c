/* master-brick
 * Copyright (C) 2012-2013 Olaf Lüke <olaf@tinkerforge.com>
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
#include "bricklib/utility/init.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/bricklet/bricklet_config.h"
#include "extensions/extension_init.h"
#include "extensions/extension_i2c.h"
#include "extensions/brickd.h"

extern ComInfo com_info;
extern uint32_t led_rxtx;
extern uint32_t led_ext3_rxtx;

extern uint8_t eap_type;
extern WIFICommand wifi_command_parse_next;

int8_t wifi_new_cid = -1;

uint8_t wifi_power_mode = 0;
bool wifi_task_created = false;
uint8_t wifi_buffer_recv[WIFI_BUFFER_SIZE] = {0};
uint16_t wifi_buffer_size_recv = 0;

extern Pin extension_pins[];

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

uint8_t WIFI_CS = WIFI_CS_0;
uint8_t WIFI_RESET = WIFI_RESET_0;
uint8_t WIFI_LED = WIFI_LED_0;
uint8_t WIFI_DATA_RDY = WIFI_DATA_RDY_0;
WIFICommand wifi_init_state = WIFI_COMMAND_ID_NONE;

WIFICommand wifi_init_next_state_connection(void) {
	if(wifi_configuration.connection == CONNECTION_AP_DHCP ||
	   wifi_configuration.connection == CONNECTION_AP_STATIC_IP) {
		return WIFI_COMMAND_ID_AT_WM_AP;
	} else if(wifi_configuration.connection == CONNECTION_ADHOC_DHCP ||
	          wifi_configuration.connection == CONNECTION_ADHOC_STATIC_IP) {
		return WIFI_COMMAND_ID_AT_WM_ADHOC;
	} else {
		return WIFI_COMMAND_ID_AT_WM_IFACE;
	}
}

WIFICommand wifi_init_next_state_dhcp(void) {
	if(wifi_configuration.connection == CONNECTION_DHCP ||
	   wifi_configuration.connection == CONNECTION_AP_DHCP ||
	   wifi_configuration.connection == CONNECTION_ADHOC_DHCP) {
		return WIFI_COMMAND_ID_AT_NDHCP_ON;
	} else {
		return WIFI_COMMAND_ID_AT_NSET;
	}
}

bool wifi_init(void) {
	wifi_status.state = WIFI_STATE_ASSOCIATING;

	extension_pins[WIFI_CS].type = PIO_OUTPUT_0;
	extension_pins[WIFI_RESET].type = PIO_INPUT;
	extension_pins[WIFI_LED].type = PIO_OUTPUT_1;
	extension_pins[WIFI_DATA_RDY].type = PIO_INPUT;
	PIO_Configure(&extension_pins[WIFI_CS], 4);

	extension_pins[WIFI_MISO].type = PIO_PERIPH_A;
	extension_pins[WIFI_MOSI].type = PIO_PERIPH_A;
	extension_pins[WIFI_CLK].type = PIO_PERIPH_B;
	PIO_Configure(&extension_pins[WIFI_MISO], 3);

	led_off(LED_EXT_BLUE_3);

	brickd_init();
	wifi_low_level_deselect();

	if(!wifi_task_created) {
		wifi_read_config((char *)&wifi_configuration, sizeof(WifiConfiguration), WIFI_CONFIGURATION_POS);
	}

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
		if(PIO_Get(&extension_pins[WIFI_DATA_RDY])) {
			break;
		}
	}

	wifi_command_flush();

	extension_pins[WIFI_RESET].type = PIO_OUTPUT_0;
	PIO_Configure(&extension_pins[WIFI_RESET], 1);
	SLEEP_MS(10);
	extension_pins[WIFI_RESET].type = PIO_INPUT;
	PIO_Configure(&extension_pins[WIFI_RESET], 1);

	wifi_command_parse_next = WIFI_COMMAND_ID_RESET;

	SLEEP_MS(1);
	while(!PIO_Get(&extension_pins[WIFI_DATA_RDY]));


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

	wifi_init_state = WIFI_COMMAND_ID_AT_ATE0;
	return true;
}

void wifi_init_next(void) {
	static uint8_t counter = 0;
	switch(wifi_init_state) {
		case WIFI_COMMAND_ID_AT_ATE0: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_ATE0);
			wifi_init_state = WIFI_COMMAND_ID_AT_ATV0;
		    break;
		}

		case WIFI_COMMAND_ID_AT_ATV0: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_ATV0);
			wifi_init_state = WIFI_COMMAND_ID_AT_WD;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WD: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WD);
		    if(wifi_power_mode == WIFI_POWER_MODE_FULL_SPEED) {
		    	wifi_init_state = WIFI_COMMAND_ID_AT_WRXPS_OFF;
		    } else {
		    	wifi_init_state = WIFI_COMMAND_ID_AT_WRXPS_ON;
		    }
			break;
		}

		case WIFI_COMMAND_ID_AT_WRXPS_OFF: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WRXPS_OFF);
			wifi_init_state = WIFI_COMMAND_ID_AT_WRXACTIVE_ON;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WRXACTIVE_ON: {
			// Wifi module always on (no sleep)
		    wifi_command_send(WIFI_COMMAND_ID_AT_WRXACTIVE_ON);
			wifi_init_state = WIFI_COMMAND_ID_AT_WSYNCINTRL;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WRXPS_ON: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WRXPS_ON);
			wifi_init_state = WIFI_COMMAND_ID_AT_WRXACTIVE_OFF;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WRXACTIVE_OFF: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WRXACTIVE_OFF);
			wifi_init_state = WIFI_COMMAND_ID_AT_WSYNCINTRL;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WSYNCINTRL: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WSYNCINTRL);
			wifi_init_state = WIFI_COMMAND_ID_AT_WRETRY; //WIFI_COMMAND_ID_AT_ATS;
		    break;
		}

//		case WIFI_COMMAND_ID_AT_ATS: {
//		    wifi_command_send(WIFI_COMMAND_ID_AT_ATS);
//			wifi_init_state = WIFI_COMMAND_ID_AT_WRETRY; //WIFI_COMMAND_ID_AT_PSPOLLINTRL;
//		    break;
//		}

//      Use default poll intervall, otherwise the WIFI Extension dis- and reassociates
//      after a while if nothing is sent
//		case WIFI_COMMAND_ID_AT_PSPOLLINTRL: {
//		    wifi_command_send(WIFI_COMMAND_ID_AT_PSPOLLINTRL);
//			wifi_init_state = WIFI_COMMAND_ID_AT_WRETRY;
//		    break;
//		}

		case WIFI_COMMAND_ID_AT_WRETRY: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WRETRY);
			wifi_init_state = WIFI_COMMAND_ID_AT_ASYNCMSGFMT;
		    break;
		}

		case WIFI_COMMAND_ID_AT_ASYNCMSGFMT: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_ASYNCMSGFMT);
			wifi_init_state = WIFI_COMMAND_ID_AT_WREGDOMAIN;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WREGDOMAIN: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WREGDOMAIN);
			wifi_init_state = WIFI_COMMAND_ID_AT_BDATA_ON;
		    break;
		}

		case WIFI_COMMAND_ID_AT_BDATA_ON: {
			// Bulk mode data transfer
		    wifi_command_send(WIFI_COMMAND_ID_AT_BDATA_ON);

		    if(wifi_configuration.encryption == ENCRYPTION_WPA) {
		    	wifi_init_state = WIFI_COMMAND_ID_AT_WWPA;
		    } else if(wifi_configuration.encryption == ENCRYPTION_WPA_ENTERPRISE) {
		    	wifi_init_state = WIFI_COMMAND_ID_AT_WEAPCONF;
		    } else if(wifi_configuration.encryption == ENCRYPTION_WEP) {
		    	wifi_init_state = WIFI_COMMAND_ID_AT_WAUTH_WEP;
		    } else if(wifi_configuration.encryption == ENCRYPTION_OPEN) {
		    	wifi_init_state = WIFI_COMMAND_ID_AT_WSEC;
		    }
		    break;
		}

		case WIFI_COMMAND_ID_AT_WSEC: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WSEC);
			wifi_init_state = wifi_init_next_state_dhcp();
		    break;
		}

		case WIFI_COMMAND_ID_AT_WAUTH_WEP: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WAUTH_WEP);
			wifi_init_state = WIFI_COMMAND_ID_AT_WWEP;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WWEP: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WWEP);
			wifi_init_state = wifi_init_next_state_dhcp();
		    break;
		}

		case WIFI_COMMAND_ID_AT_WEAPCONF: {
			wifi_command_send(WIFI_COMMAND_ID_AT_WEAPCONF);
			wifi_init_state = WIFI_COMMAND_ID_AT_WEAP;
			break;
		}

		case WIFI_COMMAND_ID_AT_WEAP: {
			switch(counter) {
				case 0: {
					counter++;
					if(wifi_configuration.ca_certificate_length > 0) {
						eap_type = 0;
						wifi_command_send(WIFI_COMMAND_ID_AT_WEAP);
					} else {
						counter++;
					}
					break;
				}

				case 1: {
					wifi_write_eap();
					counter++;
					wifi_init_state = WIFI_COMMAND_ID_AT_WEAP;
					break;
				}

				case 2: {
					counter++;
					if(wifi_configuration.client_certificate_length > 0) {
						eap_type = 1;
						wifi_command_send(WIFI_COMMAND_ID_AT_WEAP);
					} else {
						counter++;
					}
					break;
				}

				case 3: {
					wifi_write_eap();
					counter++;
					wifi_init_state = WIFI_COMMAND_ID_AT_WEAP;
					break;
				}

				case 4: {
					counter++;
					if(wifi_configuration.private_key_length > 0) {
						eap_type = 2;
						wifi_command_send(WIFI_COMMAND_ID_AT_WEAP);
					} else {
						counter = 0;
						wifi_init_state = wifi_init_next_state_dhcp();
					}
					break;
				}

				case 5: {
					wifi_write_eap();
					counter = 0;
					wifi_init_state = wifi_init_next_state_dhcp();
					break;
				}
			}
			break;
		}

		case WIFI_COMMAND_ID_AT_WWPA: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WWPA);
		    wifi_init_state = wifi_init_next_state_dhcp();
		    break;
		}

		case WIFI_COMMAND_ID_AT_NDHCP_ON: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_NDHCP_ON);
			wifi_init_state = wifi_init_next_state_connection();
		    break;
		}

		case WIFI_COMMAND_ID_AT_NSET: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_NSET);
			wifi_init_state = wifi_init_next_state_connection();
		    break;
		}

		case WIFI_COMMAND_ID_AT_WM_AP: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WM_AP);
			wifi_init_state = WIFI_COMMAND_ID_AT_WA;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WM_ADHOC: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WM_ADHOC);
			wifi_init_state = WIFI_COMMAND_ID_AT_WA;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WM_IFACE: {
		    wifi_command_send(WIFI_COMMAND_ID_AT_WM_IFACE);
			wifi_init_state = WIFI_COMMAND_ID_AT_WA;
		    break;
		}

		case WIFI_COMMAND_ID_AT_WA: {
			wifi_command_send(WIFI_COMMAND_ID_AT_WA);
			wifi_init_state = WIFI_COMMAND_ID_AT_NSTCP;
		    break;
		}

		case WIFI_COMMAND_ID_AT_NSTCP: {
			wifi_command_send(WIFI_COMMAND_ID_AT_NSTCP);
			wifi_init_state = WIFI_COMMAND_ID_NONE;
		    break;
		}

		default: {
			break;
		}
	}
}

void wifi_init_extension(const uint8_t extension) {
	if(extension == 0) {
		WIFI_CS = WIFI_CS_0;
		WIFI_RESET = WIFI_RESET_0;
		WIFI_LED = WIFI_LED_0;
		WIFI_DATA_RDY = WIFI_DATA_RDY_0;
	} else if(extension == 1) {
		WIFI_CS = WIFI_CS_1;
		WIFI_RESET = WIFI_RESET_1;
		WIFI_LED = WIFI_LED_1;
		WIFI_DATA_RDY = WIFI_DATA_RDY_1;
	}

	wifi_init();
}

uint16_t wifi_send(const void *data, const uint16_t length, uint32_t *options) {
	if(wifi_status.state != WIFI_STATE_ASSOCIATED) {
		return 0;
	}

	led_rxtx++;
	led_ext3_rxtx++;

	uint16_t send_length = MIN(length, WIFI_BUFFER_SIZE);

	wifi_data_send_escape(data, send_length);

	return send_length;
}

uint16_t wifi_recv(void *data, const uint16_t length, uint32_t *options) {
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

void wifi_message_loop_return(const char *data, const uint16_t length) {
	com_route_message_from_pc(data, length, COM_WIFI);
}

uint32_t wifi_read_key(void) {
	uint8_t extension;
	if(com_info.ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_WIFI) {
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
	if(com_info.ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_WIFI) {
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
	if(com_info.ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_WIFI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	if(wifi_read_key() != WIFI_KEY) {
		logwifiw("Key mismatch\n\r");
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
	if(com_info.ext[0] == COM_WIFI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_WIFI) {
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

	wifi_write_key();
}

void wifi_tick(const uint8_t tick_type) {
	static uint32_t wifi_counter = 0;

	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		return;
	}

	switch(wifi_status.state) {
		case WIFI_STATE_STARTUP_ERROR: {
			led_off(LED_EXT_BLUE_3);
			PIO_Set(&extension_pins[WIFI_LED]);
			wifi_counter++;
			if(wifi_counter >= 2000) {
				wifi_counter = 0;
				wifi_init_state = WIFI_COMMAND_ID_NONE;
				logwifid("Startup Error: Trying again...\n\r");
				wifi_init();
			}
			break;
		}

		case WIFI_STATE_ASSOCIATING: {
			wifi_counter++;
			// We wait 5 minutes before we assume that the GS1011 hang up
			if(wifi_counter == 60000*5) {
				wifi_status.state = WIFI_STATE_STARTUP_ERROR;
				return;
			}

			if(wifi_command_parse_next == WIFI_COMMAND_ID_NONE) {
				// We are done with initialization
				if(wifi_init_state == WIFI_COMMAND_ID_NONE ) {
					wifi_counter = 0;
					wifi_status.state = WIFI_STATE_ASSOCIATED;
					logwifii("Initialization successful\n\r");
					wifi_command_send(WIFI_COMMAND_ID_AT_NSTAT);
					return;
				}

				wifi_init_next();
			}

			const uint8_t wifi_counter_mod =  wifi_counter % 200;
			if(wifi_counter_mod == 0) {
				PIO_Clear(&extension_pins[WIFI_LED]);
			} else if(wifi_counter_mod == 100) {
				PIO_Set(&extension_pins[WIFI_LED]);
			}

			led_off(LED_EXT_BLUE_3);
			break;
		}

		case WIFI_STATE_ASSOCIATED: {
			if(wifi_new_cid != -1) {
				brick_init_enumeration(COM_WIFI);
				wifi_new_cid = -1;
			}
			PIO_Clear(&extension_pins[WIFI_LED]);
			break;
		}

		case WIFI_STATE_DISASSOCIATED: {
			led_off(LED_EXT_BLUE_3);
			PIO_Set(&extension_pins[WIFI_LED]);

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


			// Reset the WIFI Module after disassociation
			wifi_init_state = WIFI_COMMAND_ID_NONE;
			wifi_init();
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
		wifi_command_send(WIFI_COMMAND_ID_AT_WRXPS_ON);
		wifi_command_send(WIFI_COMMAND_ID_AT_WRXACTIVE_OFF);
	} else {
		wifi_power_mode = WIFI_POWER_MODE_FULL_SPEED;
		wifi_command_send(WIFI_COMMAND_ID_AT_WRXACTIVE_ON);
		wifi_command_send(WIFI_COMMAND_ID_AT_WRXPS_OFF);
	}
}
