EESchema Schematic File Version 2  date Mon Feb 14 01:54:28 2011
LIBS:power
LIBS:device
LIBS:conn
LIBS:at86rf231
LIBS:atmega8u2
LIBS:usb_a_plug
LIBS:xtal-4
LIBS:antenna
LIBS:balun-smt6
LIBS:atusb-cache
EELAYER 24  0
EELAYER END
$Descr A4 11700 8267
Sheet 3 3
Title "IEEE 802.15.4 USB Transceiver (AT86RF230)"
Date "14 feb 2011"
Rev "20110214"
Comp "Werner Almesberger"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 10200 5750
Wire Wire Line
	10400 5750 10200 5750
Wire Wire Line
	10200 5550 10200 5950
Wire Wire Line
	2500 4400 2500 4600
Connection ~ 6200 3750
Wire Wire Line
	6200 3750 6700 3750
Wire Wire Line
	6700 4500 6500 4500
Wire Wire Line
	6500 4500 6500 4850
Wire Wire Line
	3150 5000 3150 4200
Wire Wire Line
	4350 5000 4350 4800
Wire Wire Line
	4150 4000 4350 4000
Wire Wire Line
	4950 4200 5150 4200
Wire Wire Line
	5150 4200 5150 4050
Wire Wire Line
	5150 4050 6700 4050
Wire Wire Line
	4950 3800 5350 3800
Wire Wire Line
	5350 3800 5350 4200
Wire Wire Line
	5350 4200 6700 4200
Wire Wire Line
	1400 3800 1800 3800
Connection ~ 3400 6150
Wire Wire Line
	3400 6350 3400 6150
Wire Wire Line
	1600 4600 1600 3900
Connection ~ 8400 6150
Wire Wire Line
	8600 6150 8400 6150
Wire Wire Line
	7600 2050 7600 1850
Wire Wire Line
	8100 2650 8100 1250
Wire Wire Line
	8250 2650 8250 2450
Wire Wire Line
	7950 2650 7950 2450
Connection ~ 9700 2450
Wire Wire Line
	9900 2450 8550 2450
Wire Wire Line
	8550 2450 8550 2650
Connection ~ 10500 1950
Wire Wire Line
	10500 1950 10100 1950
Wire Wire Line
	10500 2650 10500 1150
Wire Wire Line
	10500 1150 10300 1150
Wire Wire Line
	9700 1350 9700 1150
Connection ~ 7500 2450
Wire Wire Line
	6200 2450 7800 2450
Wire Wire Line
	6200 2450 6200 4850
Connection ~ 6200 4350
Wire Wire Line
	6700 4350 6200 4350
Wire Wire Line
	8100 6950 8100 6750
Wire Wire Line
	7500 5450 7500 5850
Connection ~ 9700 4500
Wire Wire Line
	9700 4850 9700 4050
Wire Wire Line
	9500 4500 9700 4500
Wire Wire Line
	10400 3600 9500 3600
Wire Wire Line
	10400 3900 9500 3900
Wire Wire Line
	10400 4350 9500 4350
Wire Wire Line
	8550 5850 8550 5450
Wire Wire Line
	8400 6350 8400 5450
Wire Wire Line
	8100 6350 8100 5450
Wire Wire Line
	8250 5450 8250 5650
Wire Wire Line
	8250 5650 8100 5650
Connection ~ 8100 5650
Wire Wire Line
	6000 4650 6700 4650
Wire Wire Line
	9500 4200 10400 4200
Wire Wire Line
	9500 3750 10400 3750
Wire Wire Line
	9700 4050 9500 4050
Wire Wire Line
	7950 5850 7950 5450
Wire Wire Line
	7650 5450 7650 5650
Wire Wire Line
	7650 5650 7500 5650
Connection ~ 7500 5650
Wire Wire Line
	8400 6750 8400 6950
Wire Wire Line
	9000 5850 9000 5450
Wire Wire Line
	6200 3900 6700 3900
Connection ~ 6200 3900
Wire Wire Line
	7800 2450 7800 2650
Wire Wire Line
	7500 2450 7500 2650
Wire Wire Line
	7650 2650 7650 2450
Connection ~ 7650 2450
Wire Wire Line
	9700 2250 9700 2450
Wire Wire Line
	10500 2450 10300 2450
Connection ~ 10500 2450
Wire Wire Line
	10100 1650 10500 1650
Connection ~ 10500 1650
Wire Wire Line
	8400 2650 8400 1150
Wire Wire Line
	8400 1150 9900 1150
Connection ~ 9700 1150
Wire Wire Line
	7950 2050 7950 1850
Wire Wire Line
	7800 6350 7800 5450
Wire Wire Line
	7600 1450 7600 1250
