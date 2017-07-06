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
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
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
L ZENER D403
U 1 1 5063159F
P 7700 5000
F 0 "D403" H 7700 5100 50  0000 C CNN
F 1 "Z3.3V" H 7700 4900 40  0000 C CNN
F 2 "MiniMelf" H 7700 5000 60  0001 C CNN
F 3 "" H 7700 5000 60  0001 C CNN
	1    7700 5000
	0    1    1    0   
$EndComp
$Comp
L ZENER D402
U 1 1 50631580
P 5000 5150
F 0 "D402" H 5000 5250 50  0000 C CNN
F 1 "Z5V" H 5000 5050 40  0000 C CNN
F 2 "MiniMelf" H 5000 5150 60  0001 C CNN
F 3 "" H 5000 5150 60  0001 C CNN
	1    5000 5150
	0    1    1    0   
$EndComp
$Comp
L DIODESCH D401
U 1 1 50630D5F
P 4300 4800
F 0 "D401" H 4300 4900 40  0000 C CNN
F 1 "PMEG4050" H 4300 4700 40  0000 C CNN
F 2 "SOD-128" H 4300 4800 60  0001 C CNN
F 3 "" H 4300 4800 60  0001 C CNN
	1    4300 4800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR075
U 1 1 5062F5F6
P 5000 6050
F 0 "#PWR075" H 5000 6050 30  0001 C CNN
F 1 "GND" H 5000 5980 30  0001 C CNN
F 2 "" H 5000 6050 60  0001 C CNN
F 3 "" H 5000 6050 60  0001 C CNN
	1    5000 6050
	1    0    0    -1  
$EndComp
$Comp
L C C407
U 1 1 5062F47A
P 5400 5150
F 0 "C407" H 5400 5250 50  0000 L CNN
F 1 "10µF/10V" V 5250 4950 50  0000 L CNN
F 2 "C0805" H 5400 5150 60  0001 C CNN
F 3 "" H 5400 5150 60  0001 C CNN
	1    5400 5150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR076
U 1 1 5062F472
P 4500 6050
F 0 "#PWR076" H 4500 6050 30  0001 C CNN
F 1 "GND" H 4500 5980 30  0001 C CNN
F 2 "" H 4500 6050 60  0001 C CNN
F 3 "" H 4500 6050 60  0001 C CNN
	1    4500 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR077
U 1 1 5062F446
P 8150 6050
F 0 "#PWR077" H 8150 6050 30  0001 C CNN
F 1 "GND" H 8150 5980 30  0001 C CNN
F 2 "" H 8150 6050 60  0001 C CNN
F 3 "" H 8150 6050 60  0001 C CNN
	1    8150 6050
	1    0    0    -1  
$EndComp
$Comp
L C C421
U 1 1 5062F442
P 8150 5000
F 0 "C421" H 8150 5100 50  0000 L CNN
F 1 "10µF/10V" V 8000 4800 50  0000 L CNN
F 2 "C0805" H 8150 5000 60  0001 C CNN
F 3 "" H 8150 5000 60  0001 C CNN
	1    8150 5000
	1    0    0    -1  
$EndComp
$Comp
L LD1117 U401
U 1 1 5062F377
P 6350 5000
F 0 "U401" H 6150 5500 60  0000 C CNN
F 1 "LD1117" H 6350 5300 60  0000 C CNN
F 2 "SOT-223" H 6350 5000 60  0001 C CNN
F 3 "" H 6350 5000 60  0001 C CNN
	1    6350 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR078
U 1 1 5062F08B
P 9900 6050
F 0 "#PWR078" H 9900 6050 30  0001 C CNN
F 1 "GND" H 9900 5980 30  0001 C CNN
F 2 "" H 9900 6050 60  0001 C CNN
F 3 "" H 9900 6050 60  0001 C CNN
	1    9900 6050
	1    0    0    -1  
$EndComp
$Comp
L C C425
U 1 1 5062F087
P 9900 5100
F 0 "C425" H 9900 5200 50  0000 L CNN
F 1 "10µF/10V" V 9750 4900 50  0000 L CNN
F 2 "C0805" H 9900 5100 60  0001 C CNN
F 3 "" H 9900 5100 60  0001 C CNN
	1    9900 5100
	1    0    0    -1  
