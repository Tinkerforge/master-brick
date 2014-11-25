/* master-brick
 * Copyright (C) 2010-2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.h: Master-Brick specific configuration
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

#ifndef CONFIG_H
#define CONFIG_H

#include "bricklib/drivers/board/sam3s/SAM3S.h"
#include "master.h"
#include "communication.h"
#include "extensions/chibi/chibi.h"
#include "extensions/rs485/rs485.h"
#include "extensions/wifi/wifi.h"
#include "extensions/ethernet/ethernet.h"
#include "extensions/extension_config.h"

#define BRICK_FIRMWARE_VERSION_MAJOR 2
#define BRICK_FIRMWARE_VERSION_MINOR 3
#define BRICK_FIRMWARE_VERSION_REVISION 0

#define BRICK_HARDWARE_VERSION_MAJOR_20 2
#define BRICK_HARDWARE_VERSION_MINOR_20 0
#define BRICK_HARDWARE_VERSION_REVISION_20 0

#define BRICK_HARDWARE_VERSION_MAJOR_10 1
#define BRICK_HARDWARE_VERSION_MINOR_10 0
#define BRICK_HARDWARE_VERSION_REVISION_10 0

#define BRICK_DEVICE_IDENTIFIER 13
#define BRICK_CAN_BE_MASTER

// ************** DEBUG SETTINGS **************
#define DEBUG_SPI_STACK 1
//#define DEBUG_I2C_EEPROM 1
#define DEBUG_STARTUP 1
#define DEBUG_BRICKLET 1
#define DEBUG_CHIBI 1
#define DEBUG_MASTER 1
//#define PROFILING
//#define PROFILING_TIME 100 // After how many seconds profiling is printed

#define DISABLE_JTAG_ON_STARTUP
//#define LOGGING_SERIAL
//#define LOGGING_LEVEL LOGGING_DEBUG
#define LOGGING_LEVEL LOGGING_NONE

// ************** BRICK SETTINGS **************

// Frequencies
#define BOARD_MCK      64000000 // Frequency of brick
#define BOARD_MAINOSC  16000000 // Frequency of oscillator
#define BOARD_ADC_FREQ  6000000 // Frequency of ADC
#define BOARD_OSC_EXTERNAL      // Use external oscillator


// UART for console output (printf)
//#define CONSOLE_USART_USE_UART1

#define PIN_CONSOLE_RXD  {1 << 21, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_CONSOLE_TXD  {1 << 22, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

#define CONSOLE_BAUDRATE 115200
#define CONSOLE_USART    USART1
#define CONSOLE_ID       ID_USART1
#define CONSOLE_PINS     {PIN_CONSOLE_RXD, PIN_CONSOLE_TXD}

#define PINS_UART        {PIN_CONSOLE_RXD, PIN_CONSOLE_TXD}

// TWI
// TWI version
#define TWI_V3XX

// TWI stack definitions (for reading of eeproms from Bricks in stack)
#define TWI_STACK           TWI1
#define PIN_TWI_TWD_STACK   {1 << 4, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_TWI_TWCK_STACK  {1 << 5, PIOB, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT}
#define PINS_TWI_STACK      PIN_TWI_TWD_STACK, PIN_TWI_TWCK_STACK

#define PIN_SCL_PULLUP_MASTER   {1 << 17, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_SDA_PULLUP_MASTER   {1 << 0, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_DEFAULT}
#define PINS_TWI_PULLUP_MASTER  PIN_SCL_PULLUP_MASTER, PIN_SDA_PULLUP_MASTER

#define PIN_SCL_PULLUP_SLAVE    {1 << 17, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define PIN_SDA_PULLUP_SLAVE    {1 << 0, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PINS_TWI_PULLUP_SLAVE   PIN_SCL_PULLUP_SLAVE, PIN_SDA_PULLUP_SLAVE

// TWI bricklet definitions (for bricklets, spi select and brick specific
//                           functions)
#define TWI_BRICKLET           TWI0
#define PIN_TWI_TWD_BRICKLET   {1 << 3, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_TWI_TWCK_BRICKLET  {1 << 4, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PINS_TWI_BRICKLET      PIN_TWI_TWD_BRICKLET, PIN_TWI_TWCK_BRICKLET

// USB
// USB VBUS monitoring pin for USB plug and play
#define PIN_USB_DETECT      {1 << 17, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_MASTER20_DETECT {1 << 28, PIOC, ID_PIOC, PIO_INPUT, PIO_PULLUP}
// High if master in stack
#define PIN_3V3_ENABLE      {1 << 16, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}


#define ADC_CAL_SUM 10

#define PIN_ADC_CAL_LOW     {1 << 3,  PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#define ADC_CAL_LOW_CHANNEL 7
#define ADC_CAL_LOW_MULTIPLIER 1
#define ADC_CAL_LOW_DIVISOR 11
#define ADC_CAL_LOW_REFERENCE 3300

#define PIN_ADC_CAL_HIGH    {1 << 15, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define ADC_CAL_HIGH_CHANNEL 11
#define ADC_CAL_HIGH_MULTIPLIER 10
#define ADC_CAL_HIGH_DIVISOR 11
#define ADC_CAL_HIGH_REFERENCE 3300


// USB product descriptor (name of brick)
#define PRODUCT_DESCRIPTOR { \
	USBStringDescriptor_LENGTH(12), \
    USBGenericDescriptor_STRING, \
    USBStringDescriptor_UNICODE('M'), \
    USBStringDescriptor_UNICODE('a'), \
    USBStringDescriptor_UNICODE('s'), \
    USBStringDescriptor_UNICODE('t'), \
    USBStringDescriptor_UNICODE('e'), \
    USBStringDescriptor_UNICODE('r'), \
    USBStringDescriptor_UNICODE(' '), \
    USBStringDescriptor_UNICODE('B'), \
    USBStringDescriptor_UNICODE('r'), \
    USBStringDescriptor_UNICODE('i'), \
    USBStringDescriptor_UNICODE('c'), \
    USBStringDescriptor_UNICODE('k') \
}


// SPI
#define PIN_SPI_MISO        {1 << 12, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI_MOSI        {1 << 13, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_SPI_SPCK        {1 << 14, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

#define PINS_SPI            PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SPCK

#define PIN_SPI_SELECT_SLAVE  {1 << 11, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

#define PIN_SPI_SELECT_MASTER_0 {1 << 20, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_SPI_SELECT_MASTER_1 {1 << 21, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_SPI_SELECT_MASTER_2 {1 << 22, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_SPI_SELECT_MASTER_3 {1 << 23, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_SPI_SELECT_MASTER_4 {1 << 0, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_SPI_SELECT_MASTER_5 {1 << 1, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_SPI_SELECT_MASTER_6 {1 << 2, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#ifndef CONSOLE_USART_USE_UART1
#define PIN_SPI_SELECT_MASTER_7_20 {1 << 31, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_SPI_SELECT_MASTER_7_10 {1 << 3, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_DEFAULT}
#endif

#define PINS_SPI_SELECT_MASTER   PIN_SPI_SELECT_MASTER_0, \
                                 PIN_SPI_SELECT_MASTER_1, \
                                 PIN_SPI_SELECT_MASTER_2, \
                                 PIN_SPI_SELECT_MASTER_3, \
                                 PIN_SPI_SELECT_MASTER_4, \
                                 PIN_SPI_SELECT_MASTER_5, \
                                 PIN_SPI_SELECT_MASTER_6

// LED
#define PIN_LED_STD_BLUE    {1 << 18, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_STD_RED     {1 << 19, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PINS_STD_LED        PIN_LED_STD_BLUE, PIN_LED_STD_RED

#define PIN_LED_EXT_BLUE_0  {1 << 24, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_EXT_BLUE_1  {1 << 25, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_EXT_BLUE_2  {1 << 26, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_EXT_BLUE_3  {1 << 27, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PINS_EXT_LED        PIN_LED_EXT_BLUE_0, PIN_LED_EXT_BLUE_1, \
                            PIN_LED_EXT_BLUE_2, PIN_LED_EXT_BLUE_3

#define PINS_LED            PINS_STD_LED, PINS_EXT_LED

#define LED_STD_BLUE        0
#define LED_STD_RED         1
#define LED_EXT_BLUE_0      2
#define LED_EXT_BLUE_1      3
#define LED_EXT_BLUE_2      4
#define LED_EXT_BLUE_3      5

// Brick Detect
// Set low by master
// TODO: Change name
#define PIN_DETECT        {1 << 6, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

// If low, there is a master below (-> configure as stack participant)
#define PIN_MASTER_DETECT {1 << 18, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP}


// ************** INTERRUPT PRIORITIES ***********
#define PRIORITY_EEPROM_MASTER_TWI0 0
#define PRIORITY_EEPROM_MASTER_TWI1 0
#define PRIORITY_EEPROM_SLAVE_TWI1 0
#define PRIORITY_STACK_SLAVE_SPI 0
#define PRIORITY_STACK_MASTER_SPI 0
#define PRIORITY_PROFILING_TC0 0


// ************** POWER MEASUREMENT **************
#define PIN_STACK_VOLTAGE {1 << 19, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_STACK_CURRENT {1 << 20, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}

#define VOLTAGE_MAX_VALUE 4095

#define USB_VOLTAGE_CHANNEL 0
#define USB_VOLTAGE_MULTIPLIER (47+68)
#define USB_VOLTAGE_DIVISOR 68
#define USB_VOLTAGE_REFERENCE 3300

#define STACK_VOLTAGE_CHANNEL 2
#define STACK_VOLTAGE_MULTIPLIER 11
#define STACK_VOLTAGE_REFERENCE 3300

#define STACK_CURRENT_CHANNEL 3
#define STACK_CURRENT_MULTIPLIER 2
#define STACK_CURRENT_REFERENCE 3300

#define VOLTAGE_EPSILON 1000

// ************** BRICKLET SETTINGS **************

// Number of bricklet ports
#define BRICKLET_NUM 4

// BRICKLET A
#define BRICKLET_A_ADDRESS 84
#define BRICKLET_A_ADC_CHANNEL 12

#define BRICKLET_A_PIN_1_AD   {1 << 12, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_A_PIN_2_DA   {1 << 25, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_A_PIN_3_PWM  {1 << 0, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_A_PIN_4_IO   {1 << 4, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_A_PIN_SELECT {1 << 8, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}


// BRICKLET B
#define BRICKLET_B_ADDRESS 84
#define BRICKLET_B_ADC_CHANNEL 10

#define BRICKLET_B_PIN_1_AD   {1 << 13, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_B_PIN_2_DA   {1 << 26, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_B_PIN_3_PWM  {1 << 1, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_B_PIN_4_IO   {1 << 5, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_B_PIN_SELECT {1 << 9, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}


// BRICKLET C
#define BRICKLET_C_ADDRESS 84
#define BRICKLET_C_ADC_CHANNEL 13

#define BRICKLET_C_PIN_1_AD   {1 << 29, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_C_PIN_2_DA   {1 << 13, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_C_PIN_3_PWM  {1 << 2, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_C_PIN_4_IO   {1 << 6, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_C_PIN_SELECT {1 << 10, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}


// BRICKLET D
#define BRICKLET_D_ADDRESS 84
#define BRICKLET_D_ADC_CHANNEL 14

#define BRICKLET_D_PIN_1_AD   {1 << 30, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_D_PIN_2_DA   {1 << 14, PIOB, ID_PIOB, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_D_PIN_3_PWM  {1 << 3, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_D_PIN_4_IO   {1 << 7, PIOC, ID_PIOC, PIO_INPUT, PIO_DEFAULT}
#define BRICKLET_D_PIN_SELECT {1 << 11, PIOC, ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT}



// ************** EXTENSION SETTINGS **************
#define PIN_EXT_0_SELECT  {1 << 28, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_EXT_0_GP_0    {1 << 9, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EXT_0_GP_1    {1 << 10, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EXT_0_GP_2    {1 << 27, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}

#define PIN_EXT_1_SELECT  {1 << 29, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_EXT_1_GP_0    {1 << 1, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EXT_1_GP_1    {1 << 30, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EXT_1_GP_2    {1 << 31, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}

#define PINS_EXT          PIN_EXT_0_GP_0, PIN_EXT_0_GP_1, PIN_EXT_0_GP_2, \
                          PIN_EXT_1_GP_0, PIN_EXT_1_GP_1, PIN_EXT_1_GP_2

#define PIN_EXT_RXD       {1 << 5, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EXT_TXD       {1 << 6, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}
#define PIN_EXT_SCK       {1 << 2, PIOA, ID_PIOA, PIO_INPUT, PIO_DEFAULT}

// ************** MASTER DEBUGGING **************
#define PIN_STACK_SYNC    {1 << 7, PIOB, ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT}

#if(DEBUG_MASTER)
#define logmasterd(str, ...) do{logd("master: " str, ##__VA_ARGS__);}while(0)
#define logmasteri(str, ...) do{logi("master: " str, ##__VA_ARGS__);}while(0)
#define logmasterw(str, ...) do{logw("master: " str, ##__VA_ARGS__);}while(0)
#define logmastere(str, ...) do{loge("master: " str, ##__VA_ARGS__);}while(0)
#define logmasterf(str, ...) do{logf("master: " str, ##__VA_ARGS__);}while(0)
#else
#define logmasterd(str, ...) {}
#define logmasteri(str, ...) {}
#define logmasterw(str, ...) {}
#define logmastere(str, ...) {}
#define logmasterf(str, ...) {}
#endif


#endif
