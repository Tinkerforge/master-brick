EESchema Schematic File Version 2
LIBS:tinkerforge
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:master-cache
EELAYER 25 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 3 4
Title "Master Brick"
Date "2017-05-08"
Rev "2.2"
Comp "Tinkerforge GmbH"
Comment1 "Licensed under CERN OHL v.1.1"
Comment2 "Copyright (©) 2017, B.Nordmeyer <bastian@tinkerforge.com>"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L VPP #PWR307
U 1 1 5062EA9B
P 15600 6000
F 0 "#PWR307" H 15600 6200 40  0001 C CNN
F 1 "VPP" H 15600 6150 40  0000 C CNN
F 2 "" H 15600 6000 60  0001 C CNN
F 3 "" H 15600 6000 60  0001 C CNN
	1    15600 6000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR306
U 1 1 4EF1D5FA
P 14250 8400
F 0 "#PWR306" H 14250 8400 30  0001 C CNN
F 1 "GND" H 14250 8330 30  0001 C CNN
F 2 "" H 14250 8400 60  0001 C CNN
F 3 "" H 14250 8400 60  0001 C CNN
	1    14250 8400
	1    0    0    -1  
$EndComp
$Comp
L R R304
U 1 1 4EF1D5E6
P 14250 8100
F 0 "R304" V 14330 8100 50  0000 C CNN
F 1 "100k" V 14250 8100 50  0000 C CNN
F 2 "R0603F" H 14250 8100 60  0001 C CNN
F 3 "" H 14250 8100 60  0001 C CNN
	1    14250 8100
	-1   0    0    1   
$EndComp
NoConn ~ 10050 2700
NoConn ~ 10050 2800
$Comp
L R_PACK4 RP305
U 1 1 4D9D8B80
P 1700 4300
F 0 "RP305" H 1700 4750 40  0000 C CNN
F 1 "TBD" H 1700 4250 40  0000 C CNN
F 2 "4X0402" H 1700 4300 60  0001 C CNN
F 3 "" H 1700 4300 60  0001 C CNN
	1    1700 4300
	-1   0    0    -1  
$EndComp
$Comp
L R R301
U 1 1 4D0BA1A4
P 2050 3750
F 0 "R301" V 1950 3750 50  0000 C CNN
F 1 "1k" V 2050 3750 50  0000 C CNN
F 2 "R0603F" H 2050 3750 60  0001 C CNN
F 3 "" H 2050 3750 60  0001 C CNN
	1    2050 3750
	0    1    1    0   
$EndComp
Text GLabel 10250 2900 2    60   Input ~ 0
SCL-PULLUP
Text GLabel 10250 2600 2    60   Input ~ 0
SDA-PULLUP
Text GLabel 3100 4550 0    60   Input ~ 0
STACK-EX-0-GP02
Text GLabel 3100 4650 0    60   Input ~ 0
STACK-EX-1-GP00
$Comp
L GND #PWR302
U 1 1 4CC860F2
P 5950 8350
F 0 "#PWR302" H 5950 8350 30  0001 C CNN
F 1 "GND" H 5950 8280 30  0001 C CNN
F 2 "" H 5950 8350 60  0001 C CNN
F 3 "" H 5950 8350 60  0001 C CNN
	1    5950 8350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR301
U 1 1 4CC860E5
P 5950 5350
F 0 "#PWR301" H 5950 5350 30  0001 C CNN
F 1 "GND" H 5950 5280 30  0001 C CNN
F 2 "" H 5950 5350 60  0001 C CNN
F 3 "" H 5950 5350 60  0001 C CNN
	1    5950 5350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR305
U 1 1 4CC860D5
P 12850 8200
F 0 "#PWR305" H 12850 8200 30  0001 C CNN
F 1 "GND" H 12850 8130 30  0001 C CNN
F 2 "" H 12850 8200 60  0001 C CNN
F 3 "" H 12850 8200 60  0001 C CNN
	1    12850 8200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR304
U 1 1 4CC860D1
P 12850 5150
F 0 "#PWR304" H 12850 5150 30  0001 C CNN
F 1 "GND" H 12850 5080 30  0001 C CNN
F 2 "" H 12850 5150 60  0001 C CNN
F 3 "" H 12850 5150 60  0001 C CNN
	1    12850 5150
	1    0    0    -1  
