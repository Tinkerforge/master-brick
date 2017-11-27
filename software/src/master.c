/* master-brick
 * Copyright (C) 2010-2015 Olaf Lüke <olaf@tinkerforge.com>
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
#include "bricklib/com/spi/spi_stack/spi_stack_master_dma.h"
#include "bricklib/com/spi/spi_stack/spi_stack_common.h"
#include "bricklib/com/spi/spi_stack/spi_stack_select.h"
#include "bricklib/drivers/adc/adc.h"
#include "bricklib/drivers/usb/USBD_HAL.h"
#include "bricklib/drivers/usart/usart.h"
#include "bricklib/logging/logging.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/utility/init.h"

#include "extensions/brickd.h"
#include "extensions/extension_i2c.h"
#include "extensions/chibi/chibi_master.h"
#include "extensions/chibi/chibi_slave.h"
#include "extensions/chibi/chibi_low_level.h"
#include "extensions/rs485/rs485_config.h"

#include "extensions/rs485/rs485_master.h"
#include "extensions/rs485/rs485_low_level.h"
#include "extensions/wifi/wifi.h"
#include "extensions/ethernet/ethernet.h"
#include "extensions/wifi2/wifi2.h"

#include "config.h"

extern ComInfo com_info;
uint8_t master_mode = MASTER_MODE_NONE;

extern ComInfo com_info;
extern uint8_t rs485_first_message;
extern uint8_t chibi_first_message;
extern uint32_t usb_num_send_tries;
extern bool usb_first_connection;

bool chibi_enumerate_ready = false;

MasterCallback master_callback = {
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_VALUE_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_OPTION_DEFAULT,
	MASTER_CALLBACK_MIN_DEFAULT,
	MASTER_CALLBACK_MAX_DEFAULT,
	MASTER_CALLBACK_OPTION_DEFAULT,
	MASTER_CALLBACK_MIN_DEFAULT,
	MASTER_CALLBACK_MAX_DEFAULT,
	MASTER_CALLBACK_OPTION_DEFAULT,
	MASTER_CALLBACK_MIN_DEFAULT,
	MASTER_CALLBACK_MAX_DEFAULT,
	MASTER_CALLBACK_OPTION_DEFAULT,
	MASTER_CALLBACK_MIN_DEFAULT,
	MASTER_CALLBACK_MAX_DEFAULT,
	MASTER_CALLBACK_OPTION_DEFAULT,
	MASTER_CALLBACK_MIN_DEFAULT,
	MASTER_CALLBACK_MAX_DEFAULT,
	MASTER_CALLBACK_OPTION_DEFAULT,
	MASTER_CALLBACK_MIN_DEFAULT,
	MASTER_CALLBACK_MAX_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_PERIOD_DEFAULT,
	MASTER_CALLBACK_DEBOUNCE_PERIOD_DEFAULT,
	MASTER_CALLBACK_COUNTER_DEFAULT
};

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

		const uint32_t calibrated_low_sum = (measured_low_sum*gain_mul)/gain_div;
		const int32_t offset = ((int32_t)(correct_low_sum - calibrated_low_sum))/ADC_CAL_SUM;
		adc_set_calibration(offset, gain_mul, gain_div);

		adc_channel_disable(ADC_CAL_LOW_CHANNEL);
		adc_channel_disable(ADC_CAL_HIGH_CHANNEL);
	}


	// Make random number from temperature
	uint32_t seed = 0;
	for(uint8_t i = 0; i < 32; i++) {
		SLEEP_MS(2);
		seed |= (adc_get_temperature() & 1) << i;
	}

	// Xorshift random number generator
	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 5;
	brickd_set_authentication_seed(seed);
}

uint8_t master_get_hardware_version(void) {
	Pin detect[] = {PIN_MASTER20_DETECT, PIN_MASTER21_DETECT};
	PIO_Configure(detect, PIO_LISTSIZE(detect));

	if(PIO_Get(&detect[0])) {
		return 10;
	} else if(PIO_Get(&detect[1])) {
		return 20;
    } else {
		return 21;
    }
}

void master_handle_callbacks(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		master_callback.sum_counter++;
		master_callback.stack_current_sum += adc_channel_get_data(STACK_CURRENT_CHANNEL);
		master_callback.stack_voltage_sum += adc_channel_get_data(STACK_VOLTAGE_CHANNEL);
		master_callback.usb_voltage_sum += adc_channel_get_data(USB_VOLTAGE_CHANNEL);
		if(master_callback.sum_counter == MASTER_CALLBACK_VALUE_AVERAGE) {
			master_callback.stack_current = (master_callback.stack_current_sum/MASTER_CALLBACK_VALUE_AVERAGE) * STACK_CURRENT_REFERENCE * STACK_CURRENT_MULTIPLIER / VOLTAGE_MAX_VALUE;
			master_callback.stack_voltage = (master_callback.stack_voltage_sum/MASTER_CALLBACK_VALUE_AVERAGE) * MASTER_STACK_VOLTAGE_MULT/MASTER_STACK_VOLTAGE_DIV;
			master_callback.usb_voltage = (master_callback.usb_voltage_sum/MASTER_CALLBACK_VALUE_AVERAGE) * USB_VOLTAGE_REFERENCE*USB_VOLTAGE_MULTIPLIER/(VOLTAGE_MAX_VALUE*USB_VOLTAGE_DIVISOR);

			master_callback.sum_counter = 0;
			master_callback.stack_current_sum = 0;
			master_callback.stack_voltage_sum = 0;
			master_callback.usb_voltage_sum = 0;
		}

		if(master_callback.period_stack_current_counter < UINT32_MAX) {
			master_callback.period_stack_current_counter++;
		}
		if(master_callback.period_stack_voltage_counter < UINT32_MAX) {
			master_callback.period_stack_voltage_counter++;
		}
		if(master_callback.period_usb_voltage_counter < UINT32_MAX) {
			master_callback.period_usb_voltage_counter++;
		}
		if(master_callback.threshold_period_current_stack_current != master_callback.debounce_period) {
			master_callback.threshold_period_current_stack_current++;
		}
		if(master_callback.threshold_period_current_stack_voltage != master_callback.debounce_period) {
			master_callback.threshold_period_current_stack_voltage++;
		}
		if(master_callback.threshold_period_current_usb_voltage != master_callback.debounce_period) {
			master_callback.threshold_period_current_usb_voltage++;
		}
	} else if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		// Handle period callbacks
		if(master_callback.period_stack_current != 0 &&
		   master_callback.period_stack_current <= master_callback.period_stack_current_counter) {
			if(master_callback.stack_current_last != master_callback.stack_current) {
				StackCurrent sc;
				com_make_default_header(&sc, com_info.uid, sizeof(StackCurrent), FID_STACK_CURRENT);
				sc.current = master_callback.stack_current;

				send_blocking_with_timeout(&sc,
				                           sizeof(StackCurrent),
				                           com_info.current);

				master_callback.period_stack_current_counter = 0;
				master_callback.stack_current_last = master_callback.stack_current;
			}
		}

		if(master_callback.period_stack_voltage != 0 &&
		   master_callback.period_stack_voltage <= master_callback.period_stack_voltage_counter) {
			if(master_callback.stack_voltage_last != master_callback.stack_voltage) {
				StackVoltage sv;
				com_make_default_header(&sv, com_info.uid, sizeof(StackVoltage), FID_STACK_VOLTAGE);
				sv.voltage = master_callback.stack_voltage;

				send_blocking_with_timeout(&sv,
				                           sizeof(StackVoltage),
				                           com_info.current);

				master_callback.period_stack_voltage_counter = 0;
				master_callback.stack_voltage_last = master_callback.stack_voltage;
			}
		}

		if(master_callback.period_usb_voltage != 0 &&
		   master_callback.period_usb_voltage <= master_callback.period_usb_voltage_counter) {
			if(master_callback.usb_voltage_last != master_callback.usb_voltage) {
				USBVoltage usb_voltage;
				com_make_default_header(&usb_voltage, com_info.uid, sizeof(USBVoltage), FID_USB_VOLTAGE);
				usb_voltage.voltage = master_callback.usb_voltage;

				send_blocking_with_timeout(&usb_voltage,
				                           sizeof(USBVoltage),
				                           com_info.current);

				master_callback.period_usb_voltage_counter = 0;
				master_callback.usb_voltage_last = master_callback.usb_voltage;
			}
		}

		// Handle threshold callbacks
		if(((master_callback.option_stack_current == 'o') &&
			((master_callback.stack_current < master_callback.min_stack_current) ||
			 (master_callback.stack_current > master_callback.max_stack_current))) ||
		   ((master_callback.option_stack_current == 'i') &&
			((master_callback.stack_current > master_callback.min_stack_current) &&
			 (master_callback.stack_current < master_callback.max_stack_current)))) {

			if(master_callback.threshold_period_current_stack_current == master_callback.debounce_period) {
				StackCurrentReached scr;
				com_make_default_header(&scr, com_info.uid, sizeof(StackCurrentReached), FID_STACK_CURRENT_REACHED);
				scr.current = master_callback.stack_current;

				send_blocking_with_timeout(&scr,
				                           sizeof(StackCurrentReached),
				                           com_info.current);
				master_callback.threshold_period_current_stack_current = 0;
			}
		}

		if(((master_callback.option_stack_voltage == 'o') &&
			((master_callback.stack_voltage < master_callback.min_stack_voltage) ||
			 (master_callback.stack_voltage > master_callback.max_stack_voltage))) ||
		   ((master_callback.option_stack_voltage == 'i') &&
			((master_callback.stack_voltage > master_callback.min_stack_voltage) &&
			 (master_callback.stack_voltage < master_callback.max_stack_voltage)))) {

			if(master_callback.threshold_period_current_stack_voltage == master_callback.debounce_period) {
				StackVoltageReached svr;
				com_make_default_header(&svr, com_info.uid, sizeof(StackVoltageReached), FID_STACK_VOLTAGE_REACHED);
				svr.voltage = master_callback.stack_voltage;

				send_blocking_with_timeout(&svr,
				                           sizeof(StackVoltageReached),
				                           com_info.current);
				master_callback.threshold_period_current_stack_voltage = 0;
			}
		}

		if(((master_callback.option_usb_voltage == 'o') &&
			((master_callback.usb_voltage < master_callback.min_usb_voltage) ||
			 (master_callback.usb_voltage > master_callback.max_usb_voltage))) ||
		   ((master_callback.option_usb_voltage == 'i') &&
			((master_callback.usb_voltage > master_callback.min_usb_voltage) &&
			 (master_callback.usb_voltage < master_callback.max_usb_voltage)))) {

			if(master_callback.threshold_period_current_usb_voltage == master_callback.debounce_period) {
				USBVoltageReached vr;
				com_make_default_header(&vr, com_info.uid, sizeof(USBVoltageReached), FID_USB_VOLTAGE_REACHED);
				vr.voltage = master_callback.usb_voltage;

				send_blocking_with_timeout(&vr,
				                           sizeof(USBVoltageReached),
				                           com_info.current);
				master_callback.threshold_period_current_usb_voltage = 0;
			}
		}
	}
}

void tick_task(const uint8_t tick_type) {
	master_handle_callbacks(tick_type);
	static uint8_t message_counter = 0;
	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		if(usb_first_connection && !usbd_hal_is_disabled(IN_EP)) {
			message_counter++;
			if(message_counter >= 100) {
				message_counter = 0;
				// Chances are that USB is not ready yet, we only try 1000 times
				usb_num_send_tries = 1000;
				if(brick_init_enumeration(COM_USB)) {
					usb_first_connection = false;
					com_info.current = COM_USB;
				}
				usb_num_send_tries = NUM_SEND_TRIES;
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

	if(com_info.ext[0] == COM_WIFI2 || com_info.ext[1] == COM_WIFI2) {
		wifi2_tick(tick_type);
	}
}