$EndComp
Text GLabel 8700 5000 0    60   Input ~ 0
3V3-EN
$Comp
L GND #PWR079
U 1 1 5062F04A
P 8850 6050
F 0 "#PWR079" H 8850 6050 30  0001 C CNN
F 1 "GND" H 8850 5980 30  0001 C CNN
F 2 "" H 8850 6050 60  0001 C CNN
F 3 "" H 8850 6050 60  0001 C CNN
	1    8850 6050
	1    0    0    -1  
$EndComp
$Comp
L R R402
U 1 1 5062F03A
P 8850 5450
F 0 "R402" V 8950 5450 50  0000 C CNN
F 1 "10k" V 8850 5450 50  0000 C CNN
F 2 "R0603F" H 8850 5450 60  0001 C CNN
F 3 "" H 8850 5450 60  0001 C CNN
	1    8850 5450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR080
U 1 1 5062F033
P 9600 6050
F 0 "#PWR080" H 9600 6050 30  0001 C CNN
F 1 "GND" H 9600 5980 30  0001 C CNN
F 2 "" H 9600 6050 60  0001 C CNN
F 3 "" H 9600 6050 60  0001 C CNN
	1    9600 6050
	1    0    0    -1  
$EndComp
$Comp
L RT9701XB U402
U 1 1 5062F01C
P 9200 4800
F 0 "U402" H 9050 5000 60  0000 C CNN
F 1 "RT9701XB" H 9200 4450 60  0000 C CNN
F 2 "SOT23-5" H 9200 4800 60  0001 C CNN
F 3 "" H 9200 4800 60  0001 C CNN
	1    9200 4800
	1    0    0    -1  
$EndComp
$Comp
L VPP #PWR081
U 1 1 5062EAAD
P 9900 4800
F 0 "#PWR081" H 9900 5000 40  0001 C CNN
F 1 "VPP" H 9900 4950 40  0000 C CNN
F 2 "" H 9900 4800 60  0001 C CNN
F 3 "" H 9900 4800 60  0001 C CNN
	1    9900 4800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR082
U 1 1 4D232249
P 5000 3800
F 0 "#PWR082" H 5000 3800 30  0001 C CNN
F 1 "GND" H 5000 3730 30  0001 C CNN
F 2 "" H 5000 3800 60  0001 C CNN
F 3 "" H 5000 3800 60  0001 C CNN
	1    5000 3800
	1    0    0    -1  
$EndComp
$Comp
L C C404
U 1 1 4D232246
P 4750 3050
F 0 "C404" H 4750 3150 50  0000 L CNN
F 1 "4.7µF" H 4750 2950 50  0000 L CNN
F 2 "C0603F" H 4750 3050 60  0001 C CNN
F 3 "" H 4750 3050 60  0001 C CNN
	1    4750 3050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR083
U 1 1 4D22D115
P 6250 3800
F 0 "#PWR083" H 6250 3800 30  0001 C CNN
F 1 "GND" H 6250 3730 30  0001 C CNN
F 2 "" H 6250 3800 60  0001 C CNN
F 3 "" H 6250 3800 60  0001 C CNN
	1    6250 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR084
U 1 1 4D22D113
P 6000 3800
F 0 "#PWR084" H 6000 3800 30  0001 C CNN
F 1 "GND" H 6000 3730 30  0001 C CNN
F 2 "" H 6000 3800 60  0001 C CNN
F 3 "" H 6000 3800 60  0001 C CNN
	1    6000 3800
	1    0    0    -1  
$EndComp
$Comp
L C C412
U 1 1 4D22D111
P 6250 3050
F 0 "C412" H 6250 3150 50  0000 L CNN
F 1 "100nF" H 6250 2950 50  0000 L CNN
F 2 "C0603F" H 6250 3050 60  0001 C CNN
F 3 "" H 6250 3050 60  0001 C CNN
	1    6250 3050
	1    0    0    -1  
$EndComp
$Comp
L C C411
U 1 1 4D22D10F
P 6000 3050
F 0 "C411" H 6000 3150 50  0000 L CNN
F 1 "100nF" H 6000 2950 50  0000 L CNN
F 2 "C0603F" H 6000 3050 60  0001 C CNN
F 3 "" H 6000 3050 60  0001 C CNN
	1    6000 3050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR085