$EndComp
Text GLabel 3100 6900 0    60   Output ~ 0
STACK-MASTER-DETECT
Text GLabel 8650 4850 2    60   Input ~ 0
STACK-SER2-RTS
$Comp
L STACK-DOWN-1 J302
U 1 1 4C46D30D
P 5950 7200
F 0 "J302" H 5100 8050 60  0000 C CNN
F 1 "STACK-DOWN" H 6450 8050 60  0000 C CNN
F 2 "BTB08-ACS-BTM" H 5950 7200 60  0001 C CNN
F 3 "" H 5950 7200 60  0001 C CNN
	1    5950 7200
	1    0    0    -1  
$EndComp
Text GLabel 3100 4850 0    60   Input ~ 0
STACK-EX-1-GP02
Text GLabel 8650 4950 2    60   Input ~ 0
STACK-SPI-SELECT
Text GLabel 3100 3950 0    60   Output ~ 0
STACK-SYNC/JTAG-TCK
Text GLabel 3100 3850 0    60   Output ~ 0
STACK-HIGH/JTAG-TMS
Text GLabel 10350 8050 0    60   Output ~ 0
STACK-VOLTAGE
Text GLabel 10350 7850 0    60   Output ~ 0
STACK-CURRENT
$Comp
L STACK-UP-2 J303
U 1 1 4C46D315
P 12850 4100
F 0 "J303" H 12000 4950 60  0000 C CNN
F 1 "STACK-UP-LOW" H 13450 4950 60  0000 C CNN
F 2 "BTB08-ACS-TOP" H 12850 4100 60  0001 C CNN
F 3 "" H 12850 4100 60  0001 C CNN
	1    12850 4100
	1    0    0    -1  
$EndComp
$Comp
L STACK-DOWN-2 J304
U 1 1 4C46D31B
P 12850 7150
F 0 "J304" H 12000 8000 60  0000 C CNN
F 1 "STACK-DOWN" H 13400 8000 60  0000 C CNN
F 2 "BTB08-ACS-BTM" H 12850 7150 60  0001 C CNN
F 3 "" H 12850 7150 60  0001 C CNN
	1    12850 7150
	1    0    0    -1  
$EndComp
Text GLabel 8650 4750 2    60   Input ~ 0
STACK-SER2-TXD
Text GLabel 8650 4650 2    60   Input ~ 0
STACK-SER2-RXD
Text GLabel 1750 3750 0    60   Output ~ 0
STACK-RESET
Text GLabel 3100 4750 0    60   Input ~ 0
STACK-EX-1-GP01
Text GLabel 10350 4550 2    60   Input ~ 0
STACK-SELECT-EX-01
Text GLabel 10350 4450 2    60   Input ~ 0
STACK-SELECT-EX-00
Text GLabel 9750 4350 2    60   Input ~ 0
STACK-SELECT-07
Text GLabel 9750 4250 2    60   Input ~ 0
STACK-SELECT-06
Text GLabel 9750 4150 2    60   Input ~ 0
STACK-SELECT-05
Text GLabel 9750 4050 2    60   Input ~ 0
STACK-SELECT-04
Text GLabel 9750 3950 2    60   Input ~ 0
STACK-SELECT-03
Text GLabel 9750 3850 2    60   Input ~ 0
STACK-SELECT-02
Text GLabel 9750 3750 2    60   Input ~ 0
STACK-SELECT-01
Text GLabel 9750 3650 2    60   Input ~ 0
STACK-SELECT-00
Text GLabel 3100 4450 0    60   Input ~ 0
STACK-EX-0-GP01
Text GLabel 3100 4350 0    60   Input ~ 0
STACK-EX-0-GP00
Text GLabel 1450 4250 0    60   Input ~ 0
STACK-SER-SCK
Text GLabel 1450 4150 0    60   Input ~ 0
STACK-SER-TXD
Text GLabel 1450 4050 0    60   Input ~ 0
STACK-SER-RXD
Text GLabel 9300 3550 2    60   Input ~ 0
STACK-I2C-SCL/JTAG-TDO
Text GLabel 9300 3450 2    60   Input ~ 0
STACK-I2C-SDA/JTAG-TDI
Text GLabel 1750 3650 0    60   Input ~ 0
STACK-SPI-SCLK
Text GLabel 1750 3550 0    60   Input ~ 0
STACK-SPI-MOSI
Text GLabel 1750 3450 0    60   Input ~ 0
STACK-SPI-MISO
$Comp
L GND #PWR303
U 1 1 4C46D480
P 12350 5650
F 0 "#PWR303" H 12350 5650 30  0001 C CNN
F 1 "GND" H 12350 5580 30  0001 C CNN
F 2 "" H 12350 5650 60  0001 C CNN
F 3 "" H 12350 5650 60  0001 C CNN
	1    12350 5650
	1    0    0    -1  
