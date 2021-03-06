EESchema Schematic File Version 2  date Mon Feb 28 02:20:46 2011
LIBS:conn
LIBS:device
EELAYER 24  0
EELAYER END
$Descr A4 11700 8267
Sheet 1 1
Title "ATUSB Programming Adapter"
Date "28 feb 2011"
Rev "20110228"
Comp "Werner Almesberger"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 5650 5350 0    70   ~ 0
For red, use LTST-C190KRKT (631 nm)
Text Notes 5650 5200 0    70   ~ 0
LTST-C190KFKT is yellow/orange (605 nm)
NoConn ~ 7500 4450
Wire Wire Line
	8150 4450 7500 4450
Connection ~ 5750 4600
Wire Wire Line
	5750 4600 5750 4750
Wire Wire Line
	3950 4300 5300 4300
Wire Wire Line
	3950 4150 8150 4150
Wire Wire Line
	3950 3850 8150 3850
Wire Wire Line
	8150 4750 7050 4750
Wire Wire Line
	3950 4450 5550 4450
Wire Wire Line
	5550 4450 5550 4300
Wire Wire Line
	5550 4300 8150 4300
Wire Wire Line
	8150 4600 3950 4600
Wire Wire Line
	5750 4750 5950 4750
Wire Wire Line
	6350 4750 6550 4750
Wire Wire Line
	8150 4000 3950 4000
Wire Wire Line
	8150 4900 5300 4900
Wire Wire Line
	5300 4900 5300 4300
Text Label 7700 4900 0    60   ~ 0
DAT1
Text Label 7700 4750 0    60   ~ 0
DAT0
Text Label 7700 4600 0    60   ~ 0
GND
Text Label 7700 4450 0    60   ~ 0
CLK
Text Label 7700 4300 0    60   ~ 0
VDD
Text Label 7700 4150 0    60   ~ 0
CMD
Text Label 7700 4000 0    60   ~ 0
DAT3
Text Label 7700 3850 0    60   ~ 0
DAT2
Text Label 4150 3850 0    60   ~ 0
SCK
Text Label 4150 4000 0    60   ~ 0
PDI
Text Label 4150 4150 0    60   ~ 0
PDO
Text Label 4150 4300 0    60   ~ 0
nRESET
Text Label 4150 4450 0    60   ~ 0
VDD
Text Label 4150 4600 0    60   ~ 0
GND
$Comp
L CONN_1 P7
U 1 1 4D4A41CE
P 8300 3850
F 0 "P7" H 8380 3850 40  0000 L CNN
F 1 "CONN_1" H 8300 3905 30  0001 C CNN
F 2 "PAD_2mm" H 8300 3850 60  0001 C CNN
	1    8300 3850
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P8
U 1 1 4D4A41CC
P 8300 4000
F 0 "P8" H 8380 4000 40  0000 L CNN
F 1 "CONN_1" H 8300 4055 30  0001 C CNN
F 2 "PAD_2mm" H 8300 4000 60  0001 C CNN
	1    8300 4000
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P9
U 1 1 4D4A4162
P 8300 4150
F 0 "P9" H 8380 4150 40  0000 L CNN
F 1 "CONN_1" H 8300 4205 30  0001 C CNN
F 2 "PAD_2mm" H 8300 4150 60  0001 C CNN
	1    8300 4150
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P10
U 1 1 4D4A4160
P 8300 4300
F 0 "P10" H 8380 4300 40  0000 L CNN
F 1 "CONN_1" H 8300 4355 30  0001 C CNN
F 2 "PAD_2mm" H 8300 4300 60  0001 C CNN
	1    8300 4300
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P11
U 1 1 4D4A415F
P 8300 4450
F 0 "P11" H 8380 4450 40  0000 L CNN
F 1 "CONN_1" H 8300 4505 30  0001 C CNN
F 2 "PAD_2mm" H 8300 4450 60  0001 C CNN
	1    8300 4450
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P14
U 1 1 4D4A415C
P 8300 4900
F 0 "P14" H 8380 4900 40  0000 L CNN
F 1 "CONN_1" H 8300 4955 30  0001 C CNN
F 2 "PAD_2mm" H 8300 4900 60  0001 C CNN
	1    8300 4900
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P13
U 1 1 4D4A4156
P 8300 4750
F 0 "P13" H 8380 4750 40  0000 L CNN
F 1 "CONN_1" H 8300 4805 30  0001 C CNN
F 2 "PAD_2mm" H 8300 4750 60  0001 C CNN
	1    8300 4750
	1    0    0    -1  