U 1 1 4D22D103
P 7900 3800
F 0 "#PWR085" H 7900 3800 30  0001 C CNN
F 1 "GND" H 7900 3730 30  0001 C CNN
F 2 "" H 7900 3800 60  0001 C CNN
F 3 "" H 7900 3800 60  0001 C CNN
	1    7900 3800
	1    0    0    -1  
$EndComp
$Comp
L C C420
U 1 1 4D22D100
P 7900 3050
F 0 "C420" H 7900 3150 50  0000 L CNN
F 1 "100nF" H 7900 2950 50  0000 L CNN
F 2 "C0603F" H 7900 3050 60  0001 C CNN
F 3 "" H 7900 3050 60  0001 C CNN
	1    7900 3050
	1    0    0    -1  
$EndComp
$Comp
L AT91SAM3SXC U101
U 4 1 4D22D010
P 3350 1950
F 0 "U101" H 3400 2000 60  0000 C CNN
F 1 "AT91SAM3SXC" H 3950 2000 60  0000 C CNN
F 2 "LQFP100" H 3350 1950 60  0001 C CNN
F 3 "" H 3350 1950 60  0001 C CNN
	4    3350 1950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR086
U 1 1 4D0EFD31
P 7700 6050
F 0 "#PWR086" H 7700 6050 30  0001 C CNN
F 1 "GND" H 7700 5980 30  0001 C CNN
F 2 "" H 7700 6050 60  0001 C CNN
F 3 "" H 7700 6050 60  0001 C CNN
	1    7700 6050
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR087
U 1 1 4CAC88A1
P 4750 4650
F 0 "#PWR087" H 4750 4740 20  0001 C CNN
F 1 "+5V" H 4750 4740 30  0000 C CNN
F 2 "" H 4750 4650 60  0001 C CNN
F 3 "" H 4750 4650 60  0001 C CNN
	1    4750 4650
	1    0    0    -1  
$EndComp
$Comp
L 3V3 #PWR088
U 1 1 4CAC191E
P 9400 1900
F 0 "#PWR088" H 9400 2000 40  0001 C CNN
F 1 "3V3" H 9400 2025 40  0000 C CNN
F 2 "" H 9400 1900 60  0001 C CNN
F 3 "" H 9400 1900 60  0001 C CNN
	1    9400 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR089
U 1 1 4CAC1791
P 7650 3800
F 0 "#PWR089" H 7650 3800 30  0001 C CNN
F 1 "GND" H 7650 3730 30  0001 C CNN
F 2 "" H 7650 3800 60  0001 C CNN
F 3 "" H 7650 3800 60  0001 C CNN
	1    7650 3800
	1    0    0    -1  
$EndComp
$Comp
L C C419
U 1 1 4CAC178E
P 7650 3050
F 0 "C419" H 7650 3150 50  0000 L CNN
F 1 "100nF" H 7650 2950 50  0000 L CNN
F 2 "C0603F" H 7650 3050 60  0001 C CNN
F 3 "" H 7650 3050 60  0001 C CNN
	1    7650 3050
	1    0    0    -1  
$EndComp
Text GLabel 3950 5100 0    60   Input ~ 0
VSTACK
Text GLabel 3950 4800 0    60   Input ~ 0
VUSB
$Comp
L GND #PWR090
U 1 1 4C45A07C
P 9150 3800
F 0 "#PWR090" H 9150 3800 30  0001 C CNN
F 1 "GND" H 9150 3730 30  0001 C CNN
F 2 "" H 9150 3800 60  0001 C CNN
F 3 "" H 9150 3800 60  0001 C CNN
	1    9150 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR091
U 1 1 4C459C9B
P 4750 3800
F 0 "#PWR091" H 4750 3800 30  0001 C CNN
F 1 "GND" H 4750 3730 30  0001 C CNN
F 2 "" H 4750 3800 60  0001 C CNN
F 3 "" H 4750 3800 60  0001 C CNN
	1    4750 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR092
U 1 1 4C459C99
P 5250 3800
F 0 "#PWR092" H 5250 3800 30  0001 C CNN
F 1 "GND" H 5250 3730 30  0001 C CNN
F 2 "" H 5250 3800 60  0001 C CNN
F 3 "" H 5250 3800 60  0001 C CNN
	1    5250 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR093
