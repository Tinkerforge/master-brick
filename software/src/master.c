/* master-brick
 * Copyright (C) 2010-2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * master.c: Master specific functions
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

#include "master.h"

#include <stdio.h>

#include "bricklib/bricklet/bricklet_init.h"
#include "bricklib/com/usb/usb.h"
#include "bricklib/com/spi/spi_stack/spi_stack_master.h"
#include "bricklib/com/spi/spi_stack/spi_stack_common.h"
#include "bricklib/com/spi/spi_stack/spi_stack_select.h"
#include "bricklib/drivers/adc/adc.h"
#include "bricklib/drivers/usb/USBD_HAL.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/logging/logging.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/init.h"

#include "extensions/extension_i2c.h"
#include "extensions/chibi/chibi_master.h"
#include "extensions/chibi/chibi_slave.h"
#include "extensions/chibi/chibi_low_level.h"
#include "extensions/rs485/rs485_config.h"
#include "extensions/rs485/rs485_master.h"
#include "extensions/rs485/rs485_low_level.h"
#include "extensions/wifi/wifi.h"
#include "extensions/ethernet/ethernet.h"

#include "config.h"

extern ComInfo com_info;

uint8_t master_mode = MASTER_MODE_NONE;

extern ComInfo com_info;
extern uint8_t rs485_first_message;
extern uint8_t chibi_first_message;

bool chibi_enumerate_ready = false;

uint8_t master_startup_usb_connected = false;

uint16_t master_usb_voltage = 0;
uint16_t master_stack_voltage = 0;
uint16_t master_stack_current = 0;

uint8_t master_restart_counter = 0;

extern bool usb_first_connection;

void master_init(void) {
	Pin power_pins[] = {PIN_STACK_VOLTAGE, PIN_STACK_CURRENT};
	PIO_Configure(power_pins, PIO_LISTSIZE(power_pins));

	uint8_t version = master_get_hardware_version();
	adc_channel_enable(USB_VOLTAGE_CHANNEL);
	adc_channel_enable(STACK_VOLTAGE_CHANNEL);
	adc_channel_enable(STACK_CURRENT_CHANNEL);
	if(version > 10) {
		adc_channel_enable(ADC_CAL_LOW_CHANNEL);
		adc_channel_enable(ADC_CAL_HIGH_CHANNEL);
	}

	extension_i2c_clear_eeproms();

	const int32_t correct_low_sum = (ADC_MAX_VALUE*ADC_CAL_SUM*ADC_CAL_LOW_MULTIPLIER)/ADC_CAL_LOW_DIVISOR;
	const int32_t correct_high_sum = (ADC_MAX_VALUE*ADC_CAL_SUM*ADC_CAL_HIGH_MULTIPLIER)/ADC_CAL_HIGH_DIVISOR;

	if(version > 10) {
		uint32_t measured_low_sum = 0;
		uint32_t measured_high_sum = 0;
		for(uint8_t i = 0; i < ADC_CAL_SUM*10; i++) {
			SLEEP_MS(1);
			measured_low_sum += adc_channel_get_data_unfiltered(ADC_CAL_LOW_CHANNEL);
			measured_high_sum += adc_channel_get_data_unfiltered(ADC_CAL_HIGH_CHANNEL);
		}

		measured_low_sum /= 10;
		measured_high_sum /= 10;

		const uint32_t gain_mul = correct_high_sum - correct_low_sum;
		const uint32_t gain_div = measured_high_sum - measured_low_sum;
		const int32_t offset = -((uint32_t)((uint32_t)measured_low_sum*(uint32_t)gain_mul)/gain_div) + correct_low_sum;
		adc_set_calibration(offset, gain_mul, gain_div);

		adc_channel_disable(ADC_CAL_LOW_CHANNEL);
		adc_channel_disable(ADC_CAL_HIGH_CHANNEL);
	}
}

uint8_t master_get_hardware_version(void) {
	Pin detect = PIN_MASTER20_DETECT;
	PIO_Configure(&detect, 1);
	if(PIO_Get(&detect)) {
		return 10;
	} else {
		return 20;
	}
}

void tick_task(const uint8_t tick_type) {
	static uint8_t message_counter = 0;
	static uint8_t sum_counter = 0;
	static uint32_t usb_voltage_sum = 0;
	static uint32_t stack_voltage_sum = 0;
	static uint32_t stack_current_sum = 0;
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		sum_counter++;
		usb_voltage_sum += adc_channel_get_data(USB_VOLTAGE_CHANNEL);
		stack_voltage_sum += adc_channel_get_data(STACK_VOLTAGE_CHANNEL);
		stack_current_sum += adc_channel_get_data(STACK_CURRENT_CHANNEL);
		if(sum_counter == 100) {
			master_usb_voltage = usb_voltage_sum/100;
			master_stack_voltage = stack_voltage_sum/100;
			master_stack_current = stack_current_sum/100;

			sum_counter = 0;
			usb_voltage_sum = 0;
			stack_voltage_sum = 0;
			stack_current_sum = 0;
		}

		if(master_startup_usb_connected ^ usb_is_connected()) {
			master_restart_counter++;
			if(master_restart_counter == 100) {
				usb_isr_vbus(NULL);
			}
		} else {
			master_restart_counter = 0;
		}
	} else if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		if(usb_first_connection && !usbd_hal_is_disabled(IN_EP)) {
			message_counter++;
			if(message_counter >= 100) {
				message_counter = 0;
				if(brick_init_enumeration(COM_USB)) {
					usb_first_connection = false;
					com_info.current = COM_USB;
				}
			}
		}
		if(rs485_first_message == 1) {
			if((com_info.ext_type[0] == COM_TYPE_SLAVE && com_info.ext[0] == COM_RS485) ||
			   (com_info.ext_type[1] == COM_TYPE_SLAVE && com_info.ext[1] == COM_RS485)) {
				if(brick_init_enumeration(COM_RS485)) {
					rs485_first_message = 2;
					com_info.current = COM_RS485;
				}
			}
		}
		if(chibi_first_message == 1) {
			if((com_info.ext_type[0] == COM_TYPE_SLAVE && com_info.ext[0] == COM_CHIBI) ||
			   (com_info.ext_type[1] == COM_TYPE_SLAVE && com_info.ext[1] == COM_CHIBI)) {
				if(brick_init_enumeration(COM_CHIBI)) {
					chibi_first_message = 2;
					com_info.current = COM_CHIBI;
				}
			}
		}
	}

	if(com_info.ext[0] == COM_WIFI || com_info.ext[1] == COM_WIFI) {
		wifi_tick(tick_type);
	}
	if(com_info.ext[0] == COM_ETHERNET || com_info.ext[1] == COM_ETHERNET) {
		ethernet_tick(tick_type);
	}
}