$EndComp
$Comp
L CONN_1 P6
U 1 1 4D4A4131
P 3800 4600
F 0 "P6" H 3880 4600 40  0000 L CNN
F 1 "CONN_1" H 3800 4655 30  0001 C CNN
F 2 "PAD_80x80" H 3800 4600 60  0001 C CNN
	1    3800 4600
	-1   0    0    1   
$EndComp
$Comp
L CONN_1 P5
U 1 1 4D4A412F
P 3800 4450
F 0 "P5" H 3880 4450 40  0000 L CNN
F 1 "CONN_1" H 3800 4505 30  0001 C CNN
F 2 "PAD_80x80" H 3800 4450 60  0001 C CNN
	1    3800 4450
	-1   0    0    1   
$EndComp
$Comp
L CONN_1 P4
U 1 1 4D4A412C
P 3800 4300
F 0 "P4" H 3880 4300 40  0000 L CNN
F 1 "CONN_1" H 3800 4355 30  0001 C CNN
F 2 "PAD_80x80" H 3800 4300 60  0001 C CNN
	1    3800 4300
	-1   0    0    1   
$EndComp
$Comp
L CONN_1 P3
U 1 1 4D4A4128
P 3800 4150
F 0 "P3" H 3880 4150 40  0000 L CNN
F 1 "CONN_1" H 3800 4205 30  0001 C CNN
F 2 "PAD_80x80" H 3800 4150 60  0001 C CNN
	1    3800 4150
	-1   0    0    1   
$EndComp
$Comp
L CONN_1 P2
U 1 1 4D4A4125
P 3800 4000
F 0 "P2" H 3880 4000 40  0000 L CNN
F 1 "CONN_1" H 3800 4055 30  0001 C CNN
F 2 "PAD_80x80" H 3800 4000 60  0001 C CNN
	1    3800 4000
	-1   0    0    1   
$EndComp
$Comp
L LED D1
U 1 1 4D4A40C6
P 6150 4750
F 0 "D1" H 6150 4850 50  0000 C CNN
F 1 "LTST-C190KFKT" H 6150 4650 50  0000 C CNN
F 2 "0603" H 6150 4750 60  0001 C CNN
	1    6150 4750
	-1   0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 4D4A40C2
P 6800 4750
F 0 "R1" V 6880 4750 50  0000 C CNN
F 1 "100" V 6800 4750 50  0000 C CNN
F 2 "0603" H 6800 4750 60  0001 C CNN
	1    6800 4750
	0    -1   -1   0   
$EndComp
$Comp
L CONN_1 P1
U 1 1 4D4A40B2
P 3800 3850
F 0 "P1" H 3880 3850 40  0000 L CNN
F 1 "CONN_1" H 3800 3905 30  0001 C CNN
F 2 "PAD_80x80" H 3800 3850 60  0001 C CNN
	1    3800 3850
	-1   0    0    1   
$EndComp
$Comp
L CONN_1 P12
U 1 1 4D4A409E
P 8300 4600
F 0 "P12" H 8380 4600 40  0000 L CNN
F 1 "CONN_1" H 8300 4655 30  0001 C CNN
F 2 "PAD_2mm" H 8300 4600 60  0001 C CNN
	1    8300 4600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