U 1 1 4C459C98
P 5500 3800
F 0 "#PWR093" H 5500 3800 30  0001 C CNN
F 1 "GND" H 5500 3730 30  0001 C CNN
F 2 "" H 5500 3800 60  0001 C CNN
F 3 "" H 5500 3800 60  0001 C CNN
	1    5500 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR094
U 1 1 4C459C96
P 5750 3800
F 0 "#PWR094" H 5750 3800 30  0001 C CNN
F 1 "GND" H 5750 3730 30  0001 C CNN
F 2 "" H 5750 3800 60  0001 C CNN
F 3 "" H 5750 3800 60  0001 C CNN
	1    5750 3800
	1    0    0    -1  
$EndComp
$Comp
L C C406
U 1 1 4C459C51
P 5250 3050
F 0 "C406" H 5250 3150 50  0000 L CNN
F 1 "100nF" H 5250 2950 50  0000 L CNN
F 2 "C0603F" H 5250 3050 60  0001 C CNN
F 3 "" H 5250 3050 60  0001 C CNN
	1    5250 3050
	1    0    0    -1  
$EndComp
$Comp
L C C410
U 1 1 4C459B93
P 5750 3050
F 0 "C410" H 5750 3150 50  0000 L CNN
F 1 "100nF" H 5750 2950 50  0000 L CNN
F 2 "C0603F" H 5750 3050 60  0001 C CNN
F 3 "" H 5750 3050 60  0001 C CNN
	1    5750 3050
	1    0    0    -1  
$EndComp
$Comp
L C C408
U 1 1 4C459B6F
P 5500 3050
F 0 "C408" H 5500 3150 50  0000 L CNN
F 1 "100nF" H 5500 2950 50  0000 L CNN
F 2 "C0603F" H 5500 3050 60  0001 C CNN
F 3 "" H 5500 3050 60  0001 C CNN
	1    5500 3050
	1    0    0    -1  
$EndComp
$Comp
L C C405
U 1 1 4C459B6B
P 5000 3050
F 0 "C405" H 5000 3150 50  0000 L CNN
F 1 "4.7µF" H 5000 2950 50  0000 L CNN
F 2 "C0603F" H 5000 3050 60  0001 C CNN
F 3 "" H 5000 3050 60  0001 C CNN
	1    5000 3050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR095
U 1 1 4C4599C6
P 8500 3800
F 0 "#PWR095" H 8500 3800 30  0001 C CNN
F 1 "GND" H 8500 3730 30  0001 C CNN
F 2 "" H 8500 3800 60  0001 C CNN
F 3 "" H 8500 3800 60  0001 C CNN
	1    8500 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR096
U 1 1 4C4599C4
P 8750 3800
F 0 "#PWR096" H 8750 3800 30  0001 C CNN
F 1 "GND" H 8750 3730 30  0001 C CNN
F 2 "" H 8750 3800 60  0001 C CNN
F 3 "" H 8750 3800 60  0001 C CNN
	1    8750 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR097
U 1 1 4C4599C3
P 7400 3800
F 0 "#PWR097" H 7400 3800 30  0001 C CNN
F 1 "GND" H 7400 3730 30  0001 C CNN
F 2 "" H 7400 3800 60  0001 C CNN
F 3 "" H 7400 3800 60  0001 C CNN
	1    7400 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR098
U 1 1 4C4599C2
P 7150 3800
F 0 "#PWR098" H 7150 3800 30  0001 C CNN
F 1 "GND" H 7150 3730 30  0001 C CNN
F 2 "" H 7150 3800 60  0001 C CNN
F 3 "" H 7150 3800 60  0001 C CNN
	1    7150 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR099
U 1 1 4C4599C0
P 6850 3800
F 0 "#PWR099" H 6850 3800 30  0001 C CNN
F 1 "GND" H 6850 3730 30  0001 C CNN
F 2 "" H 6850 3800 60  0001 C CNN
F 3 "" H 6850 3800 60  0001 C CNN
	1    6850 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0100
U 1 1 4C4599BF
P 6600 3800
F 0 "#PWR0100" H 6600 3800 30  0001 C CNN
F 1 "GND" H 6600 3730 30  0001 C CNN
F 2 "" H 6600 3800 60  0001 C CNN
F 3 "" H 6600 3800 60  0001 C CNN
	1    6600 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0101