$EndComp
Text GLabel 12800 5450 0    60   Output ~ 0
VSTACK
$Comp
L STACK-UP-1 J301
U 1 1 4C46D305
P 5950 4150
F 0 "J301" H 5100 5000 60  0000 C CNN
F 1 "STACK-UP-LOW" H 6550 5000 60  0000 C CNN
F 2 "BTB08-ACS-TOP" H 5950 4150 60  0001 C CNN
F 3 "" H 5950 4150 60  0001 C CNN
	1    5950 4150
	1    0    0    -1  
$EndComp
$Comp
L FILTER FB301
U 1 1 4C458E31
P 12000 5450
F 0 "FB301" H 12000 5600 60  0000 C CNN
F 1 "0" H 12000 5350 60  0000 C CNN
F 2 "R0805" H 12000 5450 60  0001 C CNN
F 3 "" H 12000 5450 60  0001 C CNN
	1    12000 5450
	1    0    0    -1  
$EndComp
$Comp
L FILTER FB302
U 1 1 4C458E1C
P 13300 5450
F 0 "FB302" H 13300 5600 60  0000 C CNN
F 1 "FB" H 13300 5350 60  0000 C CNN
F 2 "R0603F" H 13300 5450 60  0001 C CNN
F 3 "" H 13300 5450 60  0001 C CNN
	1    13300 5450
	1    0    0    -1  
$EndComp
$Comp
L FILTER FB303
U 1 1 54B90963
P 15100 6050
F 0 "FB303" H 15100 6200 60  0000 C CNN
F 1 "FB" H 15100 5950 60  0000 C CNN
F 2 "R0603F" H 15100 6050 60  0001 C CNN
F 3 "" H 15100 6050 60  0001 C CNN
	1    15100 6050
	1    0    0    -1  
$EndComp
$Comp
L R R302
U 1 1 54B91D05
P 11050 7850
F 0 "R302" V 10950 7850 50  0000 C CNN
F 1 "10" V 11050 7850 50  0000 C CNN
F 2 "R0603F" H 11050 7850 60  0001 C CNN
F 3 "" H 11050 7850 60  0001 C CNN
	1    11050 7850
	0    1    1    0   
$EndComp
$Comp
L R R303
U 1 1 54B91D0B
P 11050 8050
F 0 "R303" V 10950 8050 50  0000 C CNN
F 1 "10" V 11050 8050 50  0000 C CNN
F 2 "R0603F" H 11050 8050 60  0001 C CNN
F 3 "" H 11050 8050 60  0001 C CNN
	1    11050 8050
	0    1    1    0   
$EndComp
NoConn ~ 9650 2700
NoConn ~ 9650 2800
Connection ~ 14250 7850
Wire Wire Line
	14250 8400 14250 8350
Wire Wire Line
	10050 2600 10250 2600
Connection ~ 7400 4850
Wire Wire Line
	7400 4850 7400 7900
Wire Wire Line
	7400 7900 7150 7900
Connection ~ 7500 4650
Wire Wire Line
	7150 7700 7500 7700
Wire Wire Line
	7500 7700 7500 4650
Connection ~ 7700 4450
Wire Wire Line
	7700 4450 7700 7500
Wire Wire Line
	7150 4350 9300 4350
Wire Wire Line
	7700 7500 7150 7500
Connection ~ 7900 4150
Wire Wire Line
	7900 4150 7900 7300
Wire Wire Line
	7900 7300 7150 7300
Connection ~ 8000 3950
Wire Wire Line
	8000 3950 8000 7100
Wire Wire Line
	8000 7100 7150 7100
Connection ~ 8100 3750
Wire Wire Line
	8100 3750 8100 6900
Wire Wire Line
	8100 6900 7150 6900
