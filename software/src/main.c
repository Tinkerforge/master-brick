/* master-brick
 * Copyright (C) 2010-2013 Olaf Lüke <olaf@tinkerforge.com>
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
#include <string.h>
#include "bricklib/free_rtos/include/FreeRTOS.h"
#include "bricklib/free_rtos/include/task.h"

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
#include "bricklib/drivers/pio/pio.h"
#include "bricklib/drivers/adc/adc.h"
#include "bricklib/drivers/wdt/wdt.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/utility/init.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/profiling.h"
#include "bricklib/utility/led.h"
#include "bricklib/logging/logging.h"

#include "config.h"
#include "master.h"
#include "routing.h"
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

extern ComInfo com_info;
extern uint8_t master_mode;
extern bool usb_first_connection;

uint8_t brick_hardware_version[3];

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	logf("Stack Overflow: %s\n\r", pcTaskName);
	led_on(LED_STD_RED);
	while(true);
}

#define BLINK_DELAY 40

void blinkenlights(const uint8_t length) {
    for(uint8_t j = 0; j < length; j++) {
    	wdt_restart();
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

    Pin pins_extension[] = {PINS_EXT};
    PIO_Configure(pins_extension, PIO_LISTSIZE(pins_extension));

    if(master_get_hardware_version() == 10) {
    	brick_hardware_version[0] = BRICK_HARDWARE_VERSION_MAJOR_10;
    	brick_hardware_version[1] = BRICK_HARDWARE_VERSION_MINOR_10;
    	brick_hardware_version[2] = BRICK_HARDWARE_VERSION_REVISION_10;
    } else {
    	// Set dummy calibration, to make sure that calibration is not read
    	// from flash in Master Brick HW Version 2.0
    	adc_set_calibration(0, 1, 1);
    	brick_hardware_version[0] = BRICK_HARDWARE_VERSION_MAJOR_20;
    	brick_hardware_version[1] = BRICK_HARDWARE_VERSION_MINOR_20;
    	brick_hardware_version[2] = BRICK_HARDWARE_VERSION_REVISION_20;
    }

	brick_init();
	wdt_restart();

#ifdef PROFILING
    profiling_init();
#endif

    // Here we have to wait for other potential masters in the stack to set
    // pin_detect, we can use this time to do some blinkenlights instead
    // of just nops!
    blinkenlights(1);

    led_on(LED_EXT_BLUE_0);
    led_on(LED_EXT_BLUE_1);
    led_on(LED_EXT_BLUE_2);
    led_on(LED_EXT_BLUE_3);
    master_init();
    led_off(LED_EXT_BLUE_0);
    led_off(LED_EXT_BLUE_1);
    led_off(LED_EXT_BLUE_2);
    led_off(LED_EXT_BLUE_3);

    wdt_restart();
    Pin pin_3v3_enable = PIN_3V3_ENABLE;
    if(PIO_Get(&pin_master_detect)) {
    	pin_3v3_enable.type = PIO_OUTPUT_1;
    	PIO_Configure(&pin_3v3_enable, 1);
		PIO_Configure(twi_stack_pullup_master_pins, PIO_LISTSIZE(twi_stack_pullup_master_pins));

    	master_mode |= MASTER_MODE_MASTER;

    	// If we are a Master in the Stack, we have to wait again, so
    	// other Bricks can enumerate there Bricklets
    	blinkenlights(5);
    	logsi("Configuring as Stack-Master\n\r");

        spi_stack_master_init();
        wdt_restart();
    	logsi("SPI Stack for Master initialized\n\r");

        routing_table_create_stack();
        wdt_restart();
        logsi("Master Routing table created\n\r");

        extension_init();

    	if(usb_init()) {
			xTaskCreate(usb_message_loop,
						(signed char *)"usb_ml",
						MESSAGE_LOOP_SIZE,
						NULL,
						1,
						(xTaskHandle *)NULL);

			logsi("USB initialized\n\r");
    	} else {
    		usb_first_connection = false;
    		logsi("No USB connection\n\r");
    	}

    	if(com_info.last_stack_address > 0) {
#if !USE_SPI_DMA
			xTaskCreate(spi_stack_master_state_machine_loop,
						(signed char *)"spi_sm",
						500,
						NULL,
						1,
						(xTaskHandle *)NULL);
#endif

			xTaskCreate(spi_stack_master_message_loop,
						(signed char *)"spi_ml",
						MESSAGE_LOOP_SIZE,
						NULL,
						1,
						(xTaskHandle *)NULL);
    	}
    } else {
    	usb_first_connection = false;
    	pin_3v3_enable.type = PIO_OUTPUT_0;
    	PIO_Configure(&pin_3v3_enable, 1);
    	PIO_Configure(twi_stack_pullup_slave_pins,
    	              PIO_LISTSIZE(twi_stack_pullup_slave_pins));
    	master_mode |= MASTER_MODE_SLAVE;
    	logsi("Configuring as Stack-Slave\n\r");
        spi_stack_slave_init();
        wdt_restart();
        logsi("SPI Stack for Slave initialized\n\r");

    	xTaskCreate(spi_stack_slave_message_loop,
    			    (signed char *)"spi_ml",
    			    MESSAGE_LOOP_SIZE,
    			    NULL,
    			    1,
    			    (xTaskHandle *)NULL);
    }

	brick_init_start_tick_task();
	wdt_restart();
	logsi("Starting Scheduler\n\r");

	vTaskStartScheduler();
}
