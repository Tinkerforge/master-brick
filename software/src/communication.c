/* master-brick
 * Copyright (C) 2010-2012 Olaf Lüke <olaf@tinkerforge.com>
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

extern WifiStatus wifi_status;
extern WifiConfiguration wifi_configuration;
extern uint8_t wifi_power_mode;

extern uint32_t wifi_data_ringbuffer_overflow;
extern uint16_t wifi_data_ringbuffer_low_watermark;

extern uint16_t master_usb_voltage;
extern uint16_t master_stack_voltage;
extern uint16_t master_stack_current;

void get_stack_voltage(const ComType com, const GetStackVoltage *data) {
	GetStackVoltageReturn gsvr;

	gsvr.header        = data->header;
	gsvr.header.length = sizeof(GetStackVoltageReturn);

	gsvr.voltage = master_stack_voltage *
	               STACK_VOLTAGE_REFERENCE *
	               STACK_VOLTAGE_MULTIPLIER /
	               VOLTAGE_MAX_VALUE;
	if(gsvr.voltage < VOLTAGE_EPSILON) {
		gsvr.voltage = 0;
	}

	send_blocking_with_timeout(&gsvr, sizeof(GetStackVoltageReturn), com);
	logmasteri("get_stack_voltage: %d\n\r", gsvr.voltage);
}

void get_stack_current(const ComType com, const GetStackCurrent *data) {
	GetStackCurrentReturn gscr;

	gscr.header        = data->header;
	gscr.header.length = sizeof(GetStackCurrentReturn);

	uint16_t voltage = master_stack_voltage *
	                   STACK_VOLTAGE_REFERENCE *
	                   STACK_VOLTAGE_MULTIPLIER /
	                   VOLTAGE_MAX_VALUE;

	if(voltage < VOLTAGE_EPSILON) {
		gscr.current = 0;
	} else {
		gscr.current = master_stack_current *
		               STACK_CURRENT_REFERENCE *
		               STACK_CURRENT_MULTIPLIER /
		               VOLTAGE_MAX_VALUE;
	}

	send_blocking_with_timeout(&gscr, sizeof(GetStackCurrentReturn), com);
	logmasteri("get_stack_current: %d\n\r", gscr.current);
}

void set_extension_type(const ComType com, const SetExtensionType *data) {
	extension_set_type(data->extension, data->exttype);
	logmasteri("set_extension_type: %lu\n\r", data->exttype);
}

void get_extension_type(const ComType com, const GetExtensionType *data) {
	GetExtensionTypeReturn getr;

	getr.header        = data->header;
	getr.header.length = sizeof(GetExtensionTypeReturn);
	getr.exttype       = extension_get_type(data->extension);

	send_blocking_with_timeout(&getr, sizeof(GetExtensionTypeReturn), com);
	logmasteri("get_extension_type: %lu\n\r", getr.exttype);
}

void is_chibi_present(const ComType com, const IsChibiPresent *data) {
	IsChibiPresentReturn icpr;

	icpr.header        = data->header;
	icpr.header.length = sizeof(IsChibiPresentReturn);
	icpr.present       = com_ext[0] == COM_CHIBI || com_ext[1] == COM_CHIBI;

	send_blocking_with_timeout(&icpr, sizeof(IsChibiPresentReturn), com);
	logchibii("is_chibi_present: %d\n\r", icpr.present);
}

void set_chibi_address(const ComType com, const SetChibiAddress *data) {
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

void get_chibi_address(const ComType com, const GetChibiAddress *data) {
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

	gcar.header        = data->header;
	gcar.header.length = sizeof(GetChibiAddressReturn);
	gcar.address       = extension_get_address(extension);

	send_blocking_with_timeout(&gcar, sizeof(GetChibiAddressReturn), com);
	logchibii("get_chibi_address: %d\n\r", gcar.address);
}

void set_chibi_master_address(const ComType com, const SetChibiMasterAddress *data) {
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

void get_chibi_master_address(const ComType com, const GetChibiMasterAddress *data) {
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

	gcmar.header        = data->header;
	gcmar.header.length = sizeof(GetChibiMasterAddressReturn);
	gcmar.address       = extension_get_master_address(extension);

	send_blocking_with_timeout(&gcmar, sizeof(GetChibiMasterAddressReturn), com);
	logchibii("get_chibi_master_address: %d\n\r", gcmar.address);
}

void set_chibi_slave_address(const ComType com, const SetChibiSlaveAddress *data) {
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

void get_chibi_slave_address(const ComType com, const GetChibiSlaveAddress *data) {
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

	gcsar.header        = data->header;
	gcsar.header.length = sizeof(GetChibiSlaveAddressReturn);
	gcsar.address       = extension_get_slave_address(extension, data->num);

	send_blocking_with_timeout(&gcsar, sizeof(GetChibiSlaveAddressReturn), com);
	logchibii("get_chibi_slave_address: %d, %d\n\r", data->num, gcsar.address);
}

void get_chibi_signal_strength(const ComType com, const GetChibiSignalStrength *data) {
	GetChibiSignalStrengthReturn gcssr;

	gcssr.header          = data->header;
	gcssr.header.length   = sizeof(GetChibiSignalStrengthReturn);
	gcssr.signal_strength = chibi_receiver_input_power;

	send_blocking_with_timeout(&gcssr, sizeof(GetChibiSignalStrengthReturn), com);
	logchibii("get_chibi_signal_strength: %d\n\r", gcssr.signal_strength);
}

void get_chibi_error_log(const ComType com, const GetChibiErrorLog *data) {
	GetChibiErrorLogReturn gcelr;

	gcelr.header          = data->header;
	gcelr.header.length   = sizeof(GetChibiErrorLogReturn);
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

void set_chibi_frequency(const ComType com, const SetChibiFrequency *data) {
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

void get_chibi_frequency(const ComType com, const GetChibiFrequency *data) {
	GetChibiFrequencyReturn gcfr;

	gcfr.header          = data->header;
	gcfr.header.length   = sizeof(GetChibiFrequencyReturn);
	gcfr.frequency       = chibi_frequency_mode;

	send_blocking_with_timeout(&gcfr, sizeof(GetChibiFrequencyReturn), com);
	logchibii("get_chibi_frequency: %d\n\r", gcfr.frequency);
}

void set_chibi_channel(const ComType com, const SetChibiChannel *data) {
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

void get_chibi_channel(const ComType com, const GetChibiChannel *data) {
	GetChibiChannelReturn gccr;

	gccr.header          = data->header;
	gccr.header.length   = sizeof(GetChibiChannelReturn);
	gccr.channel         = chibi_channel;

	send_blocking_with_timeout(&gccr, sizeof(GetChibiChannelReturn), com);
	logchibii("get_chibi_channel: %d\n\r", gccr.channel);
}

void is_rs485_present(const ComType com, const IsRS485Present *data) {
	IsRS485PresentReturn irpr;

	irpr.header        = data->header;
	irpr.header.length = sizeof(IsRS485PresentReturn);
	irpr.present       = com_ext[0] == COM_RS485 || com_ext[1] == COM_RS485;

	send_blocking_with_timeout(&irpr, sizeof(IsRS485PresentReturn), com);
	logrsi("is_rs485_present: %d\n\r", irpr.present);
}

void set_rs485_address(const ComType com, const SetRS485Address *data) {
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

void get_rs485_address(const ComType com, const GetRS485Address *data) {
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

	grar.header        = data->header;
	grar.header.length = sizeof(GetRS485AddressReturn);
	grar.address       = extension_get_address(extension);

	send_blocking_with_timeout(&grar, sizeof(GetRS485AddressReturn), com);
	logrsi("get_rs485_address: %d\n\r", grar.address);
}

void set_rs485_slave_address(const ComType com, const SetRS485SlaveAddress *data) {
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

void get_rs485_slave_address(const ComType com, const GetRS485SlaveAddress *data) {
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

	grsar.header        = data->header;
	grsar.header.length = sizeof(GetRS485SlaveAddressReturn);
	grsar.address       = extension_get_slave_address(extension, data->num);

	send_blocking_with_timeout(&grsar, sizeof(GetRS485SlaveAddressReturn), com);
	logrsi("get_rs485_slave_address: %d, %d\n\r", data->num, grsar.address);
}

void get_rs485_error_log(const ComType com, const GetRS485ErrorLog *data) {
	GetRS485ErrorLogReturn grelr;

	grelr.header          = data->header;
	grelr.header.length   = sizeof(GetRS485ErrorLogReturn);
	grelr.crc_error       = rs485_error_crc;

	send_blocking_with_timeout(&grelr, sizeof(GetRS485ErrorLogReturn), com);
}

void set_rs485_configuration(const ComType com, const SetRS485Configuration *data) {
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

	logrsi("set_rs485_configuration: %lu, %c, %d\n\r", data->speed,
	                                                   data->parity,
	                                                   data->stopbits);
}

void get_rs485_configuration(const ComType com, const GetRS485Configuration *data) {
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

	grcr.header        = data->header;
	grcr.header.length = sizeof(GetRS485ConfigurationReturn);
	extension_i2c_read(extension, EXTENSION_POS_ANY, (char*)&grcr.speed, 6);

	send_blocking_with_timeout(&grcr, sizeof(GetRS485ConfigurationReturn), com);

	logrsi("get_rs485_configuration: %lu, %c, %d\n\r", grcr.speed,
	                                                   grcr.parity,
	                                                   grcr.stopbits);
}

void is_wifi_present(const ComType com, const IsWifiPresent *data) {
	IsWifiPresentReturn iwpr;

	iwpr.header        = data->header;
	iwpr.header.length = sizeof(IsWifiPresentReturn);
	iwpr.present       = com_ext[0] == COM_WIFI || com_ext[1] == COM_WIFI;

	send_blocking_with_timeout(&iwpr, sizeof(IsWifiPresentReturn), com);
	logwifii("is_wifi_present: %d\n\r", iwpr.present);
}

void set_wifi_configuration(const ComType com, const SetWifiConfiguration *data) {
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

void get_wifi_configuration(const ComType com, const GetWifiConfiguration *data) {
	GetWifiConfigurationReturn gwcr;

	gwcr.header        = data->header;
	gwcr.header.length = sizeof(GetWifiConfigurationReturn);
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

void set_wifi_encryption(const ComType com, const SetWifiEncryption *data) {
	wifi_write_config(((char*)data) + 4,
	                  sizeof(SetWifiEncryption) - 4,
	                  WIFI_ENCRYPTION_POS);

	logwifii("set_wifi_encryption: %d\n\r", data->encryption);
}

void get_wifi_encryption(const ComType com, const GetWifiEncryption *data) {
	GetWifiEncryptionReturn gwer;

	gwer.header        = data->header;
	gwer.header.length = sizeof(GetWifiEncryptionReturn);
	wifi_read_config(((char*)&gwer) + 4,
	                 sizeof(GetWifiEncryptionReturn) - 4,
	                 WIFI_ENCRYPTION_POS);

	send_blocking_with_timeout(&gwer, sizeof(GetWifiEncryptionReturn), com);

	logwifii("get_wifi_encryption: %d\n\r", gwer.encryption);
}

void get_wifi_status(const ComType com, const GetWifiStatus *data) {
	GetWifiStatusReturn gwsr;

	gwsr.header        = data->header;
	gwsr.header.length = sizeof(GetWifiStatusReturn);
	memcpy(((char*)&gwsr)+4, &wifi_status, sizeof(GetWifiStatusReturn)-4);

	send_blocking_with_timeout(&gwsr, sizeof(GetWifiStatusReturn), com);
}

void refresh_wifi_status(const ComType com, const RefreshWifiStatus *data) {
	wifi_refresh_status();
	logwifii("wifi_refresh_status\n\r");
}

void set_wifi_certificate(const ComType com, const SetWifiCertificate *data) {
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

void get_wifi_certificate(const ComType com, const GetWifiCertificate *data) {
	GetWifiCertificateReturn gwcr;

	gwcr.header          = data->header;
	gwcr.header.length   = sizeof(GetWifiCertificateReturn);

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

void set_wifi_power_mode(const ComType com, const SetWifiPowerMode *data) {
	wifi_set_power_mode(data->mode);
	logwifii("set_wifi_power_mode: %d\n\r", data->mode);
}

void get_wifi_power_mode(const ComType com, const GetWifiPowerMode *data) {
	GetWifiPowerModeReturn gwpmr;

	gwpmr.header          = data->header;
	gwpmr.header.length   = sizeof(GetWifiPowerModeReturn);
	gwpmr.mode            = wifi_power_mode;

	send_blocking_with_timeout(&gwpmr, sizeof(GetWifiPowerModeReturn), com);
	logwifii("get_wifi_power_mode: %d\n\r", gwpmr.mode);
}

void get_wifi_buffer_info(const ComType com, const GetWifiBufferInfo *data) {
	GetWifiBufferInfoReturn gwbir;

	gwbir.header          = data->header;
	gwbir.header.length   = sizeof(GetWifiBufferInfoReturn);
	gwbir.overflow        = wifi_data_ringbuffer_overflow;
	gwbir.low_watermark   = wifi_data_ringbuffer_low_watermark;
	gwbir.used            = wifi_data_get_ringbuffer_diff();

	send_blocking_with_timeout(&gwbir, sizeof(GetWifiBufferInfoReturn), com);
	logwifii("get_wifi_buffer_info: %lu %d %d\n\r", gwbir.overflow,
	                                               gwbir.low_watermark,
	                                               gwbir.used);
}

void set_wifi_regulatory_domain(const ComType com, const SetWifiRegulatoryDomain *data) {
	if(data->domain < 4) {
		wifi_write_config((char*)&data->domain, 1, WIFI_REGULATORY_DOMAIN_POS);
		wifi_configuration.regulatory_domain = data->domain;
		logwifii("wifi_set_regulatory_domain: %d\n\r", data->domain);
	}
}

void get_wifi_regulatory_domain(const ComType com, const GetWifiRegulatoryDomain *data) {
	GetWifiRegulatoryDomainReturn gwrdr;

	gwrdr.header        = data->header;
	gwrdr.header.length = sizeof(GetWifiRegulatoryDomainReturn);
	wifi_read_config((char*)&gwrdr.domain, 1, WIFI_REGULATORY_DOMAIN_POS);

	send_blocking_with_timeout(&gwrdr, sizeof(GetWifiRegulatoryDomainReturn), com);
	logwifii("get_wifi_regulatory_domain: %d\n\r", gwrdr.domain);
}

void get_usb_voltage(const ComType com, const GetUSBVoltage *data) {
	GetUSBVoltageReturn guvr;

	guvr.header          = data->header;
	guvr.header.length   = sizeof(GetUSBVoltageReturn);
	guvr.voltage         = master_usb_voltage*USB_VOLTAGE_REFERENCE*USB_VOLTAGE_MULTIPLIER/(VOLTAGE_MAX_VALUE*USB_VOLTAGE_DIVISOR);

	send_blocking_with_timeout(&guvr, sizeof(GetUSBVoltageReturn), com);
	logwifii("get_usb_voltage: %d\n\r", guvr.voltage);
}