Connection ~ 8350 3450
Wire Wire Line
	8350 6500 7150 6500
Connection ~ 4250 4150
Wire Wire Line
	7150 4250 9300 4250
Wire Wire Line
	4750 7200 4250 7200
Wire Wire Line
	4250 7200 4250 4150
Wire Wire Line
	4400 4350 4400 7400
Wire Wire Line
	4400 7400 4750 7400
Connection ~ 4500 4550
Wire Wire Line
	4750 7600 4500 7600
Wire Wire Line
	4500 7600 4500 4550
Connection ~ 4650 4750
Wire Wire Line
	4750 7800 4650 7800
Wire Wire Line
	4650 7800 4650 4750
Wire Wire Line
	5950 5350 5950 5200
Wire Wire Line
	12850 5150 12850 5050
Wire Wire Line
	14050 4700 14300 4700
Wire Wire Line
	14300 4700 14300 7750
Wire Wire Line
	14300 7750 14050 7750
Wire Wire Line
	14050 7550 14150 7550
Wire Wire Line
	7150 4050 9300 4050
Wire Wire Line
	7150 3850 9300 3850
Wire Wire Line
	7150 3450 9300 3450
Connection ~ 3850 3450
Wire Wire Line
	4750 6500 3850 6500
Wire Wire Line
	3850 6500 3850 3450
Connection ~ 3950 3650
Wire Wire Line
	4750 6700 3950 6700
Wire Wire Line
	3950 6700 3950 3650
Connection ~ 11550 5450
Wire Wire Line
	11650 5450 11550 5450
Wire Wire Line
	11500 3850 11500 6900
Connection ~ 14150 5450
Connection ~ 11450 7850
Wire Wire Line
	11450 7850 11450 4800
Wire Wire Line
	11450 4800 11650 4800
Wire Wire Line
	14050 8050 14050 7850
Wire Wire Line
	14050 8050 11300 8050
Connection ~ 11650 3850
Wire Wire Line
	11500 3850 11650 3850
Connection ~ 11650 6900
Wire Wire Line
	11500 6900 11650 6900
Connection ~ 11650 4550
Wire Wire Line
	11650 4550 11550 4550
Wire Wire Line
	11550 4550 11550 7600
Connection ~ 11650 7600
Wire Wire Line
	11550 7600 11650 7600
Connection ~ 11650 7650
Connection ~ 14050 6650
Connection ~ 14050 6850
Connection ~ 14050 7050
Connection ~ 14050 7250
Connection ~ 11650 7250
Connection ~ 11650 7150
Wire Wire Line
	11650 7350 11650 6450
Connection ~ 11650 6950
Connection ~ 11650 7050
Connection ~ 11650 6850
Connection ~ 11650 6650
Connection ~ 14050 4500
Connection ~ 11650 4600
Wire Wire Line
	11650 4400 11650 4700
Connection ~ 14050 4100
Connection ~ 14050 3900
Connection ~ 14050 3600
Connection ~ 11650 4100
Connection ~ 11650 3900
Connection ~ 11650 3700
Connection ~ 11650 3500
Connection ~ 11650 3600
Connection ~ 11650 3800
Connection ~ 11650 4000
Wire Wire Line
	11650 3400 11650 4300
Connection ~ 11650 4200
Connection ~ 14050 3500
Connection ~ 14050 3700
Connection ~ 14050 3800
Connection ~ 14050 4000
Wire Wire Line
	14050 3400 14050 4300
Connection ~ 14050 4200
Connection ~ 11650 4500
Wire Wire Line
	14050 4400 14050 4600
Connection ~ 11650 6550
Connection ~ 11650 6750
Connection ~ 14050 7150
Connection ~ 14050 6950
Connection ~ 14050 6750
Wire Wire Line
	14050 7350 14050 6450
Connection ~ 14050 6550
Connection ~ 14050 7550
Wire Wire Line
	11650 7750 11650 7450
Connection ~ 11650 7550
Wire Wire Line
	14050 6900 14200 6900
Connection ~ 14050 6900
Wire Wire Line
	14200 6900 14200 3850
Wire Wire Line
	14200 3850 14050 3850
Connection ~ 14050 3850
Wire Wire Line
	11300 7850 11650 7850
Wire Wire Line
	14050 4800 14250 4800
Wire Wire Line
	14250 4800 14250 7850
