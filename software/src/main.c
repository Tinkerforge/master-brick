/* master-brick
 * Copyright (C) 2010-2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * main.c: Master-Brick startup code
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pio/pio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "bricklib/com/i2c/i2c_pca9549/i2c_pca9549.h"
#include "bricklib/com/spi/spi_stack/spi_stack_master.h"
#include "bricklib/com/spi/spi_stack/spi_stack_slave.h"
#include "bricklib/com/spi/spi_stack/spi_stack_common.h"
#include "bricklib/com/i2c/i2c_eeprom/i2c_eeprom_master.h"
#include "bricklib/com/i2c/i2c_eeprom/i2c_eeprom_slave.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/logging/logging.h"
#include "bricklib/bricklet/bricklet_init.h"
#include "bricklib/drivers/uid/uid.h"
#include "bricklib/drivers/adc/adc.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/init.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/profiling.h"
#include "bricklib/utility/led.h"
#include "bricklib/logging/logging.h"

#include "config.h"
#include "master.h"
#include "communication.h"
#include "extensions/chibi/chibi.h"
#include "extensions/rs485/rs485.h"
#include "extensions/rs485/rs485_config.h"
#include "extensions/rs485/rs485_slave.h"
#include "extensions/rs485/rs485_master.h"
#include "extensions/wifi/wifi.h"
#include "extensions/wifi/wifi_config.h"
#include "extensions/wifi/wifi_command.h"
#include "extensions/wifi/wifi_data.h"
#include "extensions/extension_init.h"

extern uint8_t com_last_stack_address;
extern uint8_t master_mode;

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	logf("Stack Overflow\n\r");
	while(true);
}

#define BLINK_DELAY 40

void blinkenlights(const uint8_t length) {
    for(uint8_t j = 0; j < length; j++) {
		for(uint8_t i = LED_EXT_BLUE_0; i <= LED_EXT_BLUE_3; i++) {
			led_blink(i, BLINK_DELAY);
		}

		for(uint8_t i = LED_EXT_BLUE_3; i >= LED_EXT_BLUE_0; i--) {
			led_blink(i, BLINK_DELAY);
		}
    }
}

int main() {
	Pin twi_stack_pullup_master_pins[] = {PINS_TWI_PULLUP_MASTER};
	Pin twi_stack_pullup_slave_pins[] = {PINS_TWI_PULLUP_SLAVE};
    Pin pin_master_detect = PIN_MASTER_DETECT;
    Pin pin_detect        = PIN_DETECT;
    PIO_Configure(&pin_detect, 1);
    PIO_Configure(&pin_master_detect, 1);

	brick_init();

#ifdef PROFILING
    profiling_init();
#endif

    // Here we have to wait for other potential masters in the stack to set
    // pin_detect, we can use this time to do some blinkenlights instead
    // of just nops!
    blinkenlights(1);

    logsi("Bricklets initialized\n\r");
    master_init();

    if(PIO_Get(&pin_master_detect)) {
		PIO_Configure(twi_stack_pullup_master_pins, PIO_LISTSIZE(twi_stack_pullup_master_pins));

    	master_mode |= MASTER_MODE_MASTER;

    	// If we are a Master in the Stack, we have to wait again, so
    	// other Bricks can enumerate there Bricklets
    	blinkenlights(5);
    	logsi("Configuring as Stack-Master\n\r");

        spi_stack_master_init();
    	logsi("SPI Stack for Master initialized\n\r");

        master_create_routing_table_stack();
        logsi("Master Routing table created\n\r");

        extension_init();
    	if(usb_init()) {
    		master_create_routing_table_extensions();

			xTaskCreate(usb_message_loop,
						(signed char *)"usb_ml",
						MESSAGE_LOOP_SIZE,
						NULL,
						1,
						(xTaskHandle *)NULL);

			logsi("USB initialized\n\r");
    	} else {
    		logsi("No USB connection\n\r");
    	}

    	if(com_last_stack_address > 0) {
			xTaskCreate(spi_stack_master_state_machine_loop,
						(signed char *)"spi_sm",
						500,
						NULL,
						1,
						(xTaskHandle *)NULL);

			xTaskCreate(spi_stack_master_message_loop,
						(signed char *)"spi_ml",
						MESSAGE_LOOP_SIZE,
						NULL,
						1,
						(xTaskHandle *)NULL);
    	}
    } else {
    	PIO_Configure(twi_stack_pullup_slave_pins,
    	              PIO_LISTSIZE(twi_stack_pullup_slave_pins));
    	master_mode |= MASTER_MODE_SLAVE;
    	logsi("Configuring as Stack-Slave\n\r");
        spi_stack_slave_init();
        logsi("SPI Stack for Slave initialized\n\r");

    	xTaskCreate(spi_stack_slave_message_loop,
    			    (signed char *)"spi_ml",
    			    MESSAGE_LOOP_SIZE,
    			    NULL,
    			    1,
    			    (xTaskHandle *)NULL);
    }

	brick_init_start_tick_task();
	logsi("Starting Scheduler\n\r");

	Pin pins_wifi_spi[] = {PINS_WIFI_SPI};
    pins_wifi_spi[WIFI_DATA_RDY].type = PIO_INPUT;

    PIO_Configure(&pins_wifi_spi[WIFI_DATA_RDY], 1);

	vTaskStartScheduler();
}
