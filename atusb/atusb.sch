EESchema Schematic File Version 2  date Fri Jan 28 03:26:56 2011
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
Sheet 1 3
Title "IEEE 802.15.4 USB Transceiver (AT86RF231)"
Date "28 jan 2011"
Rev "20110123"
Comp "Werner Almesberger"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	4350 5050 7350 5050
Wire Wire Line
	10000 6100 10000 6300
Wire Wire Line
	7350 4700 4350 4700
Wire Wire Line
	7350 4300 4350 4300
Wire Wire Line
	7350 4000 4350 4000
Wire Wire Line
	7350 3850 4350 3850
Wire Wire Line
	7350 4150 4350 4150
Wire Wire Line
	7350 4500 4350 4500
Wire Wire Line
	7350 4850 4350 4850
$Sheet
S 2300 3400 2050 2050
U 4C609BEF
F0 "USB" 60
F1 "usb.sch" 60
F2 "nRST_RF" O R 4350 4700 60 
F3 "SLP_TR" O R 4350 4850 60 
F4 "IRQ_RF" I R 4350 4500 60 
F5 "nSS" O R 4350 4300 60 
F6 "SCLK" O R 4350 4150 60 
F7 "MOSI" O R 4350 3850 60 
F8 "MISO" I R 4350 4000 60 
F9 "CLK" I R 4350 5050 60 
$EndSheet
$Comp
L PWR_FLAG #FLG01
U 1 1 4C641B9C
P 10000 6100
F 0 "#FLG01" H 10000 6370 30  0001 C CNN
F 1 "PWR_FLAG" H 10000 6330 30  0000 C CNN
	1    10000 6100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 4C641B91
P 10000 6300
F 0 "#PWR02" H 10000 6300 30  0001 C CNN
F 1 "GND" H 10000 6230 30  0001 C CNN
	1    10000 6300
	1    0    0    -1  
$EndComp
Text Notes 900  1250 0    200  ~ 40
IEEE 802.15.4 USB TXRX
$Sheet
S 7350 3450 1800 1950
U 4C609C08
F0 "RF" 60
F1 "atrf.sch" 60
F2 "SLP_TR" I L 7350 4850 60 
F3 "nRST_RF" I L 7350 4700 60 
F4 "SCLK" I L 7350 4150 60 
F5 "MISO" O L 7350 4000 60 
F6 "MOSI" I L 7350 3850 60 
F7 "nSS" I L 7350 4300 60 
F8 "IRQ_RF" O L 7350 4500 60 
F9 "CLK" O L 7350 5050 60 
$EndSheet
$EndSCHEMATC