Wire Wire Line
	14250 7850 14050 7850
Wire Wire Line
	13650 5450 14150 5450
Wire Wire Line
	12350 5650 12350 5450
Wire Wire Line
	4050 3750 4050 6800
Wire Wire Line
	4050 6800 4750 6800
Connection ~ 4050 3750
Wire Wire Line
	3900 3550 3900 6600
Wire Wire Line
	3900 6600 4750 6600
Connection ~ 3900 3550
Wire Wire Line
	7150 3550 9300 3550
Wire Wire Line
	7150 3750 9300 3750
Wire Wire Line
	7150 3950 9300 3950
Wire Wire Line
	7150 4150 9300 4150
Wire Wire Line
	14050 4500 14150 4500
Wire Wire Line
	14150 4500 14150 7550
Wire Wire Line
	14050 7450 14050 7650
Connection ~ 14300 6050
Wire Wire Line
	12850 8200 12850 8100
Wire Wire Line
	5950 8350 5950 8250
Wire Wire Line
	4700 4850 4700 7900
Wire Wire Line
	4700 7900 4750 7900
Connection ~ 4700 4850
Wire Wire Line
	4600 4650 4600 7700
Wire Wire Line
	4600 7700 4750 7700
Connection ~ 4600 4650
Wire Wire Line
	4450 4450 4450 7500
Wire Wire Line
	4450 7500 4750 7500
Connection ~ 4450 4450
Wire Wire Line
	4300 4250 4300 7300
Wire Wire Line
	4300 7300 4750 7300
Connection ~ 4300 4250
Wire Wire Line
	4750 7100 4200 7100
Wire Wire Line
	4200 7100 4200 4050
Wire Wire Line
	7150 4550 9800 4550
Connection ~ 4200 4050
Connection ~ 4400 4350
Connection ~ 14050 7850
Wire Wire Line
	7150 3650 9300 3650
Wire Wire Line
	7150 6600 8300 6600
Connection ~ 8300 3550
Wire Wire Line
	7150 6800 8150 6800
Wire Wire Line
	8150 6800 8150 3650
Connection ~ 8150 3650
Wire Wire Line
	7150 7000 8050 7000
Wire Wire Line
	8050 7000 8050 3850
Connection ~ 8050 3850
Wire Wire Line
	7150 7200 7950 7200
Wire Wire Line
	7950 7200 7950 4050
Connection ~ 7950 4050
Wire Wire Line
	7150 7400 7850 7400
Wire Wire Line
	7850 7400 7850 4250
Connection ~ 7850 4250
Wire Wire Line
	7150 7600 7650 7600
Wire Wire Line
	7150 4450 9850 4450
Wire Wire Line
	7650 7600 7650 4550
Connection ~ 7650 4550
Wire Wire Line
	7150 7800 7450 7800
Wire Wire Line
	7450 7800 7450 4750
Connection ~ 7450 4750
Wire Wire Line
	4100 3950 4100 7000
Wire Wire Line
	4100 7000 4750 7000
Connection ~ 4100 3950
Wire Wire Line
	10050 2900 10250 2900
Wire Wire Line
	3100 3450 4750 3450
Wire Wire Line
	3100 3550 4750 3550
Wire Wire Line
	3100 3650 4750 3650
Wire Wire Line
	3100 4850 4750 4850
Wire Wire Line
	3100 4750 4750 4750
Wire Wire Line
	3100 4650 4750 4650
Wire Wire Line
	3100 4550 4750 4550
Wire Wire Line
	3100 4450 4750 4450
Wire Wire Line
	3100 4350 4750 4350
Wire Wire Line
	1900 4250 4750 4250
Wire Wire Line
	1900 4150 4750 4150
Wire Wire Line
	1900 4050 4750 4050
Wire Wire Line
	3100 3950 4750 3950
Wire Wire Line
	3100 3850 4750 3850
Wire Wire Line
	2300 3750 4750 3750
Wire Wire Line
	1800 3750 1750 3750
Wire Wire Line
	8350 3450 8350 6500
Wire Wire Line
	8300 6600 8300 3550
Wire Wire Line
	4750 6900 3750 6900
Wire Wire Line
	3750 6900 3750 4950
Wire Wire Line
	3150 4950 3150 6900
Wire Wire Line
	3150 6900 3100 6900