U 1 1 4C4599BD
P 4450 3800
F 0 "#PWR0101" H 4450 3800 30  0001 C CNN
F 1 "GND" H 4450 3730 30  0001 C CNN
F 2 "" H 4450 3800 60  0001 C CNN
F 3 "" H 4450 3800 60  0001 C CNN
	1    4450 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0102
U 1 1 4C4599B9
P 4200 3800
F 0 "#PWR0102" H 4200 3800 30  0001 C CNN
F 1 "GND" H 4200 3730 30  0001 C CNN
F 2 "" H 4200 3800 60  0001 C CNN
F 3 "" H 4200 3800 60  0001 C CNN
	1    4200 3800
	1    0    0    -1  
$EndComp
$Comp
L R R401
U 1 1 4C45997B
P 8500 3000
F 0 "R401" V 8400 3000 50  0000 C CNN
F 1 "1R0" V 8500 3000 50  0000 C CNN
F 2 "R0603F" H 8500 3000 60  0001 C CNN
F 3 "" H 8500 3000 60  0001 C CNN
	1    8500 3000
	-1   0    0    1   
$EndComp
$Comp
L C C423
U 1 1 4C459957
P 8750 3000
F 0 "C423" H 8750 3100 50  0000 L CNN
F 1 "100nF" H 8750 2900 50  0000 L CNN
F 2 "C0603F" H 8750 3000 60  0001 C CNN
F 3 "" H 8750 3000 60  0001 C CNN
	1    8750 3000
	1    0    0    -1  
$EndComp
$Comp
L C C418
U 1 1 4C459905
P 7400 3050
F 0 "C418" H 7400 3150 50  0000 L CNN
F 1 "100nF" H 7400 2950 50  0000 L CNN
F 2 "C0603F" H 7400 3050 60  0001 C CNN
F 3 "" H 7400 3050 60  0001 C CNN
	1    7400 3050
	1    0    0    -1  
$EndComp
$Comp
L C C416
U 1 1 4C459903
P 7150 3050
F 0 "C416" H 7150 3150 50  0000 L CNN
F 1 "100nF" H 7150 2950 50  0000 L CNN
F 2 "C0603F" H 7150 3050 60  0001 C CNN
F 3 "" H 7150 3050 60  0001 C CNN
	1    7150 3050
	1    0    0    -1  
$EndComp
$Comp
L C C413
U 1 1 4C45988B
P 6600 3050
F 0 "C413" H 6600 3150 50  0000 L CNN
F 1 "2.2µF" H 6600 2950 50  0000 L CNN
F 2 "C0603F" H 6600 3050 60  0001 C CNN
F 3 "" H 6600 3050 60  0001 C CNN
	1    6600 3050
	1    0    0    -1  
$EndComp
$Comp
L C C414
U 1 1 4C459886
P 6850 3050
F 0 "C414" H 6850 3150 50  0000 L CNN
F 1 "100nF" H 6850 2950 50  0000 L CNN
F 2 "C0603F" H 6850 3050 60  0001 C CNN
F 3 "" H 6850 3050 60  0001 C CNN
	1    6850 3050
	1    0    0    -1  
$EndComp
$Comp
L C C402
U 1 1 4C459853
P 4450 3050
F 0 "C402" H 4450 3150 50  0000 L CNN
F 1 "100nF" H 4450 2950 50  0000 L CNN
F 2 "C0603F" H 4450 3050 60  0001 C CNN
F 3 "" H 4450 3050 60  0001 C CNN
	1    4450 3050
	1    0    0    -1  
$EndComp
$Comp
L C C401
U 1 1 4C459842
P 4200 3050
F 0 "C401" H 4200 3150 50  0000 L CNN
F 1 "10µF/10V" V 4050 2850 50  0000 L CNN
F 2 "C0805" H 4200 3050 60  0001 C CNN
F 3 "" H 4200 3050 60  0001 C CNN
	1    4200 3050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0103
U 1 1 4C4596D6
P 3700 3800
F 0 "#PWR0103" H 3700 3800 30  0001 C CNN
F 1 "GND" H 3700 3730 30  0001 C CNN
F 2 "" H 3700 3800 60  0001 C CNN
F 3 "" H 3700 3800 60  0001 C CNN
	1    3700 3800
	1    0    0    -1  
