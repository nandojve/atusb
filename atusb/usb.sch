EESchema Schematic File Version 2  date Thu May 19 14:21:05 2011
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
Sheet 2 3
Title "IEEE 802.15.4 USB Transceiver (AT86RF231)"
Date "19 may 2011"
Rev "20110519"
Comp "Werner Almesberger"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	10400 2250 7800 2250
Wire Wire Line
	7800 2250 7800 2600
Connection ~ 4000 4200
Wire Wire Line
	4000 5350 4000 4650
Wire Wire Line
	4000 4650 4000 4200
Wire Wire Line
	4000 4200 4000 4000
Wire Wire Line
	10400 6450 7500 6450
Wire Wire Line
	7500 6450 7500 5800
Wire Wire Line
	10400 6150 8400 6150
Wire Wire Line
	8400 6150 8400 5800
Wire Wire Line
	8550 2600 8550 1350
Wire Wire Line
	9950 1350 9950 3750
Wire Wire Line
	9950 3750 9550 3750
Wire Wire Line
	7200 1350 7200 2050
Wire Wire Line
	7950 5800 7950 6000
Wire Wire Line
	9550 4500 10400 4500
Wire Wire Line
	9550 3300 9750 3300
Wire Wire Line
	6150 4200 6350 4200
Connection ~ 4000 4650
Wire Wire Line
	6350 4650 5050 4650
Wire Wire Line
	5050 4650 4000 4650
Wire Wire Line
	5150 4350 6350 4350
Wire Wire Line
	4650 4350 4450 4350
Wire Wire Line
	2350 4650 2550 4650
Wire Wire Line
	5350 2400 5550 2400
Wire Wire Line
	5700 3900 5700 4050
Wire Wire Line
	5700 4050 5700 4800
Wire Wire Line
	5700 4800 5700 5800
Wire Wire Line
	4450 4350 4300 4500
Wire Wire Line
	4300 4500 3000 4500
Wire Wire Line
	3000 4500 2350 4500
Wire Wire Line
	4300 4350 4450 4500
Wire Wire Line
	3500 5850 3500 6050
Wire Wire Line
	4000 4200 2350 4200
Connection ~ 3000 4500
Wire Wire Line
	3000 5350 3000 4500
Wire Wire Line
	5050 6200 5050 6400
Connection ~ 5700 4800
Wire Wire Line
	5700 4800 6350 4800
Wire Wire Line
	4300 4350 3500 4350
Wire Wire Line
	3500 4350 2350 4350
Wire Wire Line
	2550 4650 2550 4850
Wire Wire Line
	1000 4850 1000 4650
Wire Wire Line
	1000 4650 1000 4200
Connection ~ 1000 4650
Wire Wire Line
	1200 4650 1000 4650
Wire Wire Line
	1000 4200 1200 4200
Wire Wire Line
	5700 6200 5700 6400
Wire Wire Line
	3500 5350 3500 4350
Connection ~ 3500 4350
Wire Wire Line
	3000 5850 3000 6050
Wire Wire Line
	4000 5850 4000 6050
Wire Wire Line
	6150 2400 5950 2400
Wire Wire Line
	4450 4500 4650 4500
Wire Wire Line
	5150 4500 6350 4500
Wire Wire Line
	5050 5800 5050 4650
Connection ~ 5050 4650
Wire Wire Line
	6350 4050 5700 4050
Connection ~ 5700 4050
Wire Wire Line
	7800 5800 7800 6000
Wire Wire Line
	9550 4800 10400 4800
Wire Wire Line
	7500 1350 7500 2600
Wire Wire Line
	7350 1350 7350 2050
Wire Wire Line
	9550 3900 10100 3900
Wire Wire Line
	10100 3900 10100 1350
Wire Wire Line
	8550 5800 8550 6000
Wire Wire Line
	8550 6000 10400 6000
Wire Wire Line
	8250 5800 8250 6300
Wire Wire Line
	8250 6300 10400 6300
Wire Wire Line
	8400 2600 8400 2400
Wire Wire Line
	8400 2400 10400 2400
Wire Wire Line
	8100 2600 8100 2400
