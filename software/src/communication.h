/* master-brick
 * Copyright (C) 2010-2014 Olaf Lüke <olaf@tinkerforge.com>
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

#include "bricklib/com/com_common.h"

#define AUTHENTICATION_SECRET_LENGTH 64

#define FID_GET_STACK_VOLTAGE 1
#define FID_GET_STACK_CURRENT 2
#define FID_SET_EXTENSION_TYPE 3
#define FID_GET_EXTENSION_TYPE 4
#define FID_IS_CHIBI_PRESENT 5
#define FID_SET_CHIBI_ADDRESS 6
#define FID_GET_CHIBI_ADDRESS 7
#define FID_SET_CHIBI_MASTER_ADDRESS 8
#define FID_GET_CHIBI_MASTER_ADDRESS 9
#define FID_SET_CHIBI_SLAVE_ADDRESS 10
#define FID_GET_CHIBI_SLAVE_ADDRESS 11
#define FID_GET_CHIBI_SIGNAL_STRENGTH 12
#define FID_GET_CHIBI_ERROR_LOG 13
#define FID_SET_CHIBI_FREQUENCY 14
#define FID_GET_CHIBI_FREQUENCY 15
#define FID_SET_CHIBI_CHANNEL 16
#define FID_GET_CHIBI_CHANNEL 17
#define FID_IS_RS485_PRESENT 18
#define FID_SET_RS485_ADDRESS 19
#define FID_GET_RS485_ADDRESS 20
#define FID_SET_RS485_SLAVE_ADDRESS 21
#define FID_GET_RS485_SLAVE_ADDRESS 22
#define FID_GET_RS485_ERROR_LOG 23
#define FID_SET_RS485_CONFIGURATION 24
#define FID_GET_RS485_CONFIGURATION 25
#define FID_IS_WIFI_PRESENT 26
#define FID_SET_WIFI_CONFIGURATION 27
#define FID_GET_WIFI_CONFIGURATION 28
#define FID_SET_WIFI_ENCRYPTION 29
#define FID_GET_WIFI_ENCRYPTION 30
#define FID_GET_WIFI_STATUS 31
#define FID_REFRESH_WIFI_STATUS 32
#define FID_SET_WIFI_CERTIFICATE 33
#define FID_GET_WIFI_CERTIFICATE 34
#define FID_SET_WIFI_POWER_MODE 35
#define FID_GET_WIFI_POWER_MODE 36
#define FID_GET_WIFI_BUFFER_INFO 37
#define FID_SET_WIFI_REGULATORY_DOMAIN 38
#define FID_GET_WIFI_REGULATORY_DOMAIN 39
#define FID_GET_USB_VOLTAGE 40
#define FID_SET_LONG_WIFI_KEY 41
#define FID_GET_LONG_WIFI_KEY 42
#define FID_SET_WIFI_HOSTNAME 43
#define FID_GET_WIFI_HOSTNAME 44
#define FID_SET_STACK_CURRENT_CALLBACK_PERIOD 45
#define FID_GET_STACK_CURRENT_CALLBACK_PERIOD 46
#define FID_SET_STACK_VOLTAGE_CALLBACK_PERIOD 47
#define FID_GET_STACK_VOLTAGE_CALLBACK_PERIOD 48
#define FID_SET_USB_VOLTAGE_CALLBACK_PERIOD 49
#define FID_GET_USB_VOLTAGE_CALLBACK_PERIOD 50
#define FID_SET_STACK_CURRENT_CALLBACK_THRESHOLD 51
#define FID_GET_STACK_CURRENT_CALLBACK_THRESHOLD 52
#define FID_SET_STACK_VOLTAGE_CALLBACK_THRESHOLD 53
#define FID_GET_STACK_VOLTAGE_CALLBACK_THRESHOLD 54
#define FID_SET_USB_VOLTAGE_CALLBACK_THRESHOLD 55
#define FID_GET_USB_VOLTAGE_CALLBACK_THRESHOLD 56
#define FID_SET_DEBOUNCE_PERIOD 57
#define FID_GET_DEBOUNCE_PERIOD 58
#define FID_STACK_CURRENT 59
#define FID_STACK_VOLTAGE 60
#define FID_USB_VOLTAGE 61
#define FID_STACK_CURRENT_REACHED 62
#define FID_STACK_VOLTAGE_REACHED 63
#define FID_USB_VOLTAGE_REACHED 64
#define FID_IS_ETHERNET_PRESENT 65
#define FID_SET_ETHERNET_CONFIGURATION 66
#define FID_GET_ETHERNET_CONFIGURATION 67
#define FID_GET_ETHERNET_STATUS 68
#define FID_SET_ETHERNET_HOSTNAME 69
#define FID_SET_ETHERNET_MAC 70
#define FID_SET_ETHERNET_WEBSOCKET_CONFIGURATION 71
#define FID_GET_ETHERNET_WEBSOCKET_CONFIGURATION 72
#define FID_SET_ETHERNET_AUTHENTICATION_SECRET 73
#define FID_GET_ETHERNET_AUTHENTICATION_SECRET 74


#define COM_MESSAGES_USER \
	{FID_GET_STACK_VOLTAGE, (message_handler_func_t)get_stack_voltage}, \
	{FID_GET_STACK_CURRENT, (message_handler_func_t)get_stack_current}, \
	{FID_SET_EXTENSION_TYPE, (message_handler_func_t)set_extension_type}, \
	{FID_GET_EXTENSION_TYPE, (message_handler_func_t)get_extension_type}, \
	{FID_IS_CHIBI_PRESENT, (message_handler_func_t)is_chibi_present}, \
	{FID_SET_CHIBI_ADDRESS, (message_handler_func_t)set_chibi_address}, \
	{FID_GET_CHIBI_ADDRESS, (message_handler_func_t)get_chibi_address}, \
	{FID_SET_CHIBI_MASTER_ADDRESS, (message_handler_func_t)set_chibi_master_address}, \
	{FID_GET_CHIBI_MASTER_ADDRESS, (message_handler_func_t)get_chibi_master_address}, \
	{FID_SET_CHIBI_SLAVE_ADDRESS, (message_handler_func_t)set_chibi_slave_address}, \
	{FID_GET_CHIBI_SLAVE_ADDRESS, (message_handler_func_t)get_chibi_slave_address}, \
	{FID_GET_CHIBI_SIGNAL_STRENGTH, (message_handler_func_t)get_chibi_signal_strength}, \
	{FID_GET_CHIBI_ERROR_LOG, (message_handler_func_t)get_chibi_error_log}, \
	{FID_SET_CHIBI_FREQUENCY, (message_handler_func_t)set_chibi_frequency}, \
	{FID_GET_CHIBI_FREQUENCY, (message_handler_func_t)get_chibi_frequency}, \
	{FID_SET_CHIBI_CHANNEL, (message_handler_func_t)set_chibi_channel}, \
	{FID_GET_CHIBI_CHANNEL, (message_handler_func_t)get_chibi_channel}, \
	{FID_IS_RS485_PRESENT, (message_handler_func_t)is_rs485_present}, \
	{FID_SET_RS485_ADDRESS, (message_handler_func_t)set_rs485_address}, \
	{FID_GET_RS485_ADDRESS, (message_handler_func_t)get_rs485_address}, \
	{FID_SET_RS485_SLAVE_ADDRESS, (message_handler_func_t)set_rs485_slave_address}, \
	{FID_GET_RS485_SLAVE_ADDRESS, (message_handler_func_t)get_rs485_slave_address}, \
	{FID_GET_RS485_ERROR_LOG, (message_handler_func_t)get_rs485_error_log}, \
	{FID_SET_RS485_CONFIGURATION, (message_handler_func_t)set_rs485_configuration}, \
	{FID_GET_RS485_CONFIGURATION, (message_handler_func_t)get_rs485_configuration}, \
	{FID_IS_WIFI_PRESENT, (message_handler_func_t)is_wifi_present}, \
	{FID_SET_WIFI_CONFIGURATION, (message_handler_func_t)set_wifi_configuration}, \
	{FID_GET_WIFI_CONFIGURATION, (message_handler_func_t)get_wifi_configuration}, \
	{FID_SET_WIFI_ENCRYPTION, (message_handler_func_t)set_wifi_encryption}, \
	{FID_GET_WIFI_ENCRYPTION, (message_handler_func_t)get_wifi_encryption}, \
	{FID_GET_WIFI_STATUS, (message_handler_func_t)get_wifi_status}, \
	{FID_REFRESH_WIFI_STATUS, (message_handler_func_t)refresh_wifi_status}, \
	{FID_SET_WIFI_CERTIFICATE, (message_handler_func_t)set_wifi_certificate}, \
	{FID_GET_WIFI_CERTIFICATE, (message_handler_func_t)get_wifi_certificate}, \
	{FID_SET_WIFI_POWER_MODE, (message_handler_func_t)set_wifi_power_mode}, \
	{FID_GET_WIFI_POWER_MODE, (message_handler_func_t)get_wifi_power_mode}, \
 	{FID_GET_WIFI_BUFFER_INFO, (message_handler_func_t)get_wifi_buffer_info}, \
	{FID_SET_WIFI_REGULATORY_DOMAIN, (message_handler_func_t)set_wifi_regulatory_domain}, \
	{FID_GET_WIFI_REGULATORY_DOMAIN, (message_handler_func_t)get_wifi_regulatory_domain}, \
	{FID_GET_USB_VOLTAGE, (message_handler_func_t)get_usb_voltage}, \
	{FID_SET_LONG_WIFI_KEY, (message_handler_func_t)set_long_wifi_key}, \
	{FID_GET_LONG_WIFI_KEY, (message_handler_func_t)get_long_wifi_key}, \
	{FID_SET_WIFI_HOSTNAME, (message_handler_func_t)set_wifi_hostname}, \
	{FID_GET_WIFI_HOSTNAME, (message_handler_func_t)get_wifi_hostname}, \
	{FID_SET_STACK_CURRENT_CALLBACK_PERIOD, (message_handler_func_t)set_stack_current_callback_period}, \
	{FID_GET_STACK_CURRENT_CALLBACK_PERIOD, (message_handler_func_t)get_stack_current_callback_period}, \
	{FID_SET_STACK_VOLTAGE_CALLBACK_PERIOD, (message_handler_func_t)set_stack_voltage_callback_period}, \
	{FID_GET_STACK_VOLTAGE_CALLBACK_PERIOD, (message_handler_func_t)get_stack_voltage_callback_period}, \
	{FID_SET_USB_VOLTAGE_CALLBACK_PERIOD, (message_handler_func_t)set_usb_voltage_callback_period}, \
	{FID_GET_USB_VOLTAGE_CALLBACK_PERIOD, (message_handler_func_t)get_usb_voltage_callback_period}, \
	{FID_SET_STACK_CURRENT_CALLBACK_THRESHOLD, (message_handler_func_t)set_stack_current_callback_threshold}, \
	{FID_GET_STACK_CURRENT_CALLBACK_THRESHOLD, (message_handler_func_t)get_stack_current_callback_threshold}, \
	{FID_SET_STACK_VOLTAGE_CALLBACK_THRESHOLD, (message_handler_func_t)set_stack_voltage_callback_threshold}, \
	{FID_GET_STACK_VOLTAGE_CALLBACK_THRESHOLD, (message_handler_func_t)get_stack_voltage_callback_threshold}, \
	{FID_SET_USB_VOLTAGE_CALLBACK_THRESHOLD, (message_handler_func_t)set_usb_voltage_callback_threshold}, \
	{FID_GET_USB_VOLTAGE_CALLBACK_THRESHOLD, (message_handler_func_t)get_usb_voltage_callback_threshold}, \
	{FID_SET_DEBOUNCE_PERIOD, (message_handler_func_t)set_debounce_period}, \
	{FID_GET_DEBOUNCE_PERIOD, (message_handler_func_t)get_debounce_period}, \
	{FID_STACK_CURRENT, (message_handler_func_t)NULL}, \
	{FID_STACK_VOLTAGE, (message_handler_func_t)NULL}, \
	{FID_USB_VOLTAGE, (message_handler_func_t)NULL}, \
	{FID_STACK_CURRENT_REACHED, (message_handler_func_t)NULL}, \
	{FID_STACK_VOLTAGE_REACHED, (message_handler_func_t)NULL}, \
	{FID_USB_VOLTAGE_REACHED, (message_handler_func_t)NULL}, \
	{FID_IS_ETHERNET_PRESENT, (message_handler_func_t)is_ethernet_present}, \
	{FID_SET_ETHERNET_CONFIGURATION, (message_handler_func_t)set_ethernet_configuration}, \
	{FID_GET_ETHERNET_CONFIGURATION, (message_handler_func_t)get_ethernet_configuration}, \
	{FID_GET_ETHERNET_STATUS, (message_handler_func_t)get_ethernet_status}, \
	{FID_SET_ETHERNET_HOSTNAME, (message_handler_func_t)set_ethernet_hostname}, \
	{FID_SET_ETHERNET_MAC, (message_handler_func_t)set_ethernet_mac}, \
	{FID_SET_ETHERNET_WEBSOCKET_CONFIGURATION, (message_handler_func_t)set_ethernet_websocket_configuration}, \
	{FID_GET_ETHERNET_WEBSOCKET_CONFIGURATION, (message_handler_func_t)get_ethernet_websocket_configuration}, \
	{FID_SET_ETHERNET_AUTHENTICATION_SECRET, (message_handler_func_t)set_ethernet_authentication_secret}, \
	{FID_GET_ETHERNET_AUTHENTICATION_SECRET, (message_handler_func_t)get_ethernet_authentication_secret},

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetStackVoltage;

typedef struct {
	MessageHeader header;
	uint16_t voltage;
} __attribute__((__packed__)) GetStackVoltageReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetStackCurrent;

typedef struct {
	MessageHeader header;
	uint16_t current;
} __attribute__((__packed__)) GetStackCurrentReturn;

typedef struct {
	MessageHeader header;
	uint8_t extension;
	uint32_t exttype;
} __attribute__((__packed__)) SetExtensionType;

typedef struct {
	MessageHeader header;
	uint8_t extension;
} __attribute__((__packed__)) GetExtensionType;

typedef struct {
	MessageHeader header;
	uint32_t exttype;
} __attribute__((__packed__)) GetExtensionTypeReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) IsChibiPresent;

typedef struct {
	MessageHeader header;
	bool present;
} __attribute__((__packed__)) IsChibiPresentReturn;

typedef struct {
	MessageHeader header;
	uint8_t address;
} __attribute__((__packed__)) SetChibiAddress;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetChibiAddress;

typedef struct {
	MessageHeader header;
	uint8_t address;
} __attribute__((__packed__)) GetChibiAddressReturn;

typedef struct {
	MessageHeader header;
	uint8_t address;
} __attribute__((__packed__)) SetChibiMasterAddress;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetChibiMasterAddress;

typedef struct {
	MessageHeader header;
	uint8_t address;
} __attribute__((__packed__)) GetChibiMasterAddressReturn;

typedef struct {
	MessageHeader header;
	uint8_t num;
	uint8_t address;
} __attribute__((__packed__)) SetChibiSlaveAddress;

typedef struct {
	MessageHeader header;
	uint8_t num;
} __attribute__((__packed__)) GetChibiSlaveAddress;

typedef struct {
	MessageHeader header;
	uint8_t address;
} __attribute__((__packed__)) GetChibiSlaveAddressReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetChibiSignalStrength;

typedef struct {
	MessageHeader header;
	uint8_t signal_strength;
} __attribute__((__packed__)) GetChibiSignalStrengthReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetChibiErrorLog;

typedef struct {
	MessageHeader header;
	uint16_t underrun;
	uint16_t crc_error;
	uint16_t no_ack;
	uint16_t overflow;
} __attribute__((__packed__)) GetChibiErrorLogReturn;

typedef struct {
	MessageHeader header;
	uint8_t frequency;
} __attribute__((__packed__)) SetChibiFrequency;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetChibiFrequency;

typedef struct {
	MessageHeader header;
	uint8_t frequency;
} __attribute__((__packed__)) GetChibiFrequencyReturn;

typedef struct {
	MessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) SetChibiChannel;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetChibiChannel;

typedef struct {
	MessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetChibiChannelReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) IsRS485Present;

typedef struct {
	MessageHeader header;
	bool present;
} __attribute__((__packed__)) IsRS485PresentReturn;

typedef struct {
	MessageHeader header;
	uint8_t address;
} __attribute__((__packed__)) SetRS485Address;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetRS485Address;

typedef struct {
	MessageHeader header;
	uint8_t address;
} __attribute__((__packed__)) GetRS485AddressReturn;

typedef struct {
	MessageHeader header;
	uint8_t num;
	uint8_t address;
} __attribute__((__packed__)) SetRS485SlaveAddress;

typedef struct {
	MessageHeader header;
	uint8_t num;
} __attribute__((__packed__)) GetRS485SlaveAddress;

typedef struct {
	MessageHeader header;
	uint8_t address;
} __attribute__((__packed__)) GetRS485SlaveAddressReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetRS485ErrorLog;

typedef struct {
	MessageHeader header;
	uint16_t crc_error;
} __attribute__((__packed__)) GetRS485ErrorLogReturn;

typedef struct {
	MessageHeader header;
	uint32_t speed;
	char parity;
	uint8_t stopbits;
} __attribute__((__packed__)) SetRS485Configuration;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetRS485Configuration;

typedef struct {
	MessageHeader header;
	uint32_t speed;
	char parity;
	uint8_t stopbits;
} __attribute__((__packed__)) GetRS485ConfigurationReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) IsWifiPresent;

typedef struct {
	MessageHeader header;
	bool present;
} __attribute__((__packed__)) IsWifiPresentReturn;

typedef struct {
	MessageHeader header;
	char ssid[32];
	uint8_t connection;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint16_t port;
} __attribute__((__packed__)) SetWifiConfiguration;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetWifiConfiguration;

typedef struct {
	MessageHeader header;
	char ssid[32];
	uint8_t connection;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint16_t port;
} __attribute__((__packed__)) GetWifiConfigurationReturn;

typedef struct {
	MessageHeader header;
	uint8_t encryption;
	char key[50];
	uint8_t key_index;
	uint8_t eap_options;
	uint16_t ca_certificate_length;
	uint16_t client_certificate_length;
	uint16_t private_key_length;
} __attribute__((__packed__)) SetWifiEncryption;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetWifiEncryption;

typedef struct {
	MessageHeader header;
	uint8_t encryption;
	char key[50];
	uint8_t key_index;
	uint8_t eap_options;
	uint16_t ca_certificate_length;
	uint16_t client_certificate_length;
	uint16_t private_key_length;
} __attribute__((__packed__)) GetWifiEncryptionReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetWifiStatus;

typedef struct {
	MessageHeader header;
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
	MessageHeader header;
} __attribute__((__packed__)) RefreshWifiStatus;

typedef struct {
	MessageHeader header;
	uint16_t index;
	char data[32];
	uint8_t data_length;
} __attribute__((__packed__)) SetWifiCertificate;

typedef struct {
	MessageHeader header;
	uint16_t index;
} __attribute__((__packed__)) GetWifiCertificate;

typedef struct {
	MessageHeader header;
	char data[32];
	uint8_t data_length;
} __attribute__((__packed__)) GetWifiCertificateReturn;

typedef struct {
	MessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) SetWifiPowerMode;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetWifiPowerMode;

typedef struct {
	MessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) GetWifiPowerModeReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetWifiBufferInfo;

typedef struct {
	MessageHeader header;
	uint32_t overflow;
	uint16_t low_watermark;
	uint16_t used;
} __attribute__((__packed__)) GetWifiBufferInfoReturn;

typedef struct {
	MessageHeader header;
	uint8_t domain;
} __attribute__((__packed__)) SetWifiRegulatoryDomain;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetWifiRegulatoryDomain;

typedef struct {
	MessageHeader header;
	uint8_t domain;
} __attribute__((__packed__)) GetWifiRegulatoryDomainReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetUSBVoltage;

typedef struct {
	MessageHeader header;
	uint16_t voltage;
} __attribute__((__packed__)) GetUSBVoltageReturn;

typedef struct {
	MessageHeader header;
	char key[64];
} __attribute__((__packed__)) SetLongWifiKey;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetLongWifiKey;

typedef struct {
	MessageHeader header;
	char key[64];
} __attribute__((__packed__)) GetLongWifiKeyReturn;

typedef struct {
	MessageHeader header;
	char hostname[16];
} __attribute__((__packed__)) SetWifiHostname;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetWifiHostname;

typedef struct {
	MessageHeader header;
	char hostname[16];
} __attribute__((__packed__)) GetWifiHostnameReturn;

typedef struct {
	MessageHeader header;
	uint32_t debounce;
} __attribute__((__packed__)) SetDebouncePeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetDebouncePeriod;

typedef struct {
	MessageHeader header;
	uint32_t debounce;
} __attribute__((__packed__)) GetDebouncePeriodReturn;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetStackCurrentCallbackPeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetStackCurrentCallbackPeriod;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetStackCurrentCallbackPeriodReturn;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetStackVoltageCallbackPeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetStackVoltageCallbackPeriod;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetStackVoltageCallbackPeriodReturn;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetUSBVoltageCallbackPeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetUSBVoltageCallbackPeriod;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetUSBVoltageCallbackPeriodReturn;

typedef struct {
	MessageHeader header;
	char option;
	uint16_t min;
	uint16_t max;
} __attribute__((__packed__)) SetStackCurrentCallbackThreshold;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetStackCurrentCallbackThreshold;

typedef struct {
	MessageHeader header;
	char option;
	uint16_t min;
	uint16_t max;
} __attribute__((__packed__)) GetStackCurrentCallbackThresholdReturn;

typedef struct {
	MessageHeader header;
	char option;
	uint16_t min;
	uint16_t max;
} __attribute__((__packed__)) SetStackVoltageCallbackThreshold;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetStackVoltageCallbackThreshold;

typedef struct {
	MessageHeader header;
	char option;
	uint16_t min;
	uint16_t max;
} __attribute__((__packed__)) GetStackVoltageCallbackThresholdReturn;

typedef struct {
	MessageHeader header;
	char option;
	uint16_t min;
	uint16_t max;
} __attribute__((__packed__)) SetUSBVoltageCallbackThreshold;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetUSBVoltageCallbackThreshold;

typedef struct {
	MessageHeader header;
	char option;
	uint16_t min;
	uint16_t max;
} __attribute__((__packed__)) GetUSBVoltageCallbackThresholdReturn;

typedef struct {
	MessageHeader header;
	uint16_t current;
} __attribute__((__packed__)) StackCurrent;

typedef struct {
	MessageHeader header;
	uint16_t voltage;
} __attribute__((__packed__)) StackVoltage;

typedef struct {
	MessageHeader header;
	uint16_t voltage;
} __attribute__((__packed__)) USBVoltage;

typedef struct {
	MessageHeader header;
	uint16_t current;
} __attribute__((__packed__)) StackCurrentReached;

typedef struct {
	MessageHeader header;
	uint16_t voltage;
} __attribute__((__packed__)) StackVoltageReached;

typedef struct {
	MessageHeader header;
	uint16_t voltage;
} __attribute__((__packed__)) USBVoltageReached;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) IsEthernetPresent;

typedef struct {
	MessageHeader header;
	bool present;
} __attribute__((__packed__)) IsEthernetPresentReturn;

typedef struct {
	MessageHeader header;
	uint8_t connection;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint16_t port;
} __attribute__((__packed__)) SetEthernetConfiguration;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetEthernetConfiguration;

typedef struct {
	MessageHeader header;
	uint8_t connection;
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint16_t port;
} __attribute__((__packed__)) GetEthernetConfigurationReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetEthernetStatus;

typedef struct {
	MessageHeader header;
	uint8_t mac_address[6];
	uint8_t ip[4];
	uint8_t subnet_mask[4];
	uint8_t gateway[4];
	uint32_t rx_count;
	uint32_t tx_count;
	char hostname[32];
} __attribute__((__packed__)) GetEthernetStatusReturn;

typedef struct {
	MessageHeader header;
	char hostname[32];
} __attribute__((__packed__)) SetEthernetHostname;

typedef struct {
	MessageHeader header;
	uint8_t mac_address[6];
} __attribute__((__packed__)) SetEthernetMAC;

typedef struct {
	MessageHeader header;
	uint8_t sockets;
	uint16_t port;
} __attribute__((__packed__)) SetEthernetWebsocketConfiguration;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetEthernetWebsocketConfiguration;

typedef struct {
	MessageHeader header;
	uint8_t sockets;
	uint16_t port;
} __attribute__((__packed__)) GetEthernetWebsocketConfigurationReturn;

typedef struct {
	MessageHeader header;
	char secret[AUTHENTICATION_SECRET_LENGTH];
} __attribute__((__packed__)) SetEthernetAuthenticationSecret;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetEthernetAuthenticationSecret;

typedef struct {
	MessageHeader header;
	char secret[AUTHENTICATION_SECRET_LENGTH];
} __attribute__((__packed__)) GetEthernetAuthenticationSecretReturn;

void get_stack_voltage(const ComType com, const GetStackVoltage *data);
void get_stack_current(const ComType com, const GetStackCurrent *data);
void set_extension_type(const ComType com, const SetExtensionType *data);
void get_extension_type(const ComType com, const GetExtensionType *data);
void is_chibi_present(const ComType com, const IsChibiPresent *data);
void set_chibi_address(const ComType com, const SetChibiAddress *data);
void get_chibi_address(const ComType com, const GetChibiAddress *data);
void set_chibi_master_address(const ComType com, const SetChibiMasterAddress *data);
void get_chibi_master_address(const ComType com, const GetChibiMasterAddress *data);
void set_chibi_slave_address(const ComType com, const SetChibiSlaveAddress *data);
void get_chibi_slave_address(const ComType com, const GetChibiSlaveAddress *data);
void get_chibi_signal_strength(const ComType com, const GetChibiSignalStrength *data);
void get_chibi_error_log(const ComType com, const GetChibiErrorLog *data);
void set_chibi_frequency(const ComType com, const SetChibiFrequency *data);
void get_chibi_frequency(const ComType com, const GetChibiFrequency *data);
void set_chibi_channel(const ComType com, const SetChibiChannel *data);
void get_chibi_channel(const ComType com, const GetChibiChannel *data);
void is_rs485_present(const ComType com, const IsRS485Present *data);
void set_rs485_address(const ComType com, const SetRS485Address *data);
void get_rs485_address(const ComType com, const GetRS485Address *data);
void set_rs485_slave_address(const ComType com, const SetRS485SlaveAddress *data);
void get_rs485_slave_address(const ComType com, const GetRS485SlaveAddress *data);
void get_rs485_error_log(const ComType com, const GetRS485ErrorLog *data);
void set_rs485_configuration(const ComType com, const SetRS485Configuration *data);
void get_rs485_configuration(const ComType com, const GetRS485Configuration *data);
void is_wifi_present(const ComType com, const IsWifiPresent *data);
void set_wifi_configuration(const ComType com, const SetWifiConfiguration *data);
void get_wifi_configuration(const ComType com, const GetWifiConfiguration *data);
void set_wifi_encryption(const ComType com, const SetWifiEncryption *data);
void get_wifi_encryption(const ComType com, const GetWifiEncryption *data);
void get_wifi_status(const ComType com, const GetWifiStatus *data);
void refresh_wifi_status(const ComType com, const RefreshWifiStatus *data);
void set_wifi_certificate(const ComType com, const SetWifiCertificate *data);
void get_wifi_certificate(const ComType com, const GetWifiCertificate *data);
void set_wifi_power_mode(const ComType com, const SetWifiPowerMode *data);
void get_wifi_power_mode(const ComType com, const GetWifiPowerMode *data);
void get_wifi_buffer_info(const ComType com, const GetWifiBufferInfo *data);
void set_wifi_regulatory_domain(const ComType com, const SetWifiRegulatoryDomain *data);
void get_wifi_regulatory_domain(const ComType com, const GetWifiRegulatoryDomain *data);
void get_usb_voltage(const ComType com, const GetUSBVoltage *data);
void set_long_wifi_key(const ComType com, const SetLongWifiKey *data);
void get_long_wifi_key(const ComType com, const GetLongWifiKey *data);
void set_wifi_hostname(const ComType com, const SetWifiHostname *data);
void get_wifi_hostname(const ComType com, const GetWifiHostname *data);
void set_stack_current_callback_period(const ComType com, const SetStackCurrentCallbackPeriod *data);
void get_stack_current_callback_period(const ComType com, const GetStackCurrentCallbackPeriod *data);
void set_stack_voltage_callback_period(const ComType com, const SetStackVoltageCallbackPeriod *data);
void get_stack_voltage_callback_period(const ComType com, const GetStackVoltageCallbackPeriod *data);
void set_usb_voltage_callback_period(const ComType com, const SetUSBVoltageCallbackPeriod *data);
void get_usb_voltage_callback_period(const ComType com, const GetUSBVoltageCallbackPeriod *data);
void set_stack_current_callback_threshold(const ComType com, const SetStackCurrentCallbackThreshold *data);
void get_stack_current_callback_threshold(const ComType com, const GetStackCurrentCallbackThreshold *data);
void set_stack_voltage_callback_threshold(const ComType com, const SetStackVoltageCallbackThreshold *data);
void get_stack_voltage_callback_threshold(const ComType com, const GetStackVoltageCallbackThreshold *data);
void set_usb_voltage_callback_threshold(const ComType com, const SetUSBVoltageCallbackThreshold *data);
void get_usb_voltage_callback_threshold(const ComType com, const GetUSBVoltageCallbackThreshold *data);
void set_debounce_period(const ComType com, const SetDebouncePeriod *data);
void get_debounce_period(const ComType com, const GetDebouncePeriod *data);
void is_ethernet_present(const ComType com, const IsEthernetPresent *data);
void set_ethernet_configuration(const ComType com, const SetEthernetConfiguration *data);
void get_ethernet_configuration(const ComType com, const GetEthernetConfiguration *data);
void get_ethernet_status(const ComType com, const GetEthernetStatus *data);
void set_ethernet_hostname(const ComType com, const SetEthernetHostname *data);
void set_ethernet_mac(const ComType com, const SetEthernetMAC *data);
void set_ethernet_websocket_configuration(const ComType com, const SetEthernetWebsocketConfiguration *data);
void get_ethernet_websocket_configuration(const ComType com, const GetEthernetWebsocketConfiguration *data);
void set_ethernet_authentication_secret(const ComType com, const SetEthernetAuthenticationSecret *data);
void get_ethernet_authentication_secret(const ComType com, const GetEthernetAuthenticationSecret *data);

#endif