$EndComp
$Comp
L 3V3 #PWR0104
U 1 1 4C4596BA
P 2950 1950
F 0 "#PWR0104" H 2950 2050 40  0001 C CNN
F 1 "3V3" H 2950 2075 40  0000 C CNN
F 2 "" H 2950 1950 60  0001 C CNN
F 3 "" H 2950 1950 60  0001 C CNN
	1    2950 1950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0105
U 1 1 4C459603
P 5400 6050
F 0 "#PWR0105" H 5400 6050 30  0001 C CNN
F 1 "GND" H 5400 5980 30  0001 C CNN
F 2 "" H 5400 6050 60  0001 C CNN
F 3 "" H 5400 6050 60  0001 C CNN
	1    5400 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0106
U 1 1 4C459418
P 7350 6050
F 0 "#PWR0106" H 7350 6050 30  0001 C CNN
F 1 "GND" H 7350 5980 30  0001 C CNN
F 2 "" H 7350 6050 60  0001 C CNN
F 3 "" H 7350 6050 60  0001 C CNN
	1    7350 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0107
U 1 1 4C459394
P 7100 6050
F 0 "#PWR0107" H 7100 6050 30  0001 C CNN
F 1 "GND" H 7100 5980 30  0001 C CNN
F 2 "" H 7100 6050 60  0001 C CNN
F 3 "" H 7100 6050 60  0001 C CNN
	1    7100 6050
	1    0    0    -1  
$EndComp
$Comp
L C C417
U 1 1 4C459335
P 7350 5000
F 0 "C417" H 7350 5100 50  0000 L CNN
F 1 "100nF" H 7350 4900 50  0000 L CNN
F 2 "C0603F" H 7350 5000 60  0001 C CNN
F 3 "" H 7350 5000 60  0001 C CNN
	1    7350 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0108
U 1 1 4C459276
P 5700 6050
F 0 "#PWR0108" H 5700 6050 30  0001 C CNN
F 1 "GND" H 5700 5980 30  0001 C CNN
F 2 "" H 5700 6050 60  0001 C CNN
F 3 "" H 5700 6050 60  0001 C CNN
	1    5700 6050
	1    0    0    -1  
$EndComp
$Comp
L C C409
U 1 1 4C45924A
P 5700 5150
F 0 "C409" H 5700 5250 50  0000 L CNN
F 1 "100nF" H 5700 5050 50  0000 L CNN
F 2 "C0603F" H 5700 5150 60  0001 C CNN
F 3 "" H 5700 5150 60  0001 C CNN
	1    5700 5150
	1    0    0    -1  
$EndComp
$Comp
L 3V3 #PWR0109
U 1 1 4C458FF8
P 7700 4650
F 0 "#PWR0109" H 7700 4750 40  0001 C CNN
F 1 "3V3" H 7700 4775 40  0000 C CNN
F 2 "" H 7700 4650 60  0001 C CNN
F 3 "" H 7700 4650 60  0001 C CNN
	1    7700 4650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR0110
U 1 1 4C458FD3
P 6350 6050
F 0 "#PWR0110" H 6350 6050 30  0001 C CNN
F 1 "GND" H 6350 5980 30  0001 C CNN
F 2 "" H 6350 6050 60  0001 C CNN
F 3 "" H 6350 6050 60  0001 C CNN
	1    6350 6050
	1    0    0    -1  
$EndComp
$Comp
L CP1 C403
U 1 1 4C458EF1
P 4500 5350
F 0 "C403" H 4500 5450 50  0000 L CNN
F 1 "100µF" H 4500 5250 50  0000 L CNN
F 2 "3528-21" H 4500 5350 60  0001 C CNN
F 3 "" H 4500 5350 60  0001 C CNN
	1    4500 5350
	1    0    0    -1  
$EndComp
$Comp
L C C422
U 1 1 4C458EBA
P 8500 3500
F 0 "C422" H 8500 3600 50  0000 L CNN
F 1 "4.7µF" H 8500 3400 50  0000 L CNN
F 2 "C0603F" H 8500 3500 60  0001 C CNN
F 3 "" H 8500 3500 60  0001 C CNN
	1    8500 3500
	1    0    0    -1  
