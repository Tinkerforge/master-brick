/* master-brick
 * Copyright (C) 2010-2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: Implementation of Master-Brick specific messages
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


#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#define TYPE_GET_STACK_VOLTAGE 1
#define TYPE_GET_STACK_CURRENT 2
#define TYPE_SET_EXTENSION_TYPE 3
#define TYPE_GET_EXTENSION_TYPE 4
#define TYPE_IS_CHIBI_PRESENT 5
#define TYPE_SET_CHIBI_ADDRESS 6
#define TYPE_GET_CHIBI_ADDRESS 7
#define TYPE_SET_CHIBI_MASTER_ADDRESS 8
#define TYPE_GET_CHIBI_MASTER_ADDRESS 9
#define TYPE_SET_CHIBI_SLAVE_ADDRESS 10
#define TYPE_GET_CHIBI_SLAVE_ADDRESS 11
#define TYPE_GET_CHIBI_SIGNAL_STRENGTH 12
#define TYPE_GET_CHIBI_ERROR_LOG 13
#define TYPE_SET_CHIBI_FREQUENCY 14
#define TYPE_GET_CHIBI_FREQUENCY 15
#define TYPE_SET_CHIBI_CHANNEL 16
#define TYPE_GET_CHIBI_CHANNEL 17
#define TYPE_IS_RS485_PRESENT 18
#define TYPE_SET_RS485_ADDRESS 19
#define TYPE_GET_RS485_ADDRESS 20
#define TYPE_SET_RS485_SLAVE_ADDRESS 21
#define TYPE_GET_RS485_SLAVE_ADDRESS 22
#define TYPE_GET_RS485_ERROR_LOG 23
#define TYPE_SET_RS485_CONFIGURATION 24
#define TYPE_GET_RS485_CONFIGURATION 25
#define TYPE_IS_WIFI_PRESENT 26
#define TYPE_SET_WIFI_CONFIGURATION 27
#define TYPE_GET_WIFI_CONFIGURATION 28
#define TYPE_SET_WIFI_ENCRYPTION 29
#define TYPE_GET_WIFI_ENCRYPTION 30
#define TYPE_GET_WIFI_STATUS 31
#define TYPE_REFRESH_WIFI_STATUS 32
#define TYPE_SET_WIFI_CERTIFICATE 33
#define TYPE_GET_WIFI_CERTIFICATE 34
#define TYPE_SET_WIFI_POWER_MODE 35
#define TYPE_GET_WIFI_POWER_MODE 36

#define COM_MESSAGES_USER \
	{TYPE_GET_STACK_VOLTAGE, (message_handler_func_t)get_stack_voltage}, \
	{TYPE_GET_STACK_CURRENT, (message_handler_func_t)get_stack_current}, \
	{TYPE_SET_EXTENSION_TYPE, (message_handler_func_t)set_extension_type}, \
	{TYPE_GET_EXTENSION_TYPE, (message_handler_func_t)get_extension_type}, \
	{TYPE_IS_CHIBI_PRESENT, (message_handler_func_t)is_chibi_present}, \
	{TYPE_SET_CHIBI_ADDRESS, (message_handler_func_t)set_chibi_address}, \
	{TYPE_GET_CHIBI_ADDRESS, (message_handler_func_t)get_chibi_address}, \
	{TYPE_SET_CHIBI_MASTER_ADDRESS, (message_handler_func_t)set_chibi_master_address}, \
	{TYPE_GET_CHIBI_MASTER_ADDRESS, (message_handler_func_t)get_chibi_master_address}, \
	{TYPE_SET_CHIBI_SLAVE_ADDRESS, (message_handler_func_t)set_chibi_slave_address}, \
	{TYPE_GET_CHIBI_SLAVE_ADDRESS, (message_handler_func_t)get_chibi_slave_address}, \
	{TYPE_GET_CHIBI_SIGNAL_STRENGTH, (message_handler_func_t)get_chibi_signal_strength}, \
	{TYPE_GET_CHIBI_ERROR_LOG, (message_handler_func_t)get_chibi_error_log}, \
	{TYPE_SET_CHIBI_FREQUENCY, (message_handler_func_t)set_chibi_frequency}, \
	{TYPE_GET_CHIBI_FREQUENCY, (message_handler_func_t)get_chibi_frequency}, \
	{TYPE_SET_CHIBI_CHANNEL, (message_handler_func_t)set_chibi_channel}, \
	{TYPE_GET_CHIBI_CHANNEL, (message_handler_func_t)get_chibi_channel}, \
	{TYPE_IS_RS485_PRESENT, (message_handler_func_t)is_rs485_present}, \
	{TYPE_SET_RS485_ADDRESS, (message_handler_func_t)set_rs485_address}, \
	{TYPE_GET_RS485_ADDRESS, (message_handler_func_t)get_rs485_address}, \
	{TYPE_SET_RS485_SLAVE_ADDRESS, (message_handler_func_t)set_rs485_slave_address}, \
	{TYPE_GET_RS485_SLAVE_ADDRESS, (message_handler_func_t)get_rs485_slave_address}, \
	{TYPE_GET_RS485_ERROR_LOG, (message_handler_func_t)get_rs485_error_log}, \
	{TYPE_SET_RS485_CONFIGURATION, (message_handler_func_t)set_rs485_configuration}, \
	{TYPE_GET_RS485_CONFIGURATION, (message_handler_func_t)get_rs485_configuration}, \
	{TYPE_IS_WIFI_PRESENT, (message_handler_func_t)is_wifi_present}, \
	{TYPE_SET_WIFI_CONFIGURATION, (message_handler_func_t)set_wifi_configuration}, \
	{TYPE_GET_WIFI_CONFIGURATION, (message_handler_func_t)get_wifi_configuration}, \
	{TYPE_SET_WIFI_ENCRYPTION, (message_handler_func_t)set_wifi_encryption}, \
	{TYPE_GET_WIFI_ENCRYPTION, (message_handler_func_t)get_wifi_encryption}, \
	{TYPE_GET_WIFI_STATUS, (message_handler_func_t)get_wifi_status}, \
	{TYPE_REFRESH_WIFI_STATUS, (message_handler_func_t)refresh_wifi_status}, \
	{TYPE_SET_WIFI_CERTIFICATE, (message_handler_func_t)set_wifi_certificate}, \
	{TYPE_GET_WIFI_CERTIFICATE, (message_handler_func_t)get_wifi_certificate}, \
	{TYPE_SET_WIFI_POWER_MODE, (message_handler_func_t)set_wifi_power_mode}, \
	{TYPE_GET_WIFI_POWER_MODE, (message_handler_func_t)get_wifi_power_mode},


typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetStackVoltage;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t voltage;
} __attribute__((__packed__)) GetStackVoltageReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetStackCurrent;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t current;
} __attribute__((__packed__)) GetStackCurrentReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t extension;
	uint32_t exttype;
} __attribute__((__packed__)) SetExtensionType;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t extension;
} __attribute__((__packed__)) GetExtensionType;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint32_t exttype;
} __attribute__((__packed__)) GetExtensionTypeReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) IsChibiPresent;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	bool present;
} __attribute__((__packed__)) IsChibiPresentReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) SetChibiAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetChibiAddressReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) SetChibiMasterAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiMasterAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetChibiMasterAddressReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t num;
	uint8_t address;
} __attribute__((__packed__)) SetChibiSlaveAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t num;
} __attribute__((__packed__)) GetChibiSlaveAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetChibiSlaveAddressReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiSignalStrength;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t signal_strength;
} __attribute__((__packed__)) GetChibiSignalStrengthReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiErrorLog;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t underrun;
	uint16_t crc_error;
	uint16_t no_ack;
	uint16_t overflow;
} __attribute__((__packed__)) GetChibiErrorLogReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t frequency;
} __attribute__((__packed__)) SetChibiFrequency;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiFrequency;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t frequency;
} __attribute__((__packed__)) GetChibiFrequencyReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t channel;
} __attribute__((__packed__)) SetChibiChannel;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetChibiChannel;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t channel;
} __attribute__((__packed__)) GetChibiChannelReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) IsRS485Present;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	bool present;
} __attribute__((__packed__)) IsRS485PresentReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) SetRS485Address;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetRS485Address;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetRS485AddressReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t num;
	uint8_t address;
} __attribute__((__packed__)) SetRS485SlaveAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t num;
} __attribute__((__packed__)) GetRS485SlaveAddress;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t address;
} __attribute__((__packed__)) GetRS485SlaveAddressReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetRS485ErrorLog;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t crc_error;
} __attribute__((__packed__)) GetRS485ErrorLogReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint32_t speed;
	char parity;
	uint8_t stopbits;
} __attribute__((__packed__)) SetRS485Configuration;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetRS485Configuration;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint32_t speed;
	char parity;
	uint8_t stopbits;
} __attribute__((__packed__)) GetRS485ConfigurationReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) IsWifiPresent;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	bool present;
} __attribute__((__packed__)) IsWifiPresentReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	char ssid[32];
	uint8_t connection;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint16_t port;
} __attribute__((__packed__)) SetWifiConfiguration;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetWifiConfiguration;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	char ssid[32];
	uint8_t connection;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint16_t port;
} __attribute__((__packed__)) GetWifiConfigurationReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t encryption;
	char key[50];
	uint8_t key_index;
	uint8_t eap_options;
	uint16_t ca_certificate_length;
	uint16_t client_certificate_length;
	uint16_t private_key_length;
} __attribute__((__packed__)) SetWifiEncryption;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetWifiEncryption;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t encryption;
	char key[50];
	uint8_t key_index;
	uint8_t eap_options;
	uint16_t ca_certificate_length;
	uint16_t client_certificate_length;
	uint16_t private_key_length;
} __attribute__((__packed__)) GetWifiEncryptionReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetWifiStatus;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t mac_address[6];
	uint8_t bssid[6];
	uint8_t channel;
	int16_t rssi;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint32_t rx_count;
	uint32_t tx_count;
	uint8_t last_error;
} __attribute__((__packed__)) GetWifiStatusReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) RefreshWifiStatus;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t index;
	char data[32];
	uint8_t data_length;
} __attribute__((__packed__)) SetWifiCertificate;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint16_t index;
} __attribute__((__packed__)) GetWifiCertificate;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	char data[32];
	uint8_t data_length;
} __attribute__((__packed__)) GetWifiCertificateReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t mode;
} __attribute__((__packed__)) SetWifiPowerMode;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetWifiPowerMode;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t mode;
} __attribute__((__packed__)) GetWifiPowerModeReturn;

void get_stack_voltage(uint8_t com, const GetStackVoltage *data);
void get_stack_current(uint8_t com, const GetStackCurrent *data);
void set_extension_type(uint8_t com, const SetExtensionType *data);
void get_extension_type(uint8_t com, const GetExtensionType *data);
void is_chibi_present(uint8_t com, const IsChibiPresent *data);
void set_chibi_address(uint8_t com, const SetChibiAddress *data);
void get_chibi_address(uint8_t com, const GetChibiAddress *data);
void set_chibi_master_address(uint8_t com, const SetChibiMasterAddress *data);
void get_chibi_master_address(uint8_t com, const GetChibiMasterAddress *data);
void set_chibi_slave_address(uint8_t com, const SetChibiSlaveAddress *data);
void get_chibi_slave_address(uint8_t com, const GetChibiSlaveAddress *data);
void get_chibi_signal_strength(uint8_t com, const GetChibiSignalStrength *data);
void get_chibi_error_log(uint8_t com, const GetChibiErrorLog *data);
void set_chibi_frequency(uint8_t com, const SetChibiFrequency *data);
void get_chibi_frequency(uint8_t com, const GetChibiFrequency *data);
void set_chibi_channel(uint8_t com, const SetChibiChannel *data);
void get_chibi_channel(uint8_t com, const GetChibiChannel *data);
void is_rs485_present(uint8_t com, const IsRS485Present *data);
void set_rs485_address(uint8_t com, const SetRS485Address *data);
void get_rs485_address(uint8_t com, const GetRS485Address *data);
void set_rs485_slave_address(uint8_t com, const SetRS485SlaveAddress *data);
void get_rs485_slave_address(uint8_t com, const GetRS485SlaveAddress *data);
void get_rs485_error_log(uint8_t com, const GetRS485ErrorLog *data);
void set_rs485_configuration(uint8_t com, const SetRS485Configuration *data);
void get_rs485_configuration(uint8_t com, const GetRS485Configuration *data);
void is_wifi_present(uint8_t com, const IsWifiPresent *data);
void set_wifi_configuration(uint8_t com, const SetWifiConfiguration *data);
void get_wifi_configuration(uint8_t com, const GetWifiConfiguration *data);
void set_wifi_encryption(uint8_t com, const SetWifiEncryption *data);
void get_wifi_encryption(uint8_t com, const GetWifiEncryption *data);
void get_wifi_status(uint8_t com, const GetWifiStatus *data);
void refresh_wifi_status(uint8_t com, const RefreshWifiStatus *data);
void set_wifi_certificate(uint8_t com, const SetWifiCertificate *data);
void get_wifi_certificate(uint8_t com, const GetWifiCertificate *data);
void set_wifi_power_mode(uint8_t com, const SetWifiPowerMode *data);
void get_wifi_power_mode(uint8_t com, const GetWifiPowerMode *data);

#endif