Wire Wire Line
	8100 2400 6650 2400
Text Label 6850 2400 0    60   ~ 0
LED
NoConn ~ 8100 5800
NoConn ~ 7950 2600
NoConn ~ 8250 2600
NoConn ~ 9550 4200
NoConn ~ 9550 4350
NoConn ~ 6350 3750
Text Label 5350 4500 0    60   ~ 0
DN
Text Label 5350 4350 0    60   ~ 0
DP
Text Label 2550 4200 0    60   ~ 0
VBUS
$Comp
L PWR_FLAG #FLG03
U 1 1 4D426152
P 4000 4000
F 0 "#FLG03" H 4000 4270 30  0001 C CNN
F 1 "PWR_FLAG" H 4000 4230 30  0000 C CNN
	1    4000 4000
	1    0    0    -1  
$EndComp
Text Label 7500 1850 1    60   ~ 0
nRESET
Text Label 10100 1850 1    60   ~ 0
SCK
Text HLabel 10400 6450 2    60   Input ~ 0
CLK
NoConn ~ 7650 5800
NoConn ~ 7650 2600
NoConn ~ 9550 4050
NoConn ~ 9550 4650
$Comp
L CONN_1 P16
U 1 1 4D425D62
P 10100 1200
F 0 "P16" H 10180 1200 40  0000 L CNN
F 1 "CONN_1" H 10100 1255 30  0001 C CNN
F 2 "PAD_60x60" H 10100 1200 60  0001 C CNN
	1    10100 1200
	0    -1   -1   0   
$EndComp
$Comp
L CONN_1 P15
U 1 1 4D425D53
P 9950 1200
F 0 "P15" H 10030 1200 40  0000 L CNN
F 1 "CONN_1" H 9950 1255 30  0001 C CNN
F 2 "PAD_60x60" H 9950 1200 60  0001 C CNN
	1    9950 1200
	0    -1   -1   0   
$EndComp
$Comp
L VDD #PWR04
U 1 1 4D425CE8
P 7350 2050
F 0 "#PWR04" H 7350 2150 30  0001 C CNN
F 1 "VDD" H 7350 2160 30  0000 C CNN
	1    7350 2050
	-1   0    0    1   
$EndComp
NoConn ~ 6350 3900
$Comp
L VDD #PWR05
U 1 1 4D425C10
P 7950 6000
F 0 "#PWR05" H 7950 6100 30  0001 C CNN
F 1 "VDD" H 7950 6110 30  0000 C CNN
	1    7950 6000
	-1   0    0    1   
$EndComp
$Comp
L R R5
U 1 1 4D4258DC
P 4900 4500
F 0 "R5" V 4980 4500 50  0000 C CNN
F 1 "22" V 4900 4500 50  0000 C CNN
F 2 "0402" H 4900 4500 60  0001 C CNN
	1    4900 4500
	0    1    1    0   
$EndComp
$Comp
L R R4
U 1 1 4D4258D9
P 4900 4350
F 0 "R4" V 4980 4350 50  0000 C CNN
F 1 "22" V 4900 4350 50  0000 C CNN
F 2 "0402" H 4900 4350 60  0001 C CNN
	1    4900 4350
	0    -1   -1   0   
$EndComp
$Comp
L ATMEGA8U2 U1
U 1 1 4D425860
P 7950 4200
F 0 "U1" V 9300 5450 60  0000 C CNN
F 1 "ATMEGA32U2" V 7650 4600 60  0000 C CNN
F 2 "QFN32-VHHD-2" H 7950 4200 60  0001 C CNN
	1    7950 4200
	0    -1   -1   0   
$EndComp
Text Notes 2800 6300 0    60   ~ 0
USB 2.0 limits the capacitative load\non full-speed drivers to 50 pF.
$Comp
L GND #PWR06
U 1 1 4CF859A9
P 9750 3300
F 0 "#PWR06" H 9750 3300 30  0001 C CNN
F 1 "GND" H 9750 3230 30  0001 C CNN
	1    9750 3300
	0    -1   -1   0   