$EndComp
$Comp
L C C424
U 1 1 4C458E8E
P 9150 3000
F 0 "C424" H 9150 3100 50  0000 L CNN
F 1 "100nF" H 9150 2900 50  0000 L CNN
F 2 "C0603F" H 9150 3000 60  0001 C CNN
F 3 "" H 9150 3000 60  0001 C CNN
	1    9150 3000
	1    0    0    -1  
$EndComp
$Comp
L C C415
U 1 1 4C458E5D
P 7100 5000
F 0 "C415" H 7100 5100 50  0000 L CNN
F 1 "10µF/10V" V 6950 4800 50  0000 L CNN
F 2 "C0805" H 7100 5000 60  0001 C CNN
F 3 "" H 7100 5000 60  0001 C CNN
	1    7100 5000
	1    0    0    -1  
$EndComp
$Comp
L INDUCTOR L401
U 1 1 4C458DDE
P 8200 2700
F 0 "L401" V 8150 2700 40  0000 C CNN
F 1 "LBR2012T100K" V 8300 2700 40  0000 C CNN
F 2 "R0805" H 8200 2700 60  0001 C CNN
F 3 "" H 8200 2700 60  0001 C CNN
	1    8200 2700
	0    -1   -1   0   
$EndComp
$Comp
L DIODESCH D404
U 1 1 512DD0FB
P 9200 4350
F 0 "D404" H 9200 4450 40  0000 C CNN
F 1 "B0520LW" H 9200 4250 40  0000 C CNN
F 2 "SOD-123" H 9200 4350 60  0001 C CNN
F 3 "" H 9200 4350 60  0001 C CNN
	1    9200 4350
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P402
U 1 1 543812C0
P 5000 4600
F 0 "P402" H 5080 4600 40  0000 L CNN
F 1 "TSTPT_5V" H 5000 4655 30  0001 C CNN
F 2 "TSTPT" H 5000 4600 60  0001 C CNN
F 3 "" H 5000 4600 60  0000 C CNN
	1    5000 4600
	0    -1   -1   0   
$EndComp
$Comp
L CONN_1 P403
U 1 1 54381714
P 8150 4600
F 0 "P403" H 8230 4600 40  0000 L CNN
F 1 "TSTPT_3V3" H 8150 4655 30  0001 C CNN
F 2 "TSTPT" H 8150 4600 60  0001 C CNN
F 3 "" H 8150 4600 60  0000 C CNN
	1    8150 4600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7700 5200 7700 6050
Wire Wire Line
	5950 4800 4500 4800
Connection ~ 5000 4800
Wire Wire Line
	5000 4700 5000 4950
Wire Wire Line
	4500 6050 4500 5550
Connection ~ 8150 4800
Wire Wire Line
	8150 6050 8150 5200
Wire Wire Line
	9900 6050 9900 5300
Connection ~ 8850 5000
Wire Wire Line
	8700 5000 8850 5000
Wire Wire Line
	8850 5000 8850 5200
Wire Wire Line
	9600 4350 9600 4900
Wire Wire Line
	3950 5100 4750 5100
Wire Wire Line
	6250 2700 2950 2700
Wire Wire Line
	5000 3800 5000 3250
Connection ~ 6000 2700
Wire Wire Line
	6250 2850 6250 2550
Wire Wire Line
	6000 3800 6000 3250
Wire Wire Line
	7900 3800 7900 3250
Wire Wire Line
	7900 2700 6850 2700
Wire Wire Line
	9150 2800 9150 2550
Connection ~ 7700 4800
Wire Wire Line
	4750 5100 4750 4650
Wire Wire Line
	8750 2700 8500 2700
Wire Wire Line
	8750 3800 8750 3200
Connection ~ 7650 2700
Wire Wire Line
	7650 2850 7650 2550
Wire Wire Line
	3700 2550 3700 3800
Connection ~ 5250 2700
Wire Wire Line
	5250 2850 5250 2550
Wire Wire Line
	5750 2850 5750 2550
Connection ~ 4200 2700
Wire Wire Line
	4200 2700 4200 2850
Wire Wire Line
	5250 3800 5250 3250
Wire Wire Line
	5750 3800 5750 3250
Wire Wire Line
	2950 2700 2950 1950
Wire Wire Line
	7150 3250 7150 3800
Wire Wire Line
	6600 3800 6600 3250