Wire Wire Line
	7600 1250 8100 1250
Wire Wire Line
	7850 1050 7850 1250
Connection ~ 7850 1250
Wire Wire Line
	1600 3900 1400 3900
Wire Wire Line
	3150 6350 3150 6150
Wire Wire Line
	3150 6150 3650 6150
Wire Wire Line
	3650 6150 3650 6350
Wire Wire Line
	2300 3800 3350 3800
Wire Wire Line
	4150 3800 4550 3800
Wire Wire Line
	4150 4200 4550 4200
Wire Wire Line
	3150 4200 3350 4200
Wire Wire Line
	4350 3600 4350 4400
Connection ~ 4350 4000
Wire Wire Line
	3350 4300 3150 4300
Connection ~ 3150 4300
Wire Wire Line
	6200 3600 6700 3600
Connection ~ 6200 3600
Wire Wire Line
	2500 4000 2500 3800
Connection ~ 2500 3800
Wire Wire Line
	9500 4650 10200 4650
Wire Wire Line
	10200 4650 10200 5050
Wire Wire Line
	10200 6350 10200 6550
Text HLabel 10400 5750 2    60   Output ~ 0
CLK
$Comp
L DGND #PWR38
U 1 1 4D426035
P 10200 6550
F 0 "#PWR38" H 10200 6550 40  0001 C CNN
F 1 "DGND" H 10200 6480 40  0000 C CNN
	1    10200 6550
	1    0    0    -1  
$EndComp
$Comp
L C C17
U 1 1 4D425FF1
P 10200 6150
F 0 "C17" H 10250 6250 50  0000 L CNN
F 1 "2.2pF" H 10250 6050 50  0000 L CNN
F 2 "0402" H 10200 6150 60  0001 C CNN
	1    10200 6150
	-1   0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 4D425FEE
P 10200 5300
F 0 "R6" V 10280 5300 50  0000 C CNN
F 1 "180" V 10200 5300 50  0000 C CNN
F 2 "0402" H 10200 5300 60  0001 C CNN
	1    10200 5300
	-1   0    0    -1  
$EndComp
$Comp
L AGND #PWR019
U 1 1 4D3B019B
P 2500 4600
F 0 "#PWR019" H 2500 4600 40  0001 C CNN
F 1 "AGND" H 2500 4530 50  0000 C CNN
	1    2500 4600
	1    0    0    -1  
$EndComp
$Comp
L C C16
U 1 1 4D3B018B
P 2500 4200
F 0 "C16" H 2550 4300 50  0000 L CNN
F 1 "NC" H 2550 4100 50  0000 L CNN
F 2 "0402" H 2500 4200 60  0001 C CNN
	1    2500 4200
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR25
U 1 1 4D2296C2
P 6500 4850
F 0 "#PWR25" H 6500 4850 40  0001 C CNN
F 1 "DGND" H 6500 4780 40  0000 C CNN
	1    6500 4850
	1    0    0    -1  
$EndComp
$Comp
L AT86RF231 U2
U 1 1 4D229690
P 8100 4050
F 0 "U2" H 7100 5250 60  0000 C CNN
F 1 "AT86RF231" H 8100 4050 60  0000 C CNN
F 2 "QFN32-VHHD-6" H 8100 4050 60  0001 C CNN
	1    8100 4050
	1    0    0    -1  
$EndComp
Text Label 2850 3800 0    60   ~ 0
FEED
$Comp
L PWR_FLAG #FLG020
U 1 1 4CF4B348
P 4350 3600
F 0 "#FLG020" H 4350 3870 30  0001 C CNN
F 1 "PWR_FLAG" H 4350 3830 30  0000 C CNN
	1    4350 3600
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR021
U 1 1 4CF4B229
P 3150 5000
F 0 "#PWR021" H 3150 5000 40  0001 C CNN
F 1 "AGND" H 3150 4930 50  0000 C CNN
	1    3150 5000
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR022
U 1 1 4CF4B224
P 4350 5000
F 0 "#PWR022" H 4350 5000 40  0001 C CNN
F 1 "AGND" H 4350 4930 50  0000 C CNN
	1    4350 5000
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 4CF4B07E
P 2050 3800
F 0 "R3" V 2130 3800 50  0000 C CNN
F 1 "0R" V 2050 3800 50  0000 C CNN
F 2 "0402" H 2050 3800 60  0001 C CNN
	1    2050 3800
	0    -1   -1   0   
$EndComp
$Comp
L BALUN-SMT6 B1
U 1 1 4CF4B034
P 3750 4000
F 0 "B1" H 3450 4350 60  0000 C CNN
F 1 "2450FB15L0001" H 3750 3550 60  0000 C CNN
F 2 "0805-6" H 3750 4000 60  0001 C CNN
	1    3750 4000
	1    0    0    -1  