$EndComp
$Comp
L USB_A_PLUG~ CON1
U 1 1 4CF4AD1B
P 1750 4450
F 0 "CON1" H 1950 4900 60  0000 C CNN
F 1 "USB_A_PLUG" H 1750 4050 60  0000 C CNN
	1    1750 4450
	-1   0    0    -1  
$EndComp
Text Notes 7150 800  0    60   ~ 0
Space P11 through P16 at 100 mil interval close to board edge\nto allow use with a 0.1" header soldered to the board.
Text Label 2550 4500 0    60   ~ 0
D+
Text Label 2550 4350 0    60   ~ 0
D-
Text Notes 1100 2500 0    200  ~ 40
USB
$Comp
L VDD #PWR07
U 1 1 4C64122B
P 5700 3900
F 0 "#PWR07" H 5700 4000 30  0001 C CNN
F 1 "VDD" H 5700 4010 30  0000 C CNN
	1    5700 3900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 4C6408C7
P 5700 6400
F 0 "#PWR08" H 5700 6400 30  0001 C CNN
F 1 "GND" H 5700 6330 30  0001 C CNN
	1    5700 6400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 4C6408C5
P 5050 6400
F 0 "#PWR09" H 5050 6400 30  0001 C CNN
F 1 "GND" H 5050 6330 30  0001 C CNN
	1    5050 6400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR010
U 1 1 4C6408BA
P 6150 4200
F 0 "#PWR010" H 6150 4200 30  0001 C CNN
F 1 "GND" H 6150 4130 30  0001 C CNN
	1    6150 4200
	0    1    1    0   
$EndComp
$Comp
L GND #PWR011
U 1 1 4C6408AD
P 5350 2400
F 0 "#PWR011" H 5350 2400 30  0001 C CNN
F 1 "GND" H 5350 2330 30  0001 C CNN
	1    5350 2400
	0    1    -1   0   
$EndComp
$Comp
L GND #PWR012
U 1 1 4C6408A8
P 7800 6000
F 0 "#PWR012" H 7800 6000 30  0001 C CNN
F 1 "GND" H 7800 5930 30  0001 C CNN
	1    7800 6000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 4C6408A3
P 7200 2050
F 0 "#PWR013" H 7200 2050 30  0001 C CNN
F 1 "GND" H 7200 1980 30  0001 C CNN
	1    7200 2050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR014
U 1 1 4C64089F
P 2550 4850
F 0 "#PWR014" H 2550 4850 30  0001 C CNN
F 1 "GND" H 2550 4780 30  0001 C CNN
	1    2550 4850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 4C640899
P 1000 4850
F 0 "#PWR015" H 1000 4850 30  0001 C CNN
F 1 "GND" H 1000 4780 30  0001 C CNN
	1    1000 4850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 4C640896
P 4000 6050
F 0 "#PWR016" H 4000 6050 30  0001 C CNN
F 1 "GND" H 4000 5980 30  0001 C CNN
	1    4000 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR017
U 1 1 4C64088F
P 3500 6050
F 0 "#PWR017" H 3500 6050 30  0001 C CNN
F 1 "GND" H 3500 5980 30  0001 C CNN
	1    3500 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 4C64088A
P 3000 6050
F 0 "#PWR018" H 3000 6050 30  0001 C CNN
F 1 "GND" H 3000 5980 30  0001 C CNN
	1    3000 6050
	1    0    0    -1  
$EndComp
Text Label 7200 1850 1    60   ~ 0
GND
Text Label 8550 1850 1    60   ~ 0
PDO
Text Label 7350 1850 1    60   ~ 0
VDD
Text Label 9950 1850 1    60   ~ 0
PDI
Text HLabel 10400 2250 2    60   Output ~ 0
nRST_RF
Text HLabel 10400 2400 2    60   Output ~ 0
SLP_TR
$Comp
L VR VR3
U 1 1 4C64034D
P 4000 5600
F 0 "VR3" V 4080 5600 50  0000 C CNN
F 1 "5.5Vdc" H 3800 5650 50  0000 C CNN
F 2 "0402" H 4000 5600 60  0001 C CNN
F 4 "33pF" H 3800 5550 50  0000 C CNN "Field4"
	1    4000 5600
	-1   0    0    -1  
