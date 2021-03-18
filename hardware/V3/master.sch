EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 4
Title ""
Date "2020-08-25"
Rev "3.1"
Comp "Tinkerforge GmbH"
Comment1 "Licensed under CERN OHL v.1.1"
Comment2 "Copyright (©) 2020, T.Schneidermann <tim@tinkerforge.com>"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L tinkerforge:R R10
U 1 1 506305EB
P 6000 3750
F 0 "R10" V 5900 3750 50  0000 C CNN
F 1 "DNP" V 6000 3750 50  0000 C CNN
F 2 "R0603" H 6000 3750 60  0001 C CNN
F 3 "" H 6000 3750 60  0001 C CNN
	1    6000 3750
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:GND #PWR01
U 1 1 506305EA
P 6000 4050
F 0 "#PWR01" H 6000 4050 30  0001 C CNN
F 1 "GND" H 6000 3980 30  0001 C CNN
F 2 "" H 6000 4050 60  0001 C CNN
F 3 "" H 6000 4050 60  0001 C CNN
	1    6000 4050
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R9
U 1 1 506305E9
P 6000 3200
F 0 "R9" V 5900 3200 50  0000 C CNN
F 1 "DNP" V 6000 3200 50  0000 C CNN
F 2 "R0603" H 6000 3200 60  0001 C CNN
F 3 "" H 6000 3200 60  0001 C CNN
	1    6000 3200
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:3V3 #PWR02
U 1 1 506305E8
P 6000 2900
F 0 "#PWR02" H 6000 3000 40  0001 C CNN
F 1 "3V3" H 6000 3025 40  0000 C CNN
F 2 "" H 6000 2900 60  0001 C CNN
F 3 "" H 6000 2900 60  0001 C CNN
	1    6000 2900
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:C C10
U 1 1 506305E7
P 6250 3750
F 0 "C10" H 6300 3850 50  0000 L CNN
F 1 "DNP" H 6300 3650 50  0000 L CNN
F 2 "C0603" H 6250 3750 60  0001 C CNN
F 3 "" H 6250 3750 60  0001 C CNN
	1    6250 3750
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR03
U 1 1 506305E6
P 6250 4050
F 0 "#PWR03" H 6250 4050 30  0001 C CNN
F 1 "GND" H 6250 3980 30  0001 C CNN
F 2 "" H 6250 4050 60  0001 C CNN
F 3 "" H 6250 4050 60  0001 C CNN
	1    6250 4050
	1    0    0    -1  
$EndComp
Text Notes 6200 3250 0    60   ~ 0
1/11 * 3.3V
Text Notes 9850 1550 0    60   ~ 0
10/11 * 3.3V
$Comp
L tinkerforge:GND #PWR04
U 1 1 50630570
P 9900 2350
F 0 "#PWR04" H 9900 2350 30  0001 C CNN
F 1 "GND" H 9900 2280 30  0001 C CNN
F 2 "" H 9900 2350 60  0001 C CNN
F 3 "" H 9900 2350 60  0001 C CNN
	1    9900 2350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C11
U 1 1 50630567
P 9900 2050
F 0 "C11" H 9950 2150 50  0000 L CNN
F 1 "DNP" H 9950 1950 50  0000 L CNN
F 2 "C0603" H 9900 2050 60  0001 C CNN
F 3 "" H 9900 2050 60  0001 C CNN
	1    9900 2050
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:3V3 #PWR05
U 1 1 506304FD
P 9650 1200
F 0 "#PWR05" H 9650 1300 40  0001 C CNN
F 1 "3V3" H 9650 1325 40  0000 C CNN
F 2 "" H 9650 1200 60  0001 C CNN
F 3 "" H 9650 1200 60  0001 C CNN
	1    9650 1200
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:R R12
U 1 1 506304F1
P 9650 1500
F 0 "R12" V 9550 1500 50  0000 C CNN
F 1 "DNP" V 9650 1500 50  0000 C CNN
F 2 "R0603" H 9650 1500 60  0001 C CNN
F 3 "" H 9650 1500 60  0001 C CNN
	1    9650 1500
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:GND #PWR06
U 1 1 506304E9
P 9650 2350
F 0 "#PWR06" H 9650 2350 30  0001 C CNN
F 1 "GND" H 9650 2280 30  0001 C CNN
F 2 "" H 9650 2350 60  0001 C CNN
F 3 "" H 9650 2350 60  0001 C CNN
	1    9650 2350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R13
U 1 1 506304E4
P 9650 2050
F 0 "R13" V 9550 2050 50  0000 C CNN
F 1 "DNP" V 9650 2050 50  0000 C CNN
F 2 "R0603" H 9650 2050 60  0001 C CNN
F 3 "" H 9650 2050 60  0001 C CNN
	1    9650 2050
	-1   0    0    1   
$EndComp
Text Notes 8900 3550 0    60   ~ 0
Detect Master 2.0
$Comp
L tinkerforge:GND #PWR07
U 1 1 506304CF
P 8850 3550
F 0 "#PWR07" H 8850 3550 30  0001 C CNN
F 1 "GND" H 8850 3480 30  0001 C CNN
F 2 "" H 8850 3550 60  0001 C CNN
F 3 "" H 8850 3550 60  0001 C CNN
	1    8850 3550
	1    0    0    -1  