$EndComp
Text Label 9950 4650 0    60   ~ 0
CLKM
Text Label 5750 4200 0    60   ~ 0
RFN
Text Label 5750 4050 0    60   ~ 0
RPF
$Comp
L GND #PWR023
U 1 1 4C641B5A
P 3150 6350
F 0 "#PWR023" H 3150 6350 30  0001 C CNN
F 1 "GND" H 3150 6280 30  0001 C CNN
	1    3150 6350
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR21
U 1 1 4C641B55
P 3400 6350
F 0 "#PWR21" H 3400 6350 40  0001 C CNN
F 1 "DGND" H 3400 6280 40  0000 C CNN
	1    3400 6350
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR024
U 1 1 4C641B53
P 3650 6350
F 0 "#PWR024" H 3650 6350 40  0001 C CNN
F 1 "AGND" H 3650 6280 50  0000 C CNN
	1    3650 6350
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR025
U 1 1 4C641731
P 1600 4600
F 0 "#PWR025" H 1600 4600 40  0001 C CNN
F 1 "AGND" H 1600 4530 50  0000 C CNN
	1    1600 4600
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 4C641710
P 4350 4600
F 0 "C3" H 4400 4700 50  0000 L CNN
F 1 "22pF/RF" H 4400 4500 50  0000 L CNN
F 2 "0402" H 4350 4600 60  0001 C CNN
	1    4350 4600
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 4C641509
P 4750 4200
F 0 "C9" H 4800 4300 50  0000 L CNN
F 1 "22pF/RF" H 4800 4100 50  0000 L CNN
F 2 "0402" H 4750 4200 60  0001 C CNN
	1    4750 4200
	0    -1   -1   0   
$EndComp
$Comp
L C C8
U 1 1 4C641506
P 4750 3800
F 0 "C8" H 4800 3900 50  0000 L CNN
F 1 "22pF/RF" H 4800 3700 50  0000 L CNN
F 2 "0402" H 4750 3800 60  0001 C CNN
	1    4750 3800
	0    -1   -1   0   
$EndComp
Text Notes 1300 1400 0    200  ~ 40
RF
Text HLabel 7800 6350 3    60   Input ~ 0
SLP_TR
Text HLabel 6000 4650 0    60   Input ~ 0
nRST_RF
Text HLabel 10400 4350 2    60   Input ~ 0
SCLK
Text HLabel 10400 4200 2    60   Output ~ 0
MISO
Text HLabel 10400 3900 2    60   Input ~ 0
MOSI
Text HLabel 10400 3750 2    60   Input ~ 0
nSS
Text HLabel 10400 3600 2    60   Output ~ 0
IRQ_RF
$Comp
L VDD #PWR026
U 1 1 4C641205
P 8600 6150
F 0 "#PWR026" H 8600 6250 30  0001 C CNN
F 1 "VDD" H 8600 6260 30  0000 C CNN
	1    8600 6150
	0    1    1    0   
$EndComp
$Comp
L VDD #PWR027
U 1 1 4C6411DB
P 7850 1050
F 0 "#PWR027" H 7850 1150 30  0001 C CNN
F 1 "VDD" H 7850 1160 30  0000 C CNN
	1    7850 1050
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR028
U 1 1 4C641100
P 7600 2050
F 0 "#PWR028" H 7600 2050 40  0001 C CNN
F 1 "AGND" H 7600 1980 50  0000 C CNN
	1    7600 2050
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR029
U 1 1 4C64109A
P 8250 2450
F 0 "#PWR029" H 8250 2450 40  0001 C CNN
F 1 "AGND" H 8250 2380 50  0000 C CNN
	1    8250 2450
	-1   0    0    1   
$EndComp
$Comp
L DGND #PWR29
U 1 1 4C641010
P 7950 1850
F 0 "#PWR29" H 7950 1850 40  0001 C CNN
F 1 "DGND" H 7950 1780 40  0000 C CNN
	1    7950 1850
	-1   0    0    1   
$EndComp
$Comp
L C C11
U 1 1 4C641004
P 7950 2250
F 0 "C11" H 8000 2350 50  0000 L CNN
F 1 "1uF" H 8000 2150 50  0000 L CNN
F 2 "0402" H 7950 2250 60  0001 C CNN
	1    7950 2250
	-1   0    0    -1  