Wire Wire Line
	4200 3800 4200 3250
Connection ~ 6850 2700
Connection ~ 7150 2700
Wire Wire Line
	6600 2850 6850 2850
Connection ~ 5400 4800
Wire Wire Line
	5400 4800 5400 4950
Wire Wire Line
	5700 6050 5700 5350
Wire Wire Line
	7350 6050 7350 5200
Connection ~ 7100 4800
Connection ~ 5700 4800
Wire Wire Line
	5700 4950 5700 4800
Wire Wire Line
	3950 4800 4100 4800
Connection ~ 4750 4800
Connection ~ 7350 4800
Wire Wire Line
	7100 5200 7100 6050
Wire Wire Line
	6850 2850 6850 2550
Wire Wire Line
	7150 2550 7150 2850
Wire Wire Line
	8500 3250 8500 3300
Wire Wire Line
	4450 3250 4450 3800
Wire Wire Line
	6850 3250 6850 3800
Wire Wire Line
	7400 3250 7400 3800
Wire Wire Line
	5500 3800 5500 3250
Wire Wire Line
	4750 3800 4750 3250
Wire Wire Line
	4200 2850 4450 2850
Wire Wire Line
	4450 2850 4450 2550
Wire Wire Line
	5500 2850 5500 2550
Connection ~ 5500 2700
Connection ~ 5750 2700
Wire Wire Line
	4750 2700 4750 2850
Connection ~ 4750 2700
Connection ~ 3700 2550
Wire Wire Line
	7650 3800 7650 3250
Wire Wire Line
	7400 2850 7400 2550
Connection ~ 7400 2700
Wire Wire Line
	8500 3800 8500 3700
Wire Wire Line
	8500 2700 8500 2750
Wire Wire Line
	8750 2800 8750 2550
Connection ~ 8750 2700
Wire Wire Line
	9400 1900 9400 2700
Wire Wire Line
	9400 2700 9150 2700
Wire Wire Line
	3500 2550 3900 2550
Connection ~ 3800 2550
Connection ~ 3600 2550
Wire Wire Line
	9150 3800 9150 3200
Connection ~ 9150 2700
Wire Wire Line
	7900 2850 7900 2550
Connection ~ 7900 2700
Wire Wire Line
	6250 3800 6250 3250
Wire Wire Line
	6000 2850 6000 2550
Connection ~ 6250 2700
Connection ~ 6850 2850
Connection ~ 4200 2850
Connection ~ 4450 2850
Connection ~ 8500 2700
Wire Wire Line
	5000 2850 5000 2700
Connection ~ 5000 2700
Wire Wire Line
	7700 4800 7700 4650
Wire Wire Line
	9600 4800 9900 4800
Wire Wire Line
	8850 6050 8850 5700
Wire Wire Line
	9600 6050 9600 5000
Wire Wire Line
	9900 4800 9900 4900
Connection ~ 9900 4800
Connection ~ 9600 4800
Wire Wire Line
	6350 6050 6350 5200
Wire Wire Line
	6750 4800 8850 4800
Wire Wire Line
	5400 6050 5400 5350
Wire Wire Line
	4500 5150 4500 5100
Connection ~ 4500 5100
Wire Wire Line
	5000 5350 5000 6050
Wire Wire Line
	9600 4350 9400 4350
Wire Wire Line
	9000 4350 8600 4350
Wire Wire Line
	8600 4350 8600 4800
Connection ~ 8600 4800
Wire Wire Line
	8150 4800 8150 4700
$Comp
L CONN_1 P401
U 1 1 54381AEF
P 4150 5800
F 0 "P401" H 4230 5800 40  0000 L CNN
F 1 "TSTPT_GND" H 4150 5855 30  0001 C CNN
F 2 "TSTPT" H 4150 5800 60  0001 C CNN
F 3 "" H 4150 5800 60  0000 C CNN
	1    4150 5800
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR0111
U 1 1 54381D49
P 4150 6050
F 0 "#PWR0111" H 4150 6050 30  0001 C CNN
F 1 "GND" H 4150 5980 30  0001 C CNN
F 2 "" H 4150 6050 60  0001 C CNN
F 3 "" H 4150 6050 60  0001 C CNN
	1    4150 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 6050 4150 5900
$EndSCHEMATC