$EndComp
Text GLabel 3000 2350 2    60   Output ~ 0
3V3-EN
Text Notes 550  7700 0    40   ~ 0
Copyright Tinkerforge GmbH 2020.\nThis documentation describes Open Hardware and is licensed under the\nCERN OHL v. 1.1.\nYou may redistribute and modify this documentation under the terms of the\nCERN OHL v.1.1. (http://ohwr.org/cernohl). This documentation is distributed\nWITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF\nMERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A\nPARTICULAR PURPOSE. Please see the CERN OHL v.1.1 for applicable\nconditions\n
NoConn ~ 9450 2700
NoConn ~ 9450 2800
NoConn ~ 9850 2700
NoConn ~ 9850 2800
$Comp
L tinkerforge:R_PACK4 RP3
U 1 1 4D9D895B
P 9650 2950
F 0 "RP3" H 9650 3400 40  0000 C CNN
F 1 "1k" H 9650 2900 40  0000 C CNN
F 2 "0603X4" H 9650 2950 60  0001 C CNN
F 3 "" H 9650 2950 60  0001 C CNN
	1    9650 2950
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:R_PACK4 RP4
U 1 1 4D9D51C1
P 10000 5150
F 0 "RP4" H 10000 5600 40  0000 C CNN
F 1 "1k" H 10000 5100 40  0000 C CNN
F 2 "0603X4" H 10000 5150 60  0001 C CNN
F 3 "" H 10000 5150 60  0001 C CNN
	1    10000 5150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:AT91SAM3SXC U1
U 1 1 4D22CFEA
P 800 650
F 0 "U1" H 850 700 60  0000 C CNN
F 1 "SAM4S4CA" H 1400 700 60  0000 C CNN
F 2 "LQFP100" H 800 650 60  0001 C CNN
F 3 "" H 800 650 60  0001 C CNN
	1    800  650 
	1    0    0    -1  
$EndComp
Text GLabel 8200 2500 2    60   Output ~ 0
SCL-PULLUP
Text GLabel 3000 750  2    60   Output ~ 0
SDA-PULLUP
NoConn ~ 1950 3050
NoConn ~ 1950 2250
Text GLabel 8200 3500 2    60   Output ~ 0
LED4
Text GLabel 8200 3400 2    60   Output ~ 0
LED3
Text GLabel 8200 3300 2    60   Output ~ 0
LED2
Text GLabel 8200 3200 2    60   Output ~ 0
LED1
Text GLabel 9650 5100 0    60   Input ~ 0
LED4
Text GLabel 9650 5000 0    60   Input ~ 0
LED3
Text GLabel 9650 4900 0    60   Input ~ 0
LED2
Text GLabel 9650 4800 0    60   Input ~ 0
LED1
$Comp
L tinkerforge:DRILL J3
U 1 1 4D231088
P 10700 6350
F 0 "J3" H 10750 6400 60  0001 C CNN
F 1 "DRILL" H 10700 6350 60  0000 C CNN
F 2 "DRILL_NP" H 10700 6350 60  0001 C CNN
F 3 "" H 10700 6350 60  0001 C CNN
	1    10700 6350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL J2
U 1 1 4D231086
P 10700 6100
F 0 "J2" H 10750 6150 60  0001 C CNN
F 1 "DRILL" H 10700 6100 60  0000 C CNN
F 2 "DRILL_NP" H 10700 6100 60  0001 C CNN
F 3 "" H 10700 6100 60  0001 C CNN
	1    10700 6100
	1    0    0    -1  
$EndComp
Text GLabel 3000 1450 2    60   Output ~ 0
Port-D-CS
Text GLabel 8200 3700 2    60   Input ~ 0
Port-C-CS
Text GLabel 3000 1150 2    60   Output ~ 0
Port-D-CLK
Text GLabel 2900 5750 2    60   Output ~ 0
Port-C-CLK
Text GLabel 8200 1500 2    60   Output ~ 0
Port-D-MISO
Text GLabel 8200 1400 2    60   Output ~ 0
Port-C-MISO
Text GLabel 8200 1100 2    60   Output ~ 0
Port-D-MOSI
Text GLabel 8200 1000 2    60   Output ~ 0
Port-C-MOSI
$Comp
L tinkerforge:3V3 #PWR08
U 1 1 4D22FF11
P 10700 4550
F 0 "#PWR08" H 10700 4650 40  0001 C CNN
F 1 "3V3" H 10700 4675 40  0000 C CNN
F 2 "" H 10700 4550 60  0001 C CNN
F 3 "" H 10700 4550 60  0001 C CNN
	1    10700 4550
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:LED D6
U 1 1 4D22FEFC
P 10500 5250
F 0 "D6" H 10650 5350 50  0000 C CNN
F 1 "blue" H 10450 5350 50  0000 C CNN
F 2 "D0603" H 10500 5250 60  0001 C CNN
F 3 "" H 10500 5250 60  0001 C CNN
	1    10500 5250
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:LED D5
U 1 1 4D22FEF4
P 10500 5050
F 0 "D5" H 10650 5150 50  0000 C CNN
F 1 "blue" H 10450 5150 50  0000 C CNN
F 2 "D0603" H 10500 5050 60  0001 C CNN
F 3 "" H 10500 5050 60  0001 C CNN
	1    10500 5050
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:LED D4
U 1 1 4D22FEDF
P 10500 4850
F 0 "D4" H 10650 4950 50  0000 C CNN
F 1 "blue" H 10450 4950 50  0000 C CNN
F 2 "D0603" H 10500 4850 60  0001 C CNN
F 3 "" H 10500 4850 60  0001 C CNN
	1    10500 4850
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:LED D3
U 1 1 4D22FED4
P 10500 4650
F 0 "D3" H 10650 4750 50  0000 C CNN
F 1 "blue" H 10450 4750 50  0000 C CNN
F 2 "D0603" H 10500 4650 60  0001 C CNN
F 3 "" H 10500 4650 60  0001 C CNN
	1    10500 4650
	-1   0    0    -1  
$EndComp
$Sheet
S 9350 5500 900  900 
U 4D22D35E
F0 "Bricklets" 60
F1 "bricklets.sch" 60
$EndSheet
$Sheet
S 8200 5500 900  900 
U 4D22D34D
F0 "Stack" 60
F1 "stack.sch" 60
$EndSheet
$Sheet
S 7050 5500 900  900 
U 4D22D320
F0 "Powersupply" 60
F1 "powersupply.sch" 60
$EndSheet
$Comp
L tinkerforge:AT91SAM3SXC U1
U 2 1 4D22D003
P 800 4250
F 0 "U1" H 850 4300 60  0000 C CNN
F 1 "SAM4S4CA" H 1400 4300 60  0000 C CNN
F 2 "LQFP100" H 800 4250 60  0001 C CNN
F 3 "" H 800 4250 60  0001 C CNN
	2    800  4250
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:AT91SAM3SXC U1
U 3 1 4D22CFF7
P 6850 650
F 0 "U1" H 6900 700 60  0000 C CNN
F 1 "SAM4S4CA" H 7450 700 60  0000 C CNN
F 2 "LQFP100" H 6850 650 60  0001 C CNN
F 3 "" H 6850 650 60  0001 C CNN
	3    6850 650 
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:CRYSTAL_3225 X1
U 1 1 4D0F3B46
P 10550 3550
F 0 "X1" H 10450 3800 60  0000 C CNN
F 1 "16Mhz" H 10600 3300 60  0000 C CNN
F 2 "CRYSTAL_3225" H 10550 3550 60  0001 C CNN
F 3 "" H 10550 3550 60  0001 C CNN
	1    10550 3550
	1    0    0    -1  
$EndComp
Text GLabel 8200 1600 2    60   Input ~ 0
STACK-EX-0-GP02
Text GLabel 3000 3750 2    60   Output ~ 0
STACK-EX-1-GP00
$Comp
L tinkerforge:GND #PWR09
U 1 1 4D0EF6F0
P 10400 4350
F 0 "#PWR09" H 10400 4350 30  0001 C CNN
F 1 "GND" H 10400 4280 30  0001 C CNN
F 2 "" H 10400 4350 60  0001 C CNN
F 3 "" H 10400 4350 60  0001 C CNN
	1    10400 4350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR010
U 1 1 4D0EF6E7
P 10850 4350
F 0 "#PWR010" H 10850 4350 30  0001 C CNN
F 1 "GND" H 10850 4280 30  0001 C CNN
F 2 "" H 10850 4350 60  0001 C CNN
F 3 "" H 10850 4350 60  0001 C CNN
	1    10850 4350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR011
U 1 1 4D0EF6E5
P 10250 4350
F 0 "#PWR011" H 10250 4350 30  0001 C CNN
F 1 "GND" H 10250 4280 30  0001 C CNN
F 2 "" H 10250 4350 60  0001 C CNN
F 3 "" H 10250 4350 60  0001 C CNN
	1    10250 4350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C13
U 1 1 4D0EF5DB
P 10850 4050
F 0 "C13" H 10900 4150 50  0000 L CNN
F 1 "DNP" H 10900 3950 50  0000 L CNN
F 2 "C0603" H 10850 4050 60  0001 C CNN
F 3 "" H 10850 4050 60  0001 C CNN
	1    10850 4050
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C12
U 1 1 4D0EF5CC
P 10250 4050
F 0 "C12" H 10300 4150 50  0000 L CNN
F 1 "DNP" H 10050 3950 50  0000 L CNN
F 2 "C0603" H 10250 4050 60  0001 C CNN
F 3 "" H 10250 4050 60  0001 C CNN
	1    10250 4050
	1    0    0    -1  
$EndComp
Text GLabel 3000 2450 2    60   Input ~ 0
USB-DETECT
Text GLabel 8200 3900 2    60   Output ~ 0
STACK-SELECT-07
Text GLabel 2900 4550 2    60   Output ~ 0
STACK-SELECT-06
Text GLabel 2900 4450 2    60   Output ~ 0
STACK-SELECT-05
Text GLabel 2900 4350 2    60   Output ~ 0
STACK-SELECT-04
Text GLabel 3000 3450 2    60   Output ~ 0
STACK-EX-1-GP02
Text GLabel 3000 1050 2    60   Output ~ 0
STACK-EX-1-GP01
Text GLabel 3000 3550 2    60   Output ~ 0
STACK-SELECT-EX-01
Text GLabel 3000 3650 2    60   Output ~ 0
STACK-SELECT-EX-00
Text GLabel 10850 3300 1    60   Output ~ 0
XIN
Text GLabel 10250 3300 1    60   Input ~ 0
XOUT
Text GLabel 2900 5350 2    60   Input ~ 0
XIN
Text GLabel 2900 5250 2    60   Output ~ 0
XOUT
Text GLabel 3200 6250 2    60   Input ~ 0
STACK-RESET
Text GLabel 3000 3150 2    60   Output ~ 0
STACK-SER2-RTS
$Comp
L tinkerforge:3V3 #PWR012
U 1 1 4CBEA182
P 10400 2500
F 0 "#PWR012" H 10400 2600 40  0001 C CNN
F 1 "3V3" H 10400 2625 40  0000 C CNN
F 2 "" H 10400 2500 60  0001 C CNN
F 3 "" H 10400 2500 60  0001 C CNN
	1    10400 2500
	-1   0    0    -1  
$EndComp
Text GLabel 8200 1300 2    60   Output ~ 0
Port-B-MISO
Text GLabel 8200 1200 2    60   Output ~ 0
Port-A-MISO
Text GLabel 3000 2550 2    60   Input ~ 0
STACK-MASTER-DETECT
NoConn ~ 1750 4950
NoConn ~ 1750 6050
Text GLabel 3000 3350 2    60   Output ~ 0
Port-B-CLK
Text GLabel 3000 3250 2    60   Output ~ 0
Port-A-CLK
Text GLabel 8300 1850 2    60   Input ~ 0
Port-B-CS
Text GLabel 8300 1750 2    60   Input ~ 0
Port-A-CS
Text GLabel 8200 900  2    60   Output ~ 0
Port-B-MOSI
Text GLabel 8200 800  2    60   Output ~ 0
Port-A-MOSI
Text GLabel 3000 2750 2    60   Input ~ 0
STACK-CURRENT
Text GLabel 3000 2650 2    60   Input ~ 0
STACK-VOLTAGE
Text GLabel 2900 5150 2    60   Input ~ 0
STACK-SYNC/JTAG-TCK
Text GLabel 2900 5050 2    60   Input ~ 0
STACK-HIGH/JTAG-TMS
$Comp
L tinkerforge:GND #PWR013
U 1 1 4C7E70E6
P 5650 5450
F 0 "#PWR013" H 5650 5450 30  0001 C CNN
F 1 "GND" H 5650 5380 30  0001 C CNN
F 2 "" H 5650 5450 60  0001 C CNN
F 3 "" H 5650 5450 60  0001 C CNN
	1    5650 5450
	1    0    0    -1  
$EndComp
Text GLabel 6750 4700 2    60   Output ~ 0
USB-DETECT
$Comp
L tinkerforge:R R6
U 1 1 4C7E70C2
P 5650 5150
F 0 "R6" V 5550 5150 50  0000 C CNN
F 1 "47k" V 5650 5150 50  0000 C CNN
F 2 "R0603" H 5650 5150 60  0001 C CNN
F 3 "" H 5650 5150 60  0001 C CNN
	1    5650 5150
	-1   0    0    1   
$EndComp
Text GLabel 3000 1850 2    60   Input ~ 0
STACK-SPI-SELECT
Text GLabel 3000 2950 2    60   Output ~ 0
STACK-SER2-TXD
Text GLabel 3000 2850 2    60   Output ~ 0
STACK-SER2-RXD
Text GLabel 3000 1750 2    60   Output ~ 0
STACK-EX-0-GP01
Text GLabel 3000 1550 2    60   Output ~ 0
STACK-EX-0-GP00
Text GLabel 3000 950  2    60   Output ~ 0
STACK-SER-SCK
Text GLabel 3000 1350 2    60   Output ~ 0
STACK-SER-TXD
Text GLabel 3000 1250 2    60   Output ~ 0
STACK-SER-RXD
Text GLabel 2900 4850 2    60   Output ~ 0
STACK-I2C-SCL/JTAG-TDO
Text GLabel 2900 4750 2    60   Output ~ 0
STACK-I2C-SDA/JTAG-TDI
Text GLabel 3000 2150 2    60   Output ~ 0
STACK-SPI-SCLK
Text GLabel 3000 1950 2    60   Output ~ 0
STACK-SPI-MISO
Text GLabel 3000 2050 2    60   Output ~ 0
STACK-SPI-MOSI
Text GLabel 4800 4850 1    60   Output ~ 0
VUSB
$Comp
L tinkerforge:3V3 #PWR014
U 1 1 4C45BA2A
P 4050 6950
F 0 "#PWR014" H 4050 7050 40  0001 C CNN
F 1 "3V3" H 4050 7075 40  0000 C CNN
F 2 "" H 4050 6950 60  0001 C CNN
F 3 "" H 4050 6950 60  0001 C CNN
	1    4050 6950
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR015
U 1 1 4C45B9F7
P 3900 6600
F 0 "#PWR015" H 3900 6600 30  0001 C CNN
F 1 "GND" H 3900 6530 30  0001 C CNN
F 2 "" H 3900 6600 60  0001 C CNN
F 3 "" H 3900 6600 60  0001 C CNN
	1    3900 6600
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL J5
U 1 1 4C45A739
P 11000 6350
F 0 "J5" H 11050 6400 60  0001 C CNN
F 1 "DRILL" H 11000 6350 60  0000 C CNN
F 2 "DRILL_NP" H 11000 6350 60  0001 C CNN
F 3 "" H 11000 6350 60  0001 C CNN
	1    11000 6350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL J4
U 1 1 4C45A72D
P 11000 6100
F 0 "J4" H 11050 6150 60  0001 C CNN
F 1 "DRILL" H 11000 6100 60  0000 C CNN
F 2 "DRILL_NP" H 11000 6100 60  0001 C CNN
F 3 "" H 11000 6100 60  0001 C CNN
	1    11000 6100
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R4
U 1 1 4C45A20F
P 3450 5550
F 0 "R4" V 3550 5550 50  0000 C CNN
F 1 "27" V 3450 5550 50  0000 C CNN
F 2 "R0603" H 3450 5550 60  0001 C CNN
F 3 "" H 3450 5550 60  0001 C CNN
	1    3450 5550
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:R R3
U 1 1 4C45A1F3
P 3450 5450
F 0 "R3" V 3350 5450 50  0000 C CNN
F 1 "27" V 3450 5450 50  0000 C CNN
F 2 "R0603" H 3450 5450 60  0001 C CNN
F 3 "" H 3450 5450 60  0001 C CNN
	1    3450 5450
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:INDUCT FB1
U 1 1 4C45A11D
P 4800 5300
F 0 "FB1" H 4950 5400 60  0000 C CNN
F 1 "FB" H 4950 5200 60  0000 C CNN
F 2 "C0603" H 4800 5300 60  0001 C CNN
F 3 "" H 4800 5300 60  0001 C CNN
	1    4800 5300
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:LED D1
U 1 1 4C3ED8E1
P 10200 2600
F 0 "D1" H 10200 2700 50  0000 C CNN
F 1 "blue" H 10200 2500 50  0000 C CNN
F 2 "D0603" H 10200 2600 60  0001 C CNN
F 3 "" H 10200 2600 60  0001 C CNN
	1    10200 2600
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:LED D2
U 1 1 4C3ED8B3
P 10200 2900
F 0 "D2" H 10200 3000 50  0000 C CNN
F 1 "red" H 10200 2800 50  0000 C CNN
F 2 "D0603" H 10200 2900 60  0001 C CNN
F 3 "" H 10200 2900 60  0001 C CNN
	1    10200 2900
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:TACT-SWITCH SW1
U 1 1 4C3EBB24
P 3600 6550
F 0 "SW1" H 3750 6660 50  0000 C CNN
F 1 "Reset" H 3600 6470 50  0000 C CNN
F 2 "TACT-SWITCH" H 3600 6550 60  0001 C CNN
F 3 "" H 3600 6550 60  0001 C CNN
	1    3600 6550
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:TACT-SWITCH SW2
U 1 1 4C3EBB21
P 3650 6950
F 0 "SW2" H 3800 7060 50  0000 C CNN
F 1 "Erase" H 3650 6850 50  0000 C CNN
F 2 "TACT-SWITCH" H 3650 6950 60  0001 C CNN
F 3 "" H 3650 6950 60  0001 C CNN
	1    3650 6950
	1    0    0    -1  
$EndComp
NoConn ~ 1950 1650
Text Notes 2000 1700 0    60   ~ 0
Do not connect
Text Notes 8700 2400 0    60   ~ 0
Detect Master 2.1
$Comp
L tinkerforge:GND #PWR026
U 1 1 53EBE733
P 8650 2400
F 0 "#PWR026" H 8650 2400 30  0001 C CNN
F 1 "GND" H 8650 2330 30  0001 C CNN
F 2 "" H 8650 2400 60  0001 C CNN
F 3 "" H 8650 2400 60  0001 C CNN
	1    8650 2400
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R8
U 1 1 53EC158E
P 5950 4900
F 0 "R8" V 5850 4900 50  0000 C CNN
F 1 "10k" V 5950 4900 50  0000 C CNN
F 2 "R0603" H 5950 4900 60  0001 C CNN
F 3 "" H 5950 4900 60  0001 C CNN
	1    5950 4900
	0    -1   -1   0   
$EndComp
$Comp
L tinkerforge:GND #PWR027
U 1 1 53EC17A8
P 6550 5150
F 0 "#PWR027" H 6550 5150 30  0001 C CNN
F 1 "GND" H 6550 5080 30  0001 C CNN
F 2 "" H 6550 5150 60  0001 C CNN
F 3 "" H 6550 5150 60  0001 C CNN
	1    6550 5150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R11
U 1 1 53EC1ADC
P 6550 4400
F 0 "R11" V 6450 4400 50  0000 C CNN
F 1 "10k" V 6550 4400 50  0000 C CNN
F 2 "R0603" H 6550 4400 60  0001 C CNN
F 3 "" H 6550 4400 60  0001 C CNN
	1    6550 4400
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:3V3 #PWR028
U 1 1 53EC1FFE
P 6550 4100
F 0 "#PWR028" H 6550 4200 40  0001 C CNN
F 1 "3V3" H 6550 4225 40  0000 C CNN
F 2 "" H 6550 4100 60  0001 C CNN
F 3 "" H 6550 4100 60  0001 C CNN
	1    6550 4100
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:WE-CNSW L1
U 1 1 53ECC70F
P 4100 5500
F 0 "L1" H 4100 5670 60  0000 C CNN
F 1 "744230900" H 4100 5340 60  0000 C CNN
F 2 "WE-CNSW-0603" H 4100 5490 60  0001 C CNN
F 3 "" H 4100 5490 60  0000 C CNN
	1    4100 5500
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:TVS_ARRAY4 U2
U 1 1 53ECE270
P 4800 6550
F 0 "U2" V 4455 6385 60  0000 C CNN
F 1 "82400152" V 5160 6535 60  0000 C CNN
F 2 "SOT563-SPEC" H 4800 6535 60  0001 C CNN
F 3 "" H 4800 6535 60  0000 C CNN
	1    4800 6550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 7200 4800 7000
Connection ~ 4800 4900
Wire Wire Line
	4800 4900 5650 4900
Wire Wire Line
	4800 4850 4800 4900
Connection ~ 4800 5700
Wire Wire Line
	4650 5400 4400 5400
Wire Wire Line
	4650 5400 4650 5900
Wire Wire Line
	4400 5600 4550 5600
Wire Wire Line
	3700 5400 3700 5450
Wire Wire Line
	3800 5400 3700 5400
Wire Wire Line
	3700 5600 3700 5550
Wire Wire Line
	3800 5600 3700 5600
Wire Wire Line
	1750 5550 3200 5550
Wire Wire Line
	3200 5450 1750 5450
Wire Wire Line
	6550 4700 6550 4650
Wire Wire Line
	6550 4150 6550 4100
Wire Wire Line
	6550 5150 6550 5100
Wire Wire Line
	6200 4900 6250 4900
Connection ~ 5650 4900
Wire Wire Line
	5650 3500 6000 3500
Wire Wire Line
	5650 4100 5650 3500
Wire Wire Line
	4500 4100 5650 4100
Wire Wire Line
	3000 1550 1950 1550
Wire Wire Line
	8650 2350 8650 2400
Wire Wire Line
	8450 2350 8650 2350
Wire Wire Line
	8450 2400 8450 2350
Wire Wire Line
	7800 2400 8450 2400
Wire Wire Line
	4500 4100 4500 4650
Wire Wire Line
	6250 3500 6250 3550
Connection ~ 6000 3500
Wire Wire Line
	6000 3500 6000 3450
Wire Wire Line
	6000 2950 6000 2900
Wire Wire Line
	6000 4000 6000 4050
Wire Wire Line
	6250 4050 6250 3950
Wire Wire Line
	8650 3600 7800 3600
Wire Wire Line
	8650 3500 8650 3600
Wire Wire Line
	8850 3500 8650 3500
Wire Wire Line
	8200 3900 7800 3900
Wire Wire Line
	9900 2350 9900 2250
Wire Wire Line
	9650 2300 9650 2350
Wire Wire Line
	9650 1250 9650 1200
Wire Wire Line
	3000 2350 1950 2350
Wire Wire Line
	9350 2900 9450 2900
Wire Wire Line
	9850 2600 10000 2600
Wire Wire Line
	9650 5000 9800 5000
Wire Wire Line
	9650 4800 9800 4800
Wire Wire Line
	10200 5050 10300 5050
Wire Wire Line
	10200 5000 10200 5050
Wire Wire Line
	10200 4650 10300 4650
Wire Wire Line
	10200 4800 10200 4650
Wire Wire Line
	1950 750  3000 750 
Wire Wire Line
	7800 3500 8200 3500
Wire Wire Line
	7800 3300 8200 3300
Wire Wire Line
	1950 3350 3000 3350
Wire Wire Line
	7800 1500 8200 1500
Wire Wire Line
	7800 1300 8200 1300
Wire Wire Line
	7800 800  8200 800 
Wire Wire Line
	7800 1000 8200 1000
Wire Wire Line
	9350 2700 7800 2700
Wire Wire Line
	1950 850  3000 850 
Connection ~ 10700 4850
Wire Wire Line
	9350 2700 9350 2900
Connection ~ 10400 2600
Wire Wire Line
	10400 2500 10400 2600
Wire Wire Line
	10400 4350 10400 3850
Wire Wire Line
	10850 4350 10850 4250
Connection ~ 10250 3550
Wire Wire Line
	10250 3300 10250 3550
Wire Wire Line
	1950 3650 3000 3650
Wire Wire Line
	1950 2450 3000 2450
Wire Wire Line
	1750 5250 2900 5250
Connection ~ 3050 6250
Wire Wire Line
	3200 6250 3050 6250
Wire Wire Line
	1950 3150 3000 3150
Wire Wire Line
	2800 6950 3350 6950
Wire Wire Line
	1950 2950 3000 2950
Wire Wire Line
	2800 5650 1750 5650
Wire Wire Line
	1950 2750 3000 2750
Wire Wire Line
	1750 4750 2900 4750
Wire Wire Line
	2900 5050 1750 5050
Wire Wire Line
	3900 6600 3900 6550
Wire Wire Line
	5650 5450 5650 5400
Wire Wire Line
	3050 6550 3300 6550
Wire Wire Line
	3050 5950 3050 6250
Wire Wire Line
	1750 5950 3050 5950
Wire Wire Line
	2900 5150 1750 5150
Wire Wire Line
	1750 5750 2900 5750
Wire Wire Line
	1750 4850 2900 4850
Wire Wire Line
	1950 2650 3000 2650
Wire Wire Line
	3950 6950 4050 6950
Wire Wire Line
	3000 2850 1950 2850
Wire Wire Line
	1750 5350 2900 5350
Wire Wire Line
	1950 3550 3000 3550
Wire Wire Line
	3000 3750 1950 3750
Wire Wire Line
	1950 2550 3000 2550
Wire Wire Line
	10850 3300 10850 3550
Connection ~ 10850 3550
Wire Wire Line
	10250 4350 10250 4250
Wire Wire Line
	1950 3450 3000 3450
Wire Wire Line
	2800 5650 2800 6950
Connection ~ 10700 5050
Wire Wire Line
	10700 4550 10700 4650
Connection ~ 10700 4650
Wire Wire Line
	7800 3700 8200 3700
Wire Wire Line
	7800 2000 8050 2000
Wire Wire Line
	7800 1100 8200 1100
Wire Wire Line
	7800 900  8200 900 
Wire Wire Line
	7800 1200 8200 1200
Wire Wire Line
	7800 1400 8200 1400
Wire Wire Line
	1950 3250 3000 3250
Wire Wire Line
	7800 3200 8200 3200
Wire Wire Line
	7800 3400 8200 3400
Wire Wire Line
	7800 2500 8200 2500
Wire Wire Line
	3000 1750 1950 1750
Wire Wire Line
	10200 4850 10200 4900
Wire Wire Line
	10300 4850 10200 4850
Wire Wire Line
	10200 5250 10200 5100
Wire Wire Line
	10300 5250 10200 5250
Wire Wire Line
	9650 4900 9800 4900
Wire Wire Line
	9650 5100 9800 5100
Wire Wire Line
	9850 2900 10000 2900
Wire Wire Line
	9450 2600 7800 2600
Wire Wire Line
	9650 1800 9650 1750
Wire Wire Line
	9450 2300 9450 1800
Wire Wire Line
	7800 2300 9450 2300
Connection ~ 9650 1800
Wire Wire Line
	9450 1800 9650 1800
Wire Wire Line
	9900 1800 9900 1850
Wire Wire Line
	8850 3550 8850 3500
Wire Wire Line
	4500 4650 1750 4650
Wire Wire Line
	4550 5600 4550 6100
Wire Wire Line
	4650 5900 5050 5900
Wire Wire Line
	5050 5900 5050 6100
Wire Wire Line
	5050 7000 5250 7000
Wire Wire Line
	5250 7000 5250 5800
Wire Wire Line
	4550 7000 4550 7100
Wire Wire Line
	4550 7100 5300 7100
Wire Wire Line
	5300 7100 5300 5900
Wire Wire Line
	6550 4700 6750 4700
$Comp
L tinkerforge:MOSFET_N_CH Q1
U 1 1 58F8DB3E
P 6450 4900
F 0 "Q1" H 6750 4950 50  0000 R CNN
F 1 "2N7002P" H 7000 4850 50  0000 R CNN
F 2 "kicad-libraries:SOT23GDS" H 6650 5000 50  0001 C CNN
F 3 "" H 6450 4900 50  0000 C CNN
	1    6450 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 5700 4800 6100
Wire Wire Line
	5650 4900 5700 4900
Wire Wire Line
	6000 3500 6250 3500
Wire Wire Line
	10700 4850 10700 5050
Wire Wire Line
	10400 2600 10400 2900
Wire Wire Line
	10250 3550 10250 3850
Wire Wire Line
	3050 6250 3050 6550
Wire Wire Line
	10850 3550 10850 3850
Wire Wire Line
	10700 5050 10700 5250
Wire Wire Line
	10700 4650 10700 4850
Wire Wire Line
	9650 1800 9900 1800
NoConn ~ 7800 1700
NoConn ~ 7800 1800
NoConn ~ 7800 1900
Wire Wire Line
	4800 4900 4800 5050
Wire Wire Line
	4800 5550 4800 5700
Wire Wire Line
	5250 5800 6350 5800
Wire Wire Line
	5300 5900 6350 5900
$Comp
L tinkerforge:Rs R5
U 1 1 5EA2FA33
P 6100 6000
F 0 "R5" V 6050 6000 31  0000 C CNN
F 1 "5k1" V 6100 6000 31  0000 C CNN
F 2 "kicad-libraries:R0603F" H 6100 6000 60  0001 C CNN
F 3 "" H 6100 6000 60  0000 C CNN
	1    6100 6000
	0    1    1    0   
$EndComp
Wire Wire Line
	6000 6000 5750 6000
Wire Wire Line
	6200 6000 6350 6000
Wire Wire Line
	5550 6200 5550 6350
Wire Wire Line
	5550 6200 5750 6200
Text GLabel 3000 850  2    60   Output ~ 0
Bricklet_Enable
Wire Wire Line
	7800 2100 8150 2100
$Comp
L tinkerforge:GND #PWR030
U 1 1 5EB48E32
P 8650 2150
F 0 "#PWR030" H 8650 2150 30  0001 C CNN
F 1 "GND" H 8650 2080 30  0001 C CNN
F 2 "" H 8650 2150 60  0001 C CNN
F 3 "" H 8650 2150 60  0001 C CNN
	1    8650 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 2200 8450 2200
Wire Wire Line
	8450 2200 8450 2050
Text Notes 8700 2150 0    60   ~ 0
Detect Master 3.0
Wire Wire Line
	8450 2050 8650 2050
Wire Wire Line
	8650 2050 8650 2150
Wire Wire Line
	8050 1750 8050 2000
Wire Wire Line
	8050 1750 8300 1750
Wire Wire Line
	8150 2100 8150 1850
Wire Wire Line
	8150 1850 8300 1850
Connection ~ 6550 4700
$Comp
L tinkerforge:USB-C-2 J1
U 1 1 5E69E986
P 6650 5950
F 0 "J1" H 6450 6300 50  0000 L CNN
F 1 "USB-C" V 6850 5800 50  0000 L CNN
F 2 "kicad-libraries:USB-C-2" H 6650 5950 50  0001 C CNN
F 3 "" H 6650 5950 50  0001 C CNN
	1    6650 5950
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:Rs R18
U 1 1 5E6B3F8B
P 6100 6100
F 0 "R18" V 6050 6100 31  0000 C CNN
F 1 "5k1" V 6100 6100 31  0000 C CNN
F 2 "kicad-libraries:R0603F" H 6100 6100 60  0001 C CNN
F 3 "" H 6100 6100 60  0000 C CNN
	1    6100 6100
	0    1    1    0   
$EndComp
Wire Wire Line
	5750 6000 5750 6100
Wire Wire Line
	5750 6100 6000 6100
Wire Wire Line
	6200 6100 6350 6100
$Comp
L tinkerforge:Cs C47
U 1 1 5E7C6D61
P 5050 5550
F 0 "C47" V 4950 5550 31  0000 C CNN
F 1 "1uF" V 5150 5550 31  0000 C CNN
F 2 "kicad-libraries:C0603F" H 5050 5550 60  0001 C CNN
F 3 "" H 5050 5550 60  0000 C CNN
	1    5050 5550
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:Cs C46
U 1 1 5E7C90E5
P 5050 5050
F 0 "C46" V 4950 5050 31  0000 C CNN
F 1 "1uF" V 5150 5050 31  0000 C CNN
F 2 "kicad-libraries:C0603F" H 5050 5050 60  0001 C CNN
F 3 "" H 5050 5050 60  0000 C CNN
	1    5050 5050
	0    1    1    0   
$EndComp
Wire Wire Line
	5150 5050 5200 5050
Wire Wire Line
	1750 4550 2900 4550
Wire Wire Line
	1750 4450 2900 4450
Wire Wire Line
	1750 4350 2900 4350
Wire Wire Line
	7800 3000 8200 3000
Wire Wire Line
	7800 3100 8200 3100
Wire Wire Line
	7800 2900 8200 2900
Wire Wire Line
	7800 2800 8200 2800
Text GLabel 8200 2800 2    60   Output ~ 0
STACK-SELECT-00
Text GLabel 8200 2900 2    60   Output ~ 0
STACK-SELECT-01
Text GLabel 8200 3000 2    60   Output ~ 0
STACK-SELECT-02
Text GLabel 8200 3100 2    60   Output ~ 0
STACK-SELECT-03
Wire Wire Line
	1950 950  3000 950 
Wire Wire Line
	1950 1850 3000 1850
Wire Wire Line
	3000 1950 1950 1950
Wire Wire Line
	1950 2050 3000 2050
Wire Wire Line
	3000 2150 1950 2150
Wire Wire Line
	1950 1250 3000 1250
Wire Wire Line
	3000 1350 1950 1350
$Comp
L tinkerforge:GND #PWR032
U 1 1 5E7F4B45
P 5300 5050
F 0 "#PWR032" H 5300 5050 30  0001 C CNN
F 1 "GND" H 5300 4980 30  0001 C CNN
F 2 "" H 5300 5050 60  0001 C CNN
F 3 "" H 5300 5050 60  0001 C CNN
	1    5300 5050
	0    -1   -1   0   
$EndComp
Text Notes 4300 3650 0    60   ~ 0
Select-ex 00 and 01 swapped
Wire Wire Line
	1950 1050 3000 1050
Text Notes 3900 1100 0    60   ~ 0
Changed
Text Notes 4000 900  0    60   ~ 0
Changed
Text Notes 4000 3800 0    60   ~ 0
Changed
Text Notes 3700 1200 0    60   ~ 0
Changed
Wire Wire Line
	1950 1150 3000 1150
Text Notes 3600 1500 0    60   ~ 0
Changed
Wire Wire Line
	1950 1450 3000 1450
Wire Wire Line
	8200 1600 7800 1600
Text Notes 3700 1200 0    60   ~ 0
Changed
Text Notes 9200 1650 0    60   ~ 0
Changed
Text Notes 4000 3500 0    60   ~ 0
Changed
Text GLabel 5400 5600 2    50   Input ~ 0
USB_GND
Wire Wire Line
	5400 5600 5350 5600
Wire Wire Line
	5350 5600 5350 5550
Wire Wire Line
	5150 5550 5200 5550
Wire Wire Line
	4950 5550 4800 5550
Connection ~ 4800 5550
$Comp
L tinkerforge:INDUCT FB101
U 1 1 5EA3107F
P 5200 5300
F 0 "FB101" H 5350 5400 60  0000 C CNN
F 1 "FB" H 5350 5200 60  0000 C CNN
F 2 "C0603" H 5200 5300 60  0001 C CNN
F 3 "" H 5200 5300 60  0001 C CNN
	1    5200 5300
	1    0    0    -1  
$EndComp
Connection ~ 5200 5050
Wire Wire Line
	5200 5050 5300 5050
Connection ~ 5200 5550
Wire Wire Line
	5200 5550 5350 5550
Wire Wire Line
	4950 5050 4800 5050
Connection ~ 4800 5050
Text GLabel 5550 6350 3    50   Input ~ 0
USB_GND
Text GLabel 4800 7200 3    50   Input ~ 0
USB_GND
$Comp
L tinkerforge:INDUCT FB102
U 1 1 5EA6331F
P 6250 6550
F 0 "FB102" H 6400 6650 60  0000 C CNN
F 1 "FB" H 6400 6450 60  0000 C CNN
F 2 "C0603" H 6250 6550 60  0001 C CNN
F 3 "" H 6250 6550 60  0001 C CNN
	1    6250 6550
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR0128
U 1 1 5EA63CEC
P 6250 6900
F 0 "#PWR0128" H 6250 6900 30  0001 C CNN
F 1 "GND" H 6250 6830 30  0001 C CNN
F 2 "" H 6250 6900 60  0001 C CNN
F 3 "" H 6250 6900 60  0001 C CNN
	1    6250 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 6800 6250 6900
Wire Wire Line
	6350 6300 6250 6300
Wire Wire Line
	4800 5700 6350 5700
Wire Wire Line
	5750 6100 5750 6200
Connection ~ 5750 6100
Connection ~ 5750 6200
Wire Wire Line
	5750 6200 6350 6200
NoConn ~ 1950 3850
NoConn ~ 1750 5850
NoConn ~ 7800 3800
$EndSCHEMATC