$EndComp
$Comp
L DGND #PWR39
U 1 1 4C640E02
P 10500 2650
F 0 "#PWR39" H 10500 2650 40  0001 C CNN
F 1 "DGND" H 10500 2580 40  0000 C CNN
	1    10500 2650
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR030
U 1 1 4C640D33
P 6200 4850
F 0 "#PWR030" H 6200 4850 40  0001 C CNN
F 1 "AGND" H 6200 4780 50  0000 C CNN
	1    6200 4850
	1    0    0    -1  
$EndComp
$Comp
L AGND #PWR031
U 1 1 4C640CBA
P 9000 5850
F 0 "#PWR031" H 9000 5850 40  0001 C CNN
F 1 "AGND" H 9000 5780 50  0000 C CNN
	1    9000 5850
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR31
U 1 1 4C640C4C
P 8100 6950
F 0 "#PWR31" H 8100 6950 40  0001 C CNN
F 1 "DGND" H 8100 6880 40  0000 C CNN
	1    8100 6950
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR33
U 1 1 4C640C48
P 8400 6950
F 0 "#PWR33" H 8400 6950 40  0001 C CNN
F 1 "DGND" H 8400 6880 40  0000 C CNN
	1    8400 6950
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR26
U 1 1 4C640C14
P 7500 5850
F 0 "#PWR26" H 7500 5850 40  0001 C CNN
F 1 "DGND" H 7500 5780 40  0000 C CNN
	1    7500 5850
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR37
U 1 1 4C640BFC
P 9700 4850
F 0 "#PWR37" H 9700 4850 40  0001 C CNN
F 1 "DGND" H 9700 4780 40  0000 C CNN
	1    9700 4850
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR30
U 1 1 4C640BF7
P 7950 5850
F 0 "#PWR30" H 7950 5850 40  0001 C CNN
F 1 "DGND" H 7950 5780 40  0000 C CNN
	1    7950 5850
	1    0    0    -1  
$EndComp
$Comp
L DGND #PWR34
U 1 1 4C640BD2
P 8550 5850
F 0 "#PWR34" H 8550 5850 40  0001 C CNN
F 1 "DGND" H 8550 5780 40  0000 C CNN
	1    8550 5850
	1    0    0    -1  
$EndComp
$Comp
L C C12
U 1 1 4C640A84
P 8100 6550
F 0 "C12" H 8150 6650 50  0000 L CNN
F 1 "1uF" H 8150 6450 50  0000 L CNN
F 2 "0402" H 8100 6550 60  0001 C CNN
	1    8100 6550
	1    0    0    -1  
$EndComp
$Comp
L C C13
U 1 1 4C640A7E
P 8400 6550
F 0 "C13" H 8450 6650 50  0000 L CNN
F 1 "1uF" H 8450 6450 50  0000 L CNN
F 2 "0402" H 8400 6550 60  0001 C CNN
	1    8400 6550
	1    0    0    -1  
$EndComp
$Comp
L C C15
U 1 1 4C640A7B
P 10100 2450
F 0 "C15" H 10150 2550 50  0000 L CNN
F 1 "12pF" H 10150 2350 50  0000 L CNN
F 2 "0402" H 10100 2450 60  0001 C CNN
	1    10100 2450
	0    -1   1    0   
$EndComp
$Comp
L C C10
U 1 1 4C640A76
P 7600 1650
F 0 "C10" H 7650 1750 50  0000 L CNN
F 1 "1uF" H 7650 1550 50  0000 L CNN
F 2 "0402" H 7600 1650 60  0001 C CNN
	1    7600 1650
	-1   0    0    -1  
$EndComp
$Comp
L C C14
U 1 1 4C640A73
P 10100 1150
F 0 "C14" H 10150 1250 50  0000 L CNN
F 1 "12pF" H 10150 1050 50  0000 L CNN
F 2 "0402" H 10100 1150 60  0001 C CNN
	1    10100 1150
	0    -1   1    0   
$EndComp
$Comp
L ANTENNA ANT1
U 1 1 4C63FE17
P 1100 3850
F 0 "ANT1" H 1100 4100 60  0000 C CNN
F 1 "50R" H 1100 3600 60  0000 C CNN
F 2 "meander" H 1100 3850 60  0001 C CNN
	1    1100 3850
	1    0    0    -1  
$EndComp
$Comp
L XTAL-4 X1
U 1 1 4C63FA9F
P 9700 1800
F 0 "X1" V 9950 2400 60  0000 C CNN
F 1 "16MHz" V 9800 2300 60  0000 C CNN
F 2 "xtal4-3.2mmx2.5mm" H 9700 1800 60  0001 C CNN
F 4 "8pF" V 9650 2350 60  0000 C CNN "Field1"
F 5 "40ppm" V 9550 2300 60  0000 C CNN "Field2"
F 6 "ESR=80R" V 9450 2250 60  0000 C CNN "Field3"
	1    9700 1800
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
