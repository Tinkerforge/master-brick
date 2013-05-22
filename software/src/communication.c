/* master-brick
 * Copyright (C) 2010-2013 Olaf Lüke <olaf@tinkerforge.com>
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
#include "extensions/wifi/wifi_ringbuffer.h"
#include "extensions/wifi/wifi_data.h"
#include "extensions/wifi/wifi_command.h"
#include "extensions/extension_i2c.h"
#include "extensions/extension_init.h"
#include "bricklib/utility/util_definitions.h"
#include "extensions/ethernet/ethernet_config.h"
#include "extensions/ethernet/ethernet.h"
#include "extensions/ethernet/ethernet_low_level.h"

extern ComInfo com_info;
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

extern uint32_t wifi_ringbuffer_overflow;
extern uint16_t wifi_ringbuffer_low_watermark;

extern MasterCallback master_callback;

extern EthernetStatus ethernet_status;

void get_stack_voltage(const ComType com, const GetStackVoltage *data) {
	GetStackVoltageReturn gsvr;

	gsvr.header        = data->header;
	gsvr.header.length = sizeof(GetStackVoltageReturn);

	gsvr.voltage = master_callback.stack_voltage;
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

	if(master_callback.stack_voltage < VOLTAGE_EPSILON) {
		gscr.current = 0;
	} else {
		gscr.current = master_callback.stack_current;
	}

	send_blocking_with_timeout(&gscr, sizeof(GetStackCurrentReturn), com);
	logmasteri("get_stack_current: %d\n\r", gscr.current);
}

void set_extension_type(const ComType com, const SetExtensionType *data) {
	if(data->exttype > EXTENSION_TYPE_MAX || data->extension >= EXTENSION_NUM_MAX) {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	extension_set_type(data->extension, data->exttype);
	logmasteri("set_extension_type: %d -> %lu\n\r", data->extension, data->exttype);

	com_return_setter(com, data);
}

void get_extension_type(const ComType com, const GetExtensionType *data) {
	GetExtensionTypeReturn getr;

	if(data->extension >= EXTENSION_NUM_MAX) {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

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
	icpr.present       = com_info.ext[0] == COM_CHIBI || com_info.ext[1] == COM_CHIBI;

	send_blocking_with_timeout(&icpr, sizeof(IsChibiPresentReturn), com);
	logchibii("is_chibi_present: %d\n\r", icpr.present);
}

void set_chibi_address(const ComType com, const SetChibiAddress *data) {
	uint8_t extension;
	if(com_info.ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	if(data->address == 0) {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	extension_set_address(extension, data->address);
	logchibii("set_chibi_address: %d\n\r", data->address);

	com_return_setter(com, data);
}

void get_chibi_address(const ComType com, const GetChibiAddress *data) {
	GetChibiAddressReturn gcar;

	uint8_t extension;
	if(com_info.ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
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
	if(com_info.ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	if(data->address == 0) {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	extension_set_master_address(extension, data->address);
	logchibii("set_chibi_address: %d\n\r", data->address);

	com_return_setter(com, data);
}

void get_chibi_master_address(const ComType com, const GetChibiMasterAddress *data) {
	GetChibiMasterAddressReturn gcmar;

	uint8_t extension;
	if(com_info.ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
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
	if(com_info.ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	extension_set_slave_address(extension, data->num, data->address);
	logchibii("set_chibi_slave_address: %d, %d\n\r", data->num, data->address);

	com_return_setter(com, data);
}

void get_chibi_slave_address(const ComType com, const GetChibiSlaveAddress *data) {
	GetChibiSlaveAddressReturn gcsar;

	uint8_t extension;
	if(com_info.ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
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
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	uint8_t extension;
	if(com_info.ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	chibi_frequency_mode = data->frequency;

	extension_i2c_write(extension,
	                    CHIBI_ADDRESS_FREQUENCY,
	                    (char*)&chibi_frequency_mode,
	                    1);

	chibi_set_mode(chibi_frequency_mode);

	logchibii("set_chibi_frequency: %d\n\r", data->frequency);

	com_return_setter(com, data);
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
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	uint8_t extension;
	if(com_info.ext[0] == COM_CHIBI) {
		extension = 0;
	} else if(com_info.ext[1] == COM_CHIBI) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	chibi_channel = data->channel;

	extension_i2c_write(extension,
	                    CHIBI_ADDRESS_CHANNEL,
	                    (char*)&chibi_channel,
	                    1);

	chibi_set_channel(chibi_channel);

	logchibii("set_chibi_channel: %d\n\r", data->channel);

	com_return_setter(com, data);
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
	irpr.present       = com_info.ext[0] == COM_RS485 || com_info.ext[1] == COM_RS485;

	send_blocking_with_timeout(&irpr, sizeof(IsRS485PresentReturn), com);
	logrsi("is_rs485_present: %d\n\r", irpr.present);
}

void set_rs485_address(const ComType com, const SetRS485Address *data) {
	uint8_t extension;
	if(com_info.ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_info.ext[1] == COM_RS485) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	extension_set_address(extension, data->address);
	logrsi("set_rs485_address: %d\n\r", data->address);

	com_return_setter(com, data);
}

void get_rs485_address(const ComType com, const GetRS485Address *data) {
	GetRS485AddressReturn grar;

	uint8_t extension;
	if(com_info.ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_info.ext[1] == COM_RS485) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
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
	if(com_info.ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_info.ext[1] == COM_RS485) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	extension_set_slave_address(extension, data->num, data->address);
	logrsi("set_rs485_slave_address: %d, %d\n\r", data->num, data->address);

	com_return_setter(com, data);
}

void get_rs485_slave_address(const ComType com, const GetRS485SlaveAddress *data) {
	GetRS485SlaveAddressReturn grsar;

	uint8_t extension;
	if(com_info.ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_info.ext[1] == COM_RS485) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
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
	if(com_info.ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_info.ext[1] == COM_RS485) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	if((data->parity != 'n' && data->parity != 'N' &&
	    data->parity != 'e' && data->parity != 'E' &&
	    data->parity != 'o' && data->parity != 'O') ||
	   (data->stopbits != 1 && data->stopbits != 2)) {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	extension_i2c_write(extension, EXTENSION_POS_ANY, (char*)&data->speed, 6);

	logrsi("set_rs485_configuration: %lu, %c, %d\n\r", data->speed,
	                                                   data->parity,
	                                                   data->stopbits);

	com_return_setter(com, data);
}

void get_rs485_configuration(const ComType com, const GetRS485Configuration *data) {
	uint8_t extension;
	if(com_info.ext[0] == COM_RS485) {
		extension = 0;
	} else if(com_info.ext[1] == COM_RS485) {
		extension = 1;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
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
	iwpr.present       = com_info.ext[0] == COM_WIFI || com_info.ext[1] == COM_WIFI;

	send_blocking_with_timeout(&iwpr, sizeof(IsWifiPresentReturn), com);
	logwifii("is_wifi_present: %d\n\r", iwpr.present);
}

void set_wifi_configuration(const ComType com, const SetWifiConfiguration *data) {
	wifi_write_config(((char*)data) + sizeof(MessageHeader),
	                  sizeof(SetWifiConfiguration) - sizeof(MessageHeader),
	                  WIFI_CONFIGURATION_POS);

	logwifii("set_wifi_configuration: %d, %d.%d.%d.%d:%d\n\r", data->connection,
	                                                           data->ip[3],
	                                                           data->ip[2],
	                                                           data->ip[1],
	                                                           data->ip[0],
	                                                           data->port);

	com_return_setter(com, data);
}

void get_wifi_configuration(const ComType com, const GetWifiConfiguration *data) {
	GetWifiConfigurationReturn gwcr;

	gwcr.header        = data->header;
	gwcr.header.length = sizeof(GetWifiConfigurationReturn);
	wifi_read_config(((char*)&gwcr) + sizeof(MessageHeader),
	                 sizeof(GetWifiConfigurationReturn) - sizeof(MessageHeader),
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
	wifi_write_config(((char*)data) + sizeof(MessageHeader),
	                  sizeof(SetWifiEncryption) - sizeof(MessageHeader),
	                  WIFI_ENCRYPTION_POS);

	logwifii("set_wifi_encryption: %d\n\r", data->encryption);

	com_return_setter(com, data);
}

void get_wifi_encryption(const ComType com, const GetWifiEncryption *data) {
	GetWifiEncryptionReturn gwer;

	gwer.header        = data->header;
	gwer.header.length = sizeof(GetWifiEncryptionReturn);
	wifi_read_config(((char*)&gwer) + sizeof(MessageHeader),
	                 sizeof(GetWifiEncryptionReturn) - sizeof(MessageHeader),
	                 WIFI_ENCRYPTION_POS);

	send_blocking_with_timeout(&gwer, sizeof(GetWifiEncryptionReturn), com);

	logwifii("get_wifi_encryption: %d\n\r", gwer.encryption);
}

void get_wifi_status(const ComType com, const GetWifiStatus *data) {
	GetWifiStatusReturn gwsr;

	gwsr.header        = data->header;
	gwsr.header.length = sizeof(GetWifiStatusReturn);
	memcpy(((char*)&gwsr) + sizeof(MessageHeader), &wifi_status, sizeof(GetWifiStatusReturn) - sizeof(MessageHeader));

	send_blocking_with_timeout(&gwsr, sizeof(GetWifiStatusReturn), com);
}

void refresh_wifi_status(const ComType com, const RefreshWifiStatus *data) {
	if(wifi_status.state == WIFI_STATE_ASSOCIATED) {
		wifi_command_send(WIFI_COMMAND_ID_AT_NSTAT);
	}
	logwifii("wifi_refresh_status\n\r");

	com_return_setter(com, data);
}

void set_wifi_certificate(const ComType com, const SetWifiCertificate *data) {
	if(data->data_length > 32) {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
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

	com_return_setter(com, data);
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
	if(data->mode > 1) {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	wifi_set_power_mode(data->mode);
	logwifii("set_wifi_power_mode: %d\n\r", data->mode);

	com_return_setter(com, data);
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
	gwbir.overflow        = wifi_ringbuffer_overflow;
	gwbir.low_watermark   = wifi_ringbuffer_low_watermark;
	gwbir.used            = WIFI_RINGBUFFER_SIZE - wifi_ringbuffer_get_free();

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

		com_return_setter(com, data);
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
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
	guvr.voltage         = master_callback.usb_voltage;

	send_blocking_with_timeout(&guvr, sizeof(GetUSBVoltageReturn), com);
	logwifii("get_usb_voltage: %d\n\r", guvr.voltage);
}

void set_long_wifi_key(const ComType com, const SetLongWifiKey *data) {
	wifi_write_config(data->key, 64, WIFI_LONG_KEY_POS);
	com_return_setter(com, data);
}

void get_long_wifi_key(const ComType com, const GetLongWifiKey *data) {
	GetLongWifiKeyReturn glwkr;

	glwkr.header        = data->header;
	glwkr.header.length = sizeof(GetLongWifiKeyReturn);
	wifi_read_config(glwkr.key, 64, WIFI_LONG_KEY_POS);

	send_blocking_with_timeout(&glwkr, sizeof(GetLongWifiKeyReturn), com);
}

void set_wifi_hostname(const ComType com, const SetWifiHostname *data) {
	WIFIHostname wifi_hostname = {WIFI_KEY, "\0"};
	uint8_t i = 0;
	for(; i < 15; i++) {
		if(data->hostname[i] == '\0') {
			break;
		}
		wifi_hostname.hostname[i] = data->hostname[i];
	}

	wifi_write_config((char*)&wifi_hostname, sizeof(WIFIHostname), WIFI_HOSTNAME_POS);
	com_return_setter(com, data);
}

void get_wifi_hostname(const ComType com, const GetWifiHostname *data) {
	GetWifiHostnameReturn gwhr;

	WIFIHostname wifi_hostname;
	gwhr.header        = data->header;
	gwhr.header.length = sizeof(GetWifiHostnameReturn);

	wifi_read_config((char*)&wifi_hostname, sizeof(GetWifiHostnameReturn), WIFI_HOSTNAME_POS);
	if(wifi_hostname.key == WIFI_KEY) {
		memcpy(gwhr.hostname, wifi_hostname.hostname, 16);
	} else {
		memset(gwhr.hostname, '\0', 16);
	}

	send_blocking_with_timeout(&gwhr, sizeof(GetWifiHostnameReturn), com);
}

void set_stack_current_callback_period(const ComType com, const SetStackCurrentCallbackPeriod *data) {
	master_callback.period_stack_current = data->period;
	com_return_setter(com, data);
}

void get_stack_current_callback_period(const ComType com, const GetStackCurrentCallbackPeriod *data) {
	GetStackCurrentCallbackPeriodReturn gccpr;

	gccpr.header        = data->header;
	gccpr.header.length = sizeof(GetStackCurrentCallbackPeriodReturn);
	gccpr.period        = master_callback.period_stack_current;

	send_blocking_with_timeout(&gccpr, sizeof(GetStackCurrentCallbackPeriodReturn), com);
}

void set_stack_voltage_callback_period(const ComType com, const SetStackVoltageCallbackPeriod *data) {
	master_callback.period_stack_voltage = data->period;
	com_return_setter(com, data);
}

void get_stack_voltage_callback_period(const ComType com, const GetStackVoltageCallbackPeriod *data) {
	GetStackVoltageCallbackPeriodReturn gvcpr;

	gvcpr.header        = data->header;
	gvcpr.header.length = sizeof(GetStackVoltageCallbackPeriodReturn);
	gvcpr.period        = master_callback.period_stack_voltage;

	send_blocking_with_timeout(&gvcpr, sizeof(GetStackVoltageCallbackPeriodReturn), com);
}

void set_usb_voltage_callback_period(const ComType com, const SetUSBVoltageCallbackPeriod *data) {
	master_callback.period_usb_voltage = data->period;
	com_return_setter(com, data);
}

void get_usb_voltage_callback_period(const ComType com, const GetUSBVoltageCallbackPeriod *data) {
	GetUSBVoltageCallbackPeriodReturn gvcpr;

	gvcpr.header        = data->header;
	gvcpr.header.length = sizeof(GetUSBVoltageCallbackPeriodReturn);
	gvcpr.period        = master_callback.period_usb_voltage;

	send_blocking_with_timeout(&gvcpr, sizeof(GetUSBVoltageCallbackPeriodReturn), com);
}

void set_stack_current_callback_threshold(const ComType com, const SetStackCurrentCallbackThreshold *data) {
	master_callback.option_stack_current_save = data->option;
	master_callback.min_stack_current_save = data->min;
	master_callback.max_stack_current_save = data->max;

	if(data->option == 'o' ||
	   data->option == 'i' ||
	   data->option == 'x') {
		master_callback.option_stack_current = data->option;
		master_callback.min_stack_current = data->min;
		master_callback.max_stack_current = data->max;
	} else if(data->option == '<') {
		master_callback.option_stack_current = 'o';
		master_callback.min_stack_current = data->min;
		master_callback.max_stack_current = 0xFFFF;
	} else if(data->option == '>') {
		master_callback.option_stack_current = 'o';
		master_callback.min_stack_current = 0;
		master_callback.max_stack_current = data->min;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	master_callback.threshold_period_current_stack_current = master_callback.debounce_period;
	com_return_setter(com, data);
}

void get_stack_current_callback_threshold(const ComType com, const GetStackCurrentCallbackThreshold *data) {
	GetStackCurrentCallbackThresholdReturn gcctr;

	gcctr.header        = data->header;
	gcctr.header.length = sizeof(GetStackCurrentCallbackThresholdReturn);
	gcctr.option        = master_callback.option_stack_current_save;
	gcctr.min           = master_callback.min_stack_current_save;
	gcctr.max           = master_callback.max_stack_current_save;

	send_blocking_with_timeout(&gcctr, sizeof(GetStackCurrentCallbackThresholdReturn), com);
}

void set_stack_voltage_callback_threshold(const ComType com, const SetStackVoltageCallbackThreshold *data) {
	master_callback.option_stack_voltage_save = data->option;
	master_callback.min_stack_voltage_save = data->min;
	master_callback.max_stack_voltage_save = data->max;

	if(data->option == 'o' ||
	   data->option == 'i' ||
	   data->option == 'x') {
		master_callback.option_stack_voltage = data->option;
		master_callback.min_stack_voltage = data->min;
		master_callback.max_stack_voltage = data->max;
	} else if(data->option == '<') {
		master_callback.option_stack_voltage = 'o';
		master_callback.min_stack_voltage = data->min;
		master_callback.max_stack_voltage = 0xFFFF;
	} else if(data->option == '>') {
		master_callback.option_stack_voltage = 'o';
		master_callback.min_stack_voltage = 0;
		master_callback.max_stack_voltage = data->min;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	master_callback.threshold_period_current_stack_voltage = master_callback.debounce_period;
	com_return_setter(com, data);
}

void get_stack_voltage_callback_threshold(const ComType com, const GetStackVoltageCallbackThreshold *data) {
	GetStackVoltageCallbackThresholdReturn gvctr;

	gvctr.header        = data->header;
	gvctr.header.length = sizeof(GetStackVoltageCallbackThresholdReturn);
	gvctr.option        = master_callback.option_stack_voltage_save;
	gvctr.min           = master_callback.min_stack_voltage_save;
	gvctr.max           = master_callback.max_stack_voltage_save;

	send_blocking_with_timeout(&gvctr, sizeof(GetStackVoltageCallbackThresholdReturn), com);
}

void set_usb_voltage_callback_threshold(const ComType com, const SetUSBVoltageCallbackThreshold *data) {
	master_callback.option_usb_voltage_save = data->option;
	master_callback.min_usb_voltage_save = data->min;
	master_callback.max_usb_voltage_save = data->max;

	if(data->option == 'o' ||
	   data->option == 'i' ||
	   data->option == 'x') {
		master_callback.option_usb_voltage = data->option;
		master_callback.min_usb_voltage = data->min;
		master_callback.max_usb_voltage = data->max;
	} else if(data->option == '<') {
		master_callback.option_usb_voltage = 'o';
		master_callback.min_usb_voltage = data->min;
		master_callback.max_usb_voltage = 0xFFFF;
	} else if(data->option == '>') {
		master_callback.option_usb_voltage = 'o';
		master_callback.min_usb_voltage = 0;
		master_callback.max_usb_voltage = data->min;
	} else {
		com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	master_callback.threshold_period_current_stack_voltage = master_callback.debounce_period;
	com_return_setter(com, data);
}

void get_usb_voltage_callback_threshold(const ComType com, const GetUSBVoltageCallbackThreshold *data) {
	GetUSBVoltageCallbackThresholdReturn gvctr;

	gvctr.header        = data->header;
	gvctr.header.length = sizeof(GetUSBVoltageCallbackThresholdReturn);
	gvctr.option        = master_callback.option_usb_voltage_save;
	gvctr.min           = master_callback.min_usb_voltage_save;
	gvctr.max           = master_callback.max_usb_voltage_save;

	send_blocking_with_timeout(&gvctr, sizeof(GetUSBVoltageCallbackThresholdReturn), com);
}

void set_debounce_period(const ComType com, const SetDebouncePeriod *data) {
	master_callback.debounce_period = data->debounce;
	com_return_setter(com, data);
}

void get_debounce_period(const ComType com, const GetDebouncePeriod *data) {
	GetDebouncePeriodReturn gdpr;

	gdpr.header        = data->header;
	gdpr.header.length = sizeof(GetDebouncePeriodReturn);
	gdpr.debounce      = master_callback.debounce_period;

	send_blocking_with_timeout(&gdpr, sizeof(GetDebouncePeriodReturn), com);
}

void is_ethernet_present(const ComType com, const IsEthernetPresent *data) {
	IsEthernetPresentReturn iepr;

	iepr.header        = data->header;
	iepr.header.length = sizeof(IsEthernetPresentReturn);
	iepr.present       = com_info.ext[0] == COM_ETHERNET || com_info.ext[1] == COM_ETHERNET;

	send_blocking_with_timeout(&iepr, sizeof(IsEthernetPresentReturn), com);

	logwifii("is_ethernet_present: %d\n\r", iepr.present);
}

void set_ethernet_configuration(const ComType com, const SetEthernetConfiguration *data) {
	ethernet_write_config(((char*)data) + sizeof(MessageHeader),
	                      sizeof(SetEthernetConfiguration) - sizeof(MessageHeader),
	                      ETHERNET_CONFIGURATION_POS);

	logwifii("set_ethernet_configuration: %d, %d.%d.%d.%d:%d\n\r", data->connection,
	                                                               data->ip[3],
	                                                               data->ip[2],
	                                                               data->ip[1],
	                                                               data->ip[0],
	                                                               data->port);

	com_return_setter(com, data);
}

void get_ethernet_configuration(const ComType com, const GetEthernetConfiguration *data) {
	GetEthernetConfigurationReturn gecr;

	gecr.header        = data->header;
	gecr.header.length = sizeof(GetEthernetConfigurationReturn);

	ethernet_read_config(((char*)&gecr) + sizeof(MessageHeader),
	                     sizeof(GetEthernetConfigurationReturn) - sizeof(MessageHeader),
	                     ETHERNET_CONFIGURATION_POS);

	send_blocking_with_timeout(&gecr, sizeof(GetEthernetConfigurationReturn), com);

	logwifii("get_ethernet_configuration: %d, %d.%d.%d.%d:%d\n\r", gecr.connection,
	                                                               gecr.ip[3],
	                                                               gecr.ip[2],
	                                                               gecr.ip[1],
	                                                               gecr.ip[0],
	                                                               gecr.port);
}

void get_ethernet_status(const ComType com, const GetEthernetStatus *data) {
	GetEthernetStatusReturn gesr;

	gesr.header        = data->header;
	gesr.header.length = sizeof(GetEthernetStatusReturn);
	memcpy(gesr.mac_address, &ethernet_status, sizeof(EthernetStatus));

	send_blocking_with_timeout(&gesr, sizeof(GetEthernetStatusReturn), com);
}

void set_ethernet_hostname(const ComType com, const SetEthernetHostname *data) {
	char hostname[ETHERNET_HOSTNAME_LENGTH] = {'\0'};

	uint8_t i = 0;
	for(; i < ETHERNET_HOSTNAME_LENGTH; i++) {
		if(data->hostname[i] == '\0') {
			break;
		}
		hostname[i] = data->hostname[i];
	}

	if(i == 0) {
		ethernet_low_level_get_default_hostname(hostname);
	}

	ethernet_write_config(hostname, ETHERNET_HOSTNAME_LENGTH, ETHERNET_HOSTNAME_POS);
	com_return_setter(com, data);
}

void set_ethernet_mac(const ComType com, const SetEthernetMAC *data) {
	ethernet_write_config((const char *)data->mac_address,
	                      sizeof(SetEthernetMAC) - sizeof(MessageHeader),
	                      ETHERNET_MAC_POS);
	com_return_setter(com, data);
}
