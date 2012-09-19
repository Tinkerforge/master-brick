/* master-brick
 * Copyright (C) 2010-2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: Implementation of Master-Brick specific messages
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

#include "communication.h"

#include <string.h>

#include "bricklib/drivers/adc/adc.h"
#include "bricklib/com/com_common.h"
#include "extensions/chibi/chibi_config.h"
#include "extensions/chibi/chibi_low_level.h"
#include "extensions/rs485/rs485_config.h"
#include "extensions/wifi/wifi_config.h"
#include "extensions/wifi/wifi.h"
#include "extensions/wifi/wifi_data.h"
#include "extensions/wifi/wifi_command.h"
#include "extensions/extension_i2c.h"
#include "extensions/extension_init.h"

extern ComType com_ext[];
extern uint8_t chibi_receiver_input_power;
extern uint16_t chibi_underrun;
extern uint16_t chibi_crc_error;
extern uint16_t chibi_no_ack;
extern uint16_t chibi_overflow;
extern uint8_t chibi_frequency_mode;
extern uint8_t chibi_channel;

extern uint16_t rs485_error_crc;
extern uint32_t rs485_config_speed;
extern char rs485_config_parity;
extern uint8_t rs485_config_stopbits;

extern WifiStatus wifi_status;
extern WifiConfiguration wifi_configuration;
extern uint8_t wifi_power_mode;
extern uint8_t eap_type;

extern uint32_t wifi_data_ringbuffer_overflow;
extern uint16_t wifi_data_ringbuffer_low_watermark;

void get_stack_voltage(uint8_t com, const GetStackVoltage *data) {
	GetStackVoltageReturn gsvr;

	gsvr.stack_id      = data->stack_id;
	gsvr.type          = data->type;
	gsvr.length        = sizeof(GetStackVoltageReturn);

	gsvr.voltage = adc_channel_get_data(STACK_VOLTAGE_CHANNEL) *
	               STACK_VOLTAGE_REFERENCE *
	               STACK_VOLTAGE_MULTIPLIER /
	               VOLTAGE_MAX_VALUE;
	if(gsvr.voltage < VOLTAGE_EPSILON) {
		gsvr.voltage = 0;
	}

	send_blocking_with_timeout(&gsvr, sizeof(GetStackVoltageReturn), com);
	logmasteri("get_stack_voltage: %d\n\r", gsvr.voltage);
}

void get_stack_current(uint8_t com, const GetStackCurrent *data) {
	GetStackCurrentReturn gscr;

	gscr.stack_id      = data->stack_id;
	gscr.type          = data->type;
	gscr.length        = sizeof(GetStackCurrentReturn);

	uint16_t voltage = adc_channel_get_data(STACK_VOLTAGE_CHANNEL) *
	                   STACK_VOLTAGE_REFERENCE *
	                   STACK_VOLTAGE_MULTIPLIER /
	                   VOLTAGE_MAX_VALUE;

	if(voltage < VOLTAGE_EPSILON) {
		gscr.current = 0;
	} else {
		gscr.current = adc_channel_get_data(STACK_CURRENT_CHANNEL) *
		               STACK_CURRENT_REFERENCE *
		               STACK_CURRENT_MULTIPLIER /
		               VOLTAGE_MAX_VALUE;
	}

	send_blocking_with_timeout(&gscr, sizeof(GetStackCurrentReturn), com);
	logmasteri("get_stack_current: %d\n\r", gscr.current);
}

void set_extension_type(uint8_t com, const SetExtensionType *data) {
	extension_set_type(data->extension, data->exttype);
	logmasteri("set_extension_type: %d\n\r", data->exttype);
}

void get_extension_type(uint8_t com, const GetExtensionType *data) {
	GetExtensionTypeReturn getr;

	getr.stack_id      = data->stack_id;
	getr.type          = data->type;
	getr.length        = sizeof(GetExtensionTypeReturn);
	getr.exttype       = extension_get_type(data->extension);

	send_blocking_with_timeout(&getr, sizeof(GetExtensionTypeReturn), com);
	logmasteri("get_extension_type: %d\n\r", getr.exttype);
}

void is_chibi_present(uint8_t com, const IsChibiPresent *data) {
	IsChibiPresentReturn icpr;

	icpr.stack_id      = data->stack_id;
	icpr.type          = data->type;
	icpr.length        = sizeof(IsChibiPresentReturn);
	icpr.present       = com_ext[0] == COM_CHIBI || com_ext[1] == COM_CHIBI;

	send_blocking_with_timeout(&icpr, sizeof(IsChibiPresentReturn), com);
	logchibii("is_chibi_present: %d\n\r", icpr.present);
}

void set_chibi_address(uint8_t com, const SetChibiAddress *data) {
	uint8_t extension;
	if(com_ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	extension_set_address(extension, data->address);
	logchibii("set_chibi_address: %d\n\r", data->address);
}

void get_chibi_address(uint8_t com, const GetChibiAddress *data) {
	GetChibiAddressReturn gcar;

	uint8_t extension;
	if(com_ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	gcar.stack_id      = data->stack_id;
	gcar.type          = data->type;
	gcar.length        = sizeof(GetChibiAddressReturn);
	gcar.address       = extension_get_address(extension);

	send_blocking_with_timeout(&gcar, sizeof(GetChibiAddressReturn), com);
	logchibii("get_chibi_address: %d\n\r", gcar.address);
}

void set_chibi_master_address(uint8_t com, const SetChibiMasterAddress *data) {
	uint8_t extension;
	if(com_ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	extension_set_master_address(extension, data->address);
	logchibii("set_chibi_address: %d\n\r", data->address);
}

void get_chibi_master_address(uint8_t com, const GetChibiMasterAddress *data) {
	GetChibiMasterAddressReturn gcmar;

	uint8_t extension;
	if(com_ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	gcmar.stack_id      = data->stack_id;
	gcmar.type          = data->type;
	gcmar.length        = sizeof(GetChibiMasterAddressReturn);
	gcmar.address       = extension_get_master_address(extension);

	send_blocking_with_timeout(&gcmar, sizeof(GetChibiMasterAddressReturn), com);
	logchibii("get_chibi_master_address: %d\n\r", gcmar.address);
}

void set_chibi_slave_address(uint8_t com, const SetChibiSlaveAddress *data) {
	uint8_t extension;
	if(com_ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	extension_set_slave_address(extension, data->num, data->address);
	logchibii("set_chibi_slave_address: %d, %d\n\r", data->num, data->address);
}

void get_chibi_slave_address(uint8_t com, const GetChibiSlaveAddress *data) {
	GetChibiSlaveAddressReturn gcsar;

	uint8_t extension;
	if(com_ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	gcsar.stack_id      = data->stack_id;
	gcsar.type          = data->type;
	gcsar.length        = sizeof(GetChibiSlaveAddressReturn);
	gcsar.address       = extension_get_slave_address(extension, data->num);

	send_blocking_with_timeout(&gcsar, sizeof(GetChibiSlaveAddressReturn), com);
	logchibii("get_chibi_slave_address: %d, %d\n\r", data->num, gcsar.address);
}

void get_chibi_signal_strength(uint8_t com, const GetChibiSignalStrength *data) {
	GetChibiSignalStrengthReturn gcssr;

	gcssr.stack_id        = data->stack_id;
	gcssr.type            = data->type;
	gcssr.length          = sizeof(GetChibiSignalStrengthReturn);
	gcssr.signal_strength = chibi_receiver_input_power;

	send_blocking_with_timeout(&gcssr, sizeof(GetChibiSignalStrengthReturn), com);
	logchibii("get_chibi_signal_strength: %d\n\r", gcssr.signal_strength);
}

void get_chibi_error_log(uint8_t com, const GetChibiErrorLog *data) {
	GetChibiErrorLogReturn gcelr;

	gcelr.stack_id        = data->stack_id;
	gcelr.type            = data->type;
	gcelr.length          = sizeof(GetChibiErrorLogReturn);
	gcelr.underrun        = chibi_underrun;
	gcelr.crc_error       = chibi_crc_error;
	gcelr.no_ack          = chibi_no_ack;
	gcelr.overflow        = chibi_overflow;

	send_blocking_with_timeout(&gcelr, sizeof(GetChibiErrorLogReturn), com);
	logchibii("get_chibi_error_log: %d, %d %d %d\n\r", gcelr.underrun,
	                                                   gcelr.crc_error,
	                                                   gcelr.no_ack,
	                                                   gcelr.overflow);
}

void set_chibi_frequency(uint8_t com, const SetChibiFrequency *data) {
	if(data->frequency > 3) {
		return;
	}

	uint8_t extension;
	if(com_ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	chibi_frequency_mode = data->frequency;

	extension_i2c_write(extension,
	                    CHIBI_ADDRESS_FREQUENCY,
	                    (char*)&chibi_frequency_mode,
	                    1);

	chibi_set_mode(chibi_frequency_mode);

	logchibii("set_chibi_frequency: %d\n\r", data->frequency);
}

void get_chibi_frequency(uint8_t com, const GetChibiFrequency *data) {
	GetChibiFrequencyReturn gcfr;

	gcfr.stack_id        = data->stack_id;
	gcfr.type            = data->type;
	gcfr.length          = sizeof(GetChibiFrequencyReturn);
	gcfr.frequency       = chibi_frequency_mode;

	send_blocking_with_timeout(&gcfr, sizeof(GetChibiFrequencyReturn), com);
	logchibii("get_chibi_frequency: %d\n\r", gcfr.frequency);
}

void set_chibi_channel(uint8_t com, const SetChibiChannel *data) {
	if(data->channel > 10) {
		return;
	}

	uint8_t extension;
	if(com_ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	chibi_channel = data->channel;

	extension_i2c_write(extension,
	                    CHIBI_ADDRESS_CHANNEL,
	                    (char*)&chibi_channel,
	                    1);

	chibi_set_channel(chibi_channel);

	logchibii("set_chibi_channel: %d\n\r", data->channel);
}

void get_chibi_channel(uint8_t com, const GetChibiChannel *data) {
	GetChibiChannelReturn gccr;

	gccr.stack_id        = data->stack_id;
	gccr.type            = data->type;
	gccr.length          = sizeof(GetChibiChannelReturn);
	gccr.channel         = chibi_channel;

	send_blocking_with_timeout(&gccr, sizeof(GetChibiChannelReturn), com);
	logchibii("get_chibi_channel: %d\n\r", gccr.channel);
}

void is_rs485_present(uint8_t com, const IsRS485Present *data) {
	IsRS485PresentReturn irpr;

	irpr.stack_id      = data->stack_id;
	irpr.type          = data->type;
	irpr.length        = sizeof(IsRS485PresentReturn);
	irpr.present       = com_ext[0] == COM_RS485 || com_ext[1] == COM_RS485;

	send_blocking_with_timeout(&irpr, sizeof(IsRS485PresentReturn), com);
	logrsi("is_rs485_present: %d\n\r", irpr.present);
}

void set_rs485_address(uint8_t com, const SetRS485Address *data) {
	uint8_t extension;
	if(com_ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_ext[1] == COM_RS485) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	extension_set_address(extension, data->address);
	logrsi("set_rs485_address: %d\n\r", data->address);
}

void get_rs485_address(uint8_t com, const GetRS485Address *data) {
	GetRS485AddressReturn grar;

	uint8_t extension;
	if(com_ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_ext[1] == COM_RS485) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	grar.stack_id      = data->stack_id;
	grar.type          = data->type;
	grar.length        = sizeof(GetRS485AddressReturn);
	grar.address       = extension_get_address(extension);

	send_blocking_with_timeout(&grar, sizeof(GetRS485AddressReturn), com);
	logrsi("get_rs485_address: %d\n\r", grar.address);
}

void set_rs485_slave_address(uint8_t com, const SetRS485SlaveAddress *data) {
	uint8_t extension;
	if(com_ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_ext[1] == COM_RS485) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	extension_set_slave_address(extension, data->num, data->address);
	logrsi("set_rs485_slave_address: %d, %d\n\r", data->num, data->address);
}

void get_rs485_slave_address(uint8_t com, const GetRS485SlaveAddress *data) {
	GetRS485SlaveAddressReturn grsar;

	uint8_t extension;
	if(com_ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_ext[1] == COM_RS485) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	grsar.stack_id      = data->stack_id;
	grsar.type          = data->type;
	grsar.length        = sizeof(GetRS485SlaveAddressReturn);
	grsar.address       = extension_get_slave_address(extension, data->num);

	send_blocking_with_timeout(&grsar, sizeof(GetRS485SlaveAddressReturn), com);
	logrsi("get_rs485_slave_address: %d, %d\n\r", data->num, grsar.address);
}

void get_rs485_error_log(uint8_t com, const GetRS485ErrorLog *data) {
	GetRS485ErrorLogReturn grelr;

	grelr.stack_id        = data->stack_id;
	grelr.type            = data->type;
	grelr.length          = sizeof(GetRS485ErrorLogReturn);
	grelr.crc_error       = rs485_error_crc;

	send_blocking_with_timeout(&grelr, sizeof(GetRS485ErrorLogReturn), com);
}

void set_rs485_configuration(uint8_t com, const SetRS485Configuration *data) {
	uint8_t extension;
	if(com_ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_ext[1] == COM_RS485) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	extension_i2c_write(extension, EXTENSION_POS_ANY, (char*)&data->speed, 6);

	logrsi("set_rs485_configuration: %d, %c, %d\n\r", data->speed,
	                                                  data->parity,
	                                                  data->stopbits);
}

void get_rs485_configuration(uint8_t com, const GetRS485Configuration *data) {
	uint8_t extension;
	if(com_ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_ext[1] == COM_RS485) {
		extension = 1;
	} else {
		// TODO: Error?
		return;
	}

	GetRS485ConfigurationReturn grcr;

	grcr.stack_id        = data->stack_id;
	grcr.type            = data->type;
	grcr.length          = sizeof(GetRS485ConfigurationReturn);
	extension_i2c_read(extension, EXTENSION_POS_ANY, (char*)&grcr.speed, 6);

	send_blocking_with_timeout(&grcr, sizeof(GetRS485ConfigurationReturn), com);

	logrsi("get_rs485_configuration: %d, %c, %d\n\r", grcr.speed,
	                                                  grcr.parity,
	                                                  grcr.stopbits);
}

void is_wifi_present(uint8_t com, const IsWifiPresent *data) {
	IsWifiPresentReturn iwpr;

	iwpr.stack_id      = data->stack_id;
	iwpr.type          = data->type;
	iwpr.length        = sizeof(IsWifiPresentReturn);
	iwpr.present       = com_ext[0] == COM_WIFI || com_ext[1] == COM_WIFI;

	send_blocking_with_timeout(&iwpr, sizeof(IsWifiPresentReturn), com);
	logwifii("is_wifi_present: %d\n\r", iwpr.present);
}

void set_wifi_configuration(uint8_t com, const SetWifiConfiguration *data) {
	wifi_write_config(((char*)data) + 4,
	                  sizeof(SetWifiConfiguration)-4,
	                  WIFI_CONFIGURATION_POS);

	logwifii("set_wifi_configuration: %d, %d.%d.%d.%d:%d\n\r", data->connection,
	                                                           data->ip[3],
	                                                           data->ip[2],
	                                                           data->ip[1],
	                                                           data->ip[0],
	                                                           data->port);
}

void get_wifi_configuration(uint8_t com, const GetWifiConfiguration *data) {
	GetWifiConfigurationReturn gwcr;

	gwcr.stack_id        = data->stack_id;
	gwcr.type            = data->type;
	gwcr.length          = sizeof(GetWifiConfigurationReturn);
	wifi_read_config(((char*)&gwcr) + 4,
	                 sizeof(GetWifiConfigurationReturn)-4,
	                 WIFI_CONFIGURATION_POS);

	send_blocking_with_timeout(&gwcr, sizeof(GetWifiConfigurationReturn), com);

	logwifii("get_wifi_configuration: %d, %d.%d.%d.%d:%d\n\r", gwcr.connection,
	                                                           gwcr.ip[3],
	                                                           gwcr.ip[2],
	                                                           gwcr.ip[1],
	                                                           gwcr.ip[0],
	                                                           gwcr.port);
}

void set_wifi_encryption(uint8_t com, const SetWifiEncryption *data) {
	wifi_write_config(((char*)data) + 4,
	                  sizeof(SetWifiEncryption) - 4,
	                  WIFI_ENCRYPTION_POS);

	logwifii("set_wifi_encryption: %d\n\r", data->encryption);
}

void get_wifi_encryption(uint8_t com, const GetWifiEncryption *data) {
	GetWifiEncryptionReturn gwer;

	gwer.stack_id        = data->stack_id;
	gwer.type            = data->type;
	gwer.length          = sizeof(GetWifiEncryptionReturn);
	wifi_read_config(((char*)&gwer) + 4,
	                 sizeof(GetWifiEncryptionReturn) - 4,
	                 WIFI_ENCRYPTION_POS);

	send_blocking_with_timeout(&gwer, sizeof(GetWifiEncryptionReturn), com);

	logwifii("get_wifi_encryption: %d\n\r", gwer.encryption);
}

void get_wifi_status(uint8_t com, const GetWifiStatus *data) {
	GetWifiStatusReturn gwsr;

	gwsr.stack_id        = data->stack_id;
	gwsr.type            = data->type;
	gwsr.length          = sizeof(GetWifiStatusReturn);
	memcpy(((char*)&gwsr)+4, &wifi_status, sizeof(GetWifiStatusReturn)-4);

	send_blocking_with_timeout(&gwsr, sizeof(GetWifiStatusReturn), com);
}

void refresh_wifi_status(uint8_t com, const RefreshWifiStatus *data) {
	wifi_refresh_status();
	logwifii("wifi_refresh_status\n\r");
}

void set_wifi_certificate(uint8_t com, const SetWifiCertificate *data) {
	if(data->data_length > 32) {
		return;
	}

	if(data->index == 0xFFFF) {
		char data_tmp[32];
		memcpy(data_tmp, data->data, data->data_length);
		memset(data_tmp + data->data_length, 0, 32 - data->data_length);
		wifi_write_config(data_tmp, 32, WIFI_USERNAME_POS);
	} else if(data->index == 0xFFFE) {
		char data_tmp[32];
		memcpy(data_tmp, data->data, data->data_length);
		memset(data_tmp + data->data_length, 0, 32 - data->data_length);
		wifi_write_config(data_tmp, 32, WIFI_PASSWORD_POS);
	} else if(data->index < WIFI_CA_CERTIFICATE_MAX_LENGTH/32) {
		wifi_write_config(data->data,
		                  32,
		                  WIFI_CA_CERTIFICATE_POS + data->index*32);
	} else if(data->index < 10000 + WIFI_CLIENT_CERTIFICATE_MAX_LENGTH/32) {
		uint16_t index = data->index - 10000;
		wifi_write_config(data->data,
		                  32,
		                  WIFI_CLIENT_CERTIFICATE_POS + index*32);
	} else if(data->index < 20000 + WIFI_PRIVATE_KEY_MAX_LENGTH/32) {
		uint16_t index = data->index - 20000;
		wifi_write_config(data->data,
		                  32,
		                  WIFI_PRIVATE_KEY_POS + index*32);
	}

	logwifii("set_wifi_certificate: %d %d\n\r", data->index, data->data_length);
}

void get_wifi_certificate(uint8_t com, const GetWifiCertificate *data) {
	GetWifiCertificateReturn gwcr;

	gwcr.stack_id        = data->stack_id;
	gwcr.type            = data->type;
	gwcr.length          = sizeof(GetWifiCertificateReturn);

	gwcr.data_length     = 0;
	if(data->index == 0xFFFF) {
		wifi_read_config(gwcr.data,
		                 32,
		                 WIFI_USERNAME_POS);

		for(int8_t i = 31; i >= 0; i--) {
			if(gwcr.data[i] != 0) {
				gwcr.data_length = i+1;
				break;
 			}
		}
	} else if(data->index == 0xFFFE) {
		wifi_read_config(gwcr.data,
		                 32,
		                 WIFI_PASSWORD_POS);

		for(int8_t i = 31; i >= 0; i--) {
			if(gwcr.data[i] != 0) {
				gwcr.data_length = i+1;
				break;
 			}
		}
	} else if(data->index < WIFI_CA_CERTIFICATE_MAX_LENGTH/32) {
		wifi_read_config(gwcr.data,
		                 32,
		                 WIFI_CA_CERTIFICATE_POS + data->index*32);
		if(data->index == ((wifi_configuration.ca_certificate_length+31)/32 - 1)) {
			gwcr.data_length = wifi_configuration.ca_certificate_length % 32;
		} else {
			gwcr.data_length = 32;
		}
	} else if(data->index < 10000 + WIFI_CLIENT_CERTIFICATE_MAX_LENGTH/32) {
		uint16_t index = data->index - 10000;
		wifi_read_config(gwcr.data,
		                 32,
		                 WIFI_CLIENT_CERTIFICATE_POS + index*32);
		if(index == ((wifi_configuration.client_certificate_length+31)/32 - 1)) {
			gwcr.data_length = wifi_configuration.client_certificate_length % 32;
		} else {
			gwcr.data_length = 32;
		}
	} else if(data->index < 20000 + WIFI_PRIVATE_KEY_MAX_LENGTH/32) {
		uint16_t index = data->index - 20000;
		wifi_read_config(gwcr.data,
		                 32,
		                 WIFI_PRIVATE_KEY_POS + index*32);
		if(index == ((wifi_configuration.private_key_length+31)/32 - 1)) {
			gwcr.data_length = wifi_configuration.private_key_length % 32;
		} else {
			gwcr.data_length = 32;
		}
	}

	send_blocking_with_timeout(&gwcr, sizeof(GetWifiCertificateReturn), com);
	logwifii("get_wifi_certificate: %d %d\n\r", data->index, gwcr.data_length);
}

void set_wifi_power_mode(uint8_t com, const SetWifiPowerMode *data) {
	wifi_set_power_mode(data->mode);
	logwifii("set_wifi_power_mode: %d\n\r", data->mode);
}

void get_wifi_power_mode(uint8_t com, const GetWifiPowerMode *data) {
	GetWifiPowerModeReturn gwpmr;

	gwpmr.stack_id        = data->stack_id;
	gwpmr.type            = data->type;
	gwpmr.length          = sizeof(GetWifiPowerModeReturn);
	gwpmr.mode            = wifi_power_mode;

	send_blocking_with_timeout(&gwpmr, sizeof(GetWifiPowerModeReturn), com);
	logwifii("get_wifi_power_mode: %d\n\r", gwpmr.mode);
}

void get_wifi_buffer_info(uint8_t com, const GetWifiBufferInfo *data) {
	GetWifiBufferInfoReturn gwbir;

	gwbir.stack_id        = data->stack_id;
	gwbir.type            = data->type;
	gwbir.length          = sizeof(GetWifiBufferInfoReturn);
	gwbir.overflow        = wifi_data_ringbuffer_overflow;
	gwbir.low_watermark   = wifi_data_ringbuffer_low_watermark;
	gwbir.used            = wifi_data_get_ringbuffer_diff();

	send_blocking_with_timeout(&gwbir, sizeof(GetWifiBufferInfoReturn), com);
	logwifii("get_wifi_buffer_info: %d %d %d\n\r", gwbir.overflow,
	                                               gwbir.low_watermark,
	                                               gwbir.used);
}