$EndComp
$Comp
L VR VR2
U 1 1 4C640343
P 3500 5600
F 0 "VR2" V 3580 5600 50  0000 C CNN
F 1 "5.5Vdc" H 3300 5650 50  0000 C CNN
F 2 "0402" H 3500 5600 60  0001 C CNN
F 4 "33pF" H 3300 5550 50  0000 C CNN "Field4"
	1    3500 5600
	-1   0    0    -1  
$EndComp
$Comp
L VR VR1
U 1 1 4C6402FB
P 3000 5600
F 0 "VR1" V 3080 5600 50  0000 C CNN
F 1 "5.5Vdc" H 2800 5650 50  0000 C CNN
F 2 "0402" H 3000 5600 60  0001 C CNN
F 4 "33pF" H 2800 5550 50  0000 C CNN "Field4"
	1    3000 5600
	-1   0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 4C6402F2
P 6400 2400
F 0 "R1" V 6480 2400 50  0000 C CNN
F 1 "180" V 6400 2400 50  0000 C CNN
F 2 "0402" H 6400 2400 60  0001 C CNN
	1    6400 2400
	0    -1   -1   0   
$EndComp
$Comp
L LED D1
U 1 1 4C6402EE
P 5750 2400
F 0 "D1" H 5750 2500 50  0000 C CNN
F 1 "LTST-C190KRKT" H 5750 2300 50  0000 C CNN
F 2 "0603" H 5750 2400 60  0001 C CNN
	1    5750 2400
	-1   0    0    -1  
$EndComp
Text HLabel 10400 6300 2    60   Input ~ 0
IRQ_RF
Text HLabel 10400 6150 2    60   Output ~ 0
nSS
Text HLabel 10400 4500 2    60   Output ~ 0
SCLK
Text HLabel 10400 4800 2    60   Output ~ 0
MOSI
Text HLabel 10400 6000 2    60   Input ~ 0
MISO
$Comp
L CONN_1 P14
U 1 1 4C640203
P 8550 1200
F 0 "P14" H 8630 1200 40  0000 L CNN
F 1 "CONN_1" H 8550 1255 30  0001 C CNN
F 2 "PAD_60x60" H 8550 1200 60  0001 C CNN
	1    8550 1200
	0    -1   -1   0   
$EndComp
$Comp
L CONN_1 P13
U 1 1 4C640202
P 7500 1200
F 0 "P13" H 7580 1200 40  0000 L CNN
F 1 "CONN_1" H 7500 1255 30  0001 C CNN
F 2 "PAD_60x60" H 7500 1200 60  0001 C CNN
	1    7500 1200
	0    -1   -1   0   
$EndComp
$Comp
L CONN_1 P12
U 1 1 4C640200
P 7350 1200
F 0 "P12" H 7430 1200 40  0000 L CNN
F 1 "CONN_1" H 7350 1255 30  0001 C CNN
F 2 "PAD_60x60" H 7350 1200 60  0001 C CNN
	1    7350 1200
	0    -1   -1   0   
$EndComp
$Comp
L CONN_1 P11
U 1 1 4C6401FE
P 7200 1200
F 0 "P11" H 7280 1200 40  0000 L CNN
F 1 "CONN_1" H 7200 1255 30  0001 C CNN
F 2 "PAD_60x60" H 7200 1200 60  0001 C CNN
	1    7200 1200
	0    -1   -1   0   
$EndComp
$Comp
L C C2
U 1 1 4C6401B3
P 5700 6000
F 0 "C2" H 5750 6100 50  0000 L CNN
F 1 "1uF" H 5750 5900 50  0000 L CNN
F 2 "0402" H 5700 6000 60  0001 C CNN
	1    5700 6000
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 4C6401AA
P 5050 6000
F 0 "C1" H 5100 6100 50  0000 L CNN
F 1 "10uF" H 5100 5900 50  0000 L CNN
F 2 "0603" H 5050 6000 60  0001 C CNN
	1    5050 6000
	1    0    0    -1  
$EndComp
$EndSCHEMATC
