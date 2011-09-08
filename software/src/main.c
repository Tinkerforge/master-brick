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
#include "bricklib/utility/init.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/profiling.h"
#include "bricklib/utility/led.h"
#include "bricklib/logging/logging.h"

#include "config.h"
#include "communication.h"
#include "extensions/chibi/chibi.h"

extern uint8_t com_last_stack_address;

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	logf("Stack Overflow");
	while(true);
}

#define BLINK_CYCLE 1000000

void blinkenlights(const uint8_t length) {
    for(uint8_t j = 0; j < length; j++) {
		for(uint8_t i = LED_EXT_BLUE_0; i <= LED_EXT_BLUE_3; i++) {
			led_blink(i, BLINK_CYCLE);
		}

		for(uint8_t i = LED_EXT_BLUE_3; i >= LED_EXT_BLUE_0; i--) {
			led_blink(i, BLINK_CYCLE);
		}
    }
}

#include "extensions/chibi/chibi.h"
#include "extensions/chibi/chibi_slave.h"
#include "extensions/chibi/chibi_master.h"

int main() {
	Pin chibi_select_pin = PIN_CHIBI_SELECT;
	PIO_Clear(&chibi_select_pin);
    Pin pin_master_detect = PIN_MASTER_DETECT;
    Pin pin_detect        = PIN_DETECT;
    PIO_Configure(&pin_detect, 1);
    PIO_Configure(&pin_master_detect, 1);

	brick_init();

#ifdef PROFILING
    profiling_init();
#endif
/*
#if 1
    chibi_master_init();
    while(true) {
    	printf("sending...\n\r");
    	chibi_send("Hello Olaf\n\r", 14);
    }
#else
    chibi_slave_init();
    char data[50] = {0};
    while(true) {
    	printf("receiving...\n\r");
    	chibi_recv(data, 15);
    	printf("data: %s", data);
    }
#endif
*/

    // Here we have to wait for other potential masters in the stack to set
    // pin_detect, we can use this time to do some blinkenlights instead
    // of just nops!
    blinkenlights(1);

#ifdef PROFILING
    profiling_init();
#endif

    logsi("Bricklets initialized\n\r");
    master_init();

    if(PIO_Get(&pin_master_detect)) {
    	// If we are a Master in the Stack, we have to wait again, so
    	// other Bricks can enumerate there Bricklets
    	blinkenlights(4);
    	logsi("Configuring as Stack-Master\n\r");

        spi_stack_master_init();
    	logsi("SPI Stack for Master initialized\n\r");

        i2c_eeprom_master_init(TWI_STACK);

        master_create_routing_table();
        logsi("Master Routing table created\n\r");

    	usb_init();
    	logsi("USB initialized\n\r");

    	xTaskCreate(usb_message_loop,
    				(signed char *)"usb_ml",
    				2000,
    				NULL,
    				1,
    				(xTaskHandle *)NULL);

    	if(com_last_stack_address > 0) {
			xTaskCreate(spi_stack_master_state_machine_loop,
						(signed char *)"spi_sm",
						1000,
						NULL,
						1,
						(xTaskHandle *)NULL);

			xTaskCreate(spi_stack_master_message_loop,
						(signed char *)"spi_ml",
						1000,
						NULL,
						1,
						(xTaskHandle *)NULL);
    	}
    } else {
    	logsi("Configuring as Stack-Slave\n\r");
        spi_stack_slave_init();
        logsi("SPI Stack for Slave initialized\n\r");

    	xTaskCreate(spi_stack_slave_message_loop,
    			    (signed char *)"spi_ml",
    			    2500,
    			    NULL,
    			    1,
    			    (xTaskHandle *)NULL);
    }

	brick_init_start_tick_task();
	logsi("Starting Scheduler\n\r");
	vTaskStartScheduler();
}