Wire Wire Line
	7150 6700 8450 6700
Wire Wire Line
	8450 6700 8450 4950
Wire Wire Line
	14300 6050 14750 6050
Wire Wire Line
	15600 6000 15600 6050
Wire Wire Line
	12800 5450 12950 5450
Wire Wire Line
	10800 8050 10350 8050
Wire Wire Line
	10350 7850 10800 7850
Wire Wire Line
	9650 2900 9250 2900
Wire Wire Line
	9250 2900 9250 3550
Connection ~ 9250 3550
Wire Wire Line
	9650 2600 9150 2600
Wire Wire Line
	9150 2600 9150 3450
Connection ~ 9150 3450
Wire Wire Line
	3750 4950 3150 4950
Wire Wire Line
	15600 6050 15450 6050
$Comp
L R_PACK4 RP302
U 1 1 5911D9E2
P 2900 3700
F 0 "RP302" H 2900 4150 40  0000 C CNN
F 1 "TBD" H 2900 3900 40  0000 C CNN
F 2 "4X0402" H 2900 3700 60  0001 C CNN
F 3 "" H 2900 3700 60  0001 C CNN
	1    2900 3700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1750 3650 2700 3650
Wire Wire Line
	2700 3550 1750 3550
Wire Wire Line
	1750 3450 2700 3450
NoConn ~ 3100 3350
NoConn ~ 2700 3350
$Comp
L R_PACK4 RP301
U 1 1 5911E4C6
P 9850 2950
F 0 "RP301" H 9850 3400 40  0000 C CNN
F 1 "1k" H 9850 3150 40  0000 C CNN
F 2 "0603X4" H 9850 2950 60  0001 C CNN
F 3 "" H 9850 2950 60  0001 C CNN
	1    9850 2950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1450 4250 1500 4250
Wire Wire Line
	1500 4150 1450 4150
Wire Wire Line
	1450 4050 1500 4050
NoConn ~ 1500 3950
NoConn ~ 1900 3950
$Comp
L R_PACK4 RP303
U 1 1 5911ED4C
P 9500 4000
F 0 "RP303" H 9500 4300 40  0000 C CNN
F 1 "TBD" H 9500 4200 40  0000 C CNN
F 2 "4X0402" H 9500 4000 60  0001 C CNN
F 3 "" H 9500 4000 60  0001 C CNN
	1    9500 4000
	-1   0    0    -1  
$EndComp
$Comp
L R_PACK4 RP304
U 1 1 5911EFC9
P 9500 4400
F 0 "RP304" H 9500 4700 40  0000 C CNN
F 1 "TBD" H 9500 4600 40  0000 C CNN
F 2 "4X0402" H 9500 4400 60  0001 C CNN
F 3 "" H 9500 4400 60  0001 C CNN
	1    9500 4400
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9700 3650 9750 3650
Wire Wire Line
	9750 3750 9700 3750
Wire Wire Line
	9700 3850 9750 3850
Wire Wire Line
	9750 3950 9700 3950
Wire Wire Line
	9700 4050 9750 4050
Wire Wire Line
	9750 4150 9700 4150
Wire Wire Line
	9700 4250 9750 4250
Wire Wire Line
	9750 4350 9700 4350
$Comp
L R_PACK4 RP306
U 1 1 5911F96B
P 10050 4800
F 0 "RP306" H 10050 5100 40  0000 C CNN
F 1 "TBD" H 10050 5000 40  0000 C CNN
F 2 "4X0402" H 10050 4800 60  0001 C CNN
F 3 "" H 10050 4800 60  0001 C CNN
	1    10050 4800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9800 4550 9800 4750
Wire Wire Line
	9800 4750 9850 4750
Wire Wire Line
	10250 4750 10300 4750
Wire Wire Line
	10300 4750 10300 4550
Wire Wire Line
	10300 4550 10350 4550
Wire Wire Line
	10250 4450 10350 4450
NoConn ~ 10250 4550
NoConn ~ 10250 4650
NoConn ~ 9850 4650
NoConn ~ 9850 4550
Wire Wire Line
	8650 4850 7150 4850
Wire Wire Line
	7150 4750 8650 4750
Wire Wire Line
	8650 4650 7150 4650
Wire Wire Line
	8450 4950 8650 4950
$EndSCHEMATC
