EESchema Schematic File Version 4
LIBS:vtx-scanner-pcb-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:+5V #PWR02
U 1 1 5BFB321E
P 2850 3650
F 0 "#PWR02" H 2850 3500 50  0001 C CNN
F 1 "+5V" H 2865 3823 50  0000 C CNN
F 2 "" H 2850 3650 50  0001 C CNN
F 3 "" H 2850 3650 50  0001 C CNN
	1    2850 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 4250 2850 4250
Wire Wire Line
	2850 4250 2850 3650
$Comp
L power:GND #PWR03
U 1 1 5BFB337A
P 2850 4900
F 0 "#PWR03" H 2850 4650 50  0001 C CNN
F 1 "GND" H 2855 4727 50  0000 C CNN
F 2 "" H 2850 4900 50  0001 C CNN
F 3 "" H 2850 4900 50  0001 C CNN
	1    2850 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 3850 2750 3850
Wire Wire Line
	2750 3850 2750 4350
Wire Wire Line
	2750 4350 2700 4350
Wire Wire Line
	2750 4350 2750 4900
Wire Wire Line
	2750 4900 2850 4900
Connection ~ 2750 4350
Wire Wire Line
	1700 3850 1600 3850
Wire Wire Line
	1600 3850 1600 4050
Wire Wire Line
	1600 4050 1700 4050
NoConn ~ 1550 1950
$Comp
L power:+5V #PWR0104
U 1 1 5BFC5986
P 1550 1650
F 0 "#PWR0104" H 1550 1500 50  0001 C CNN
F 1 "+5V" H 1565 1823 50  0000 C CNN
F 2 "" H 1550 1650 50  0001 C CNN
F 3 "" H 1550 1650 50  0001 C CNN
	1    1550 1650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5BFC59E7
P 1350 1950
F 0 "#PWR0105" H 1350 1700 50  0001 C CNN
F 1 "GND" H 1355 1777 50  0000 C CNN
F 2 "" H 1350 1950 50  0001 C CNN
F 3 "" H 1350 1950 50  0001 C CNN
	1    1350 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 1750 1350 1750
$Comp
L power:+BATT #PWR0106
U 1 1 5BFC6275
P 1200 1600
F 0 "#PWR0106" H 1200 1450 50  0001 C CNN
F 1 "+BATT" H 1215 1773 50  0000 C CNN
F 2 "" H 1200 1600 50  0001 C CNN
F 3 "" H 1200 1600 50  0001 C CNN
	1    1200 1600
	1    0    0    -1  
$EndComp
$Comp
L power:-BATT #PWR0107
U 1 1 5BFC6313
P 1000 1600
F 0 "#PWR0107" H 1000 1450 50  0001 C CNN
F 1 "-BATT" H 1015 1773 50  0000 C CNN
F 2 "" H 1000 1600 50  0001 C CNN
F 3 "" H 1000 1600 50  0001 C CNN
	1    1000 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 1750 1000 1600
Wire Wire Line
	1000 1750 1350 1750
Connection ~ 1350 1750
Wire Wire Line
	1200 1600 1200 1850
Wire Wire Line
	1200 1850 1550 1850
Wire Wire Line
	1350 1750 1350 1950
$Comp
L power:-BATT #PWR0108
U 1 1 5BFC86F6
P 1450 950
F 0 "#PWR0108" H 1450 800 50  0001 C CNN
F 1 "-BATT" H 1465 1123 50  0000 C CNN
F 2 "" H 1450 950 50  0001 C CNN
F 3 "" H 1450 950 50  0001 C CNN
	1    1450 950 
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0104
U 1 1 5BFC875B
P 1450 950
F 0 "#FLG0104" H 1450 1025 50  0001 C CNN
F 1 "PWR_FLAG" H 1450 1123 50  0000 C CNN
F 2 "" H 1450 950 50  0001 C CNN
F 3 "~" H 1450 950 50  0001 C CNN
	1    1450 950 
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG0105
U 1 1 5BFC8791
P 1050 950
F 0 "#FLG0105" H 1050 1025 50  0001 C CNN
F 1 "PWR_FLAG" H 1050 1123 50  0000 C CNN
F 2 "" H 1050 950 50  0001 C CNN
F 3 "~" H 1050 950 50  0001 C CNN
	1    1050 950 
	-1   0    0    1   
$EndComp
$Comp
L power:+BATT #PWR0109
U 1 1 5BFC87CE
P 1050 950
F 0 "#PWR0109" H 1050 800 50  0001 C CNN
F 1 "+BATT" H 1065 1123 50  0000 C CNN
F 2 "" H 1050 950 50  0001 C CNN
F 3 "" H 1050 950 50  0001 C CNN
	1    1050 950 
	1    0    0    -1  
$EndComp
$Comp
L deets:oskj-dc-dc U3
U 1 1 5C66ED7D
P 2100 1800
F 0 "U3" H 2477 1801 50  0000 L CNN
F 1 "oskj-dc-dc" H 2477 1710 50  0000 L CNN
F 2 "deets:oskj-dc-dc" H 2000 1700 50  0001 C CNN
F 3 "" H 2100 1800 50  0001 C CNN
	1    2100 1800
	1    0    0    -1  
$EndComp
$Comp
L deets:RX5808 U1
U 1 1 5C66EB41
P 2200 4250
F 0 "U1" H 2200 4875 50  0000 C CNN
F 1 "RX5808" H 2200 4784 50  0000 C CNN
F 2 "deets:RX5808" H 2100 4150 50  0001 C CNN
F 3 "" H 2200 4250 50  0001 C CNN
	1    2200 4250
	1    0    0    -1  
$EndComp
$Comp
L deets:ESP32-WROVER-I A1
U 1 1 5C685CC2
P 5600 3600
F 0 "A1" H 5600 4967 50  0000 C CNN
F 1 "ESP32-WROVER-I" H 5600 4876 50  0000 C CNN
F 2 "deets:XCVR_ESP32-WROVER-I" H 5600 3600 50  0001 L BNN
F 3 "Module: combo; GPIO, I2C x2, I2S x2, SDIO, SPI x3, UART x3; U.FL" H 5600 3600 50  0001 L BNN
F 4 "Espressif Systems" H 5600 3600 50  0001 L BNN "Field4"
F 5 "Module Espressif Systems" H 5600 3600 50  0001 L BNN "Field5"
F 6 "Unavailable" H 5600 3600 50  0001 L BNN "Field6"
F 7 "None" H 5600 3600 50  0001 L BNN "Field7"
F 8 "ESP32-WROVER-I" H 5600 3600 50  0001 L BNN "Field8"
	1    5600 3600
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 5C685EBB
P 1300 6700
F 0 "J1" H 1406 7078 50  0000 C CNN
F 1 "Conn_01x06_Male" H 1406 6987 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 1300 6700 50  0001 C CNN
F 3 "~" H 1300 6700 50  0001 C CNN
	1    1300 6700
	1    0    0    -1  
$EndComp
Text Label 1500 7000 0    50   ~ 0
GND
NoConn ~ 1500 6900
Text Label 1500 6500 0    50   ~ 0
RX
Text Label 1500 6600 0    50   ~ 0
TX
Text Label 1500 6700 0    50   ~ 0
RTS
Text Label 1500 6800 0    50   ~ 0
DTR
$Comp
L Device:R R2
U 1 1 5C6870C8
P 2750 7000
F 0 "R2" V 2543 7000 50  0000 C CNN
F 1 "10K" V 2634 7000 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 2680 7000 50  0001 C CNN
F 3 "~" H 2750 7000 50  0001 C CNN
	1    2750 7000
	0    1    1    0   
$EndComp
$Comp
L Device:R R1
U 1 1 5C68714B
P 2750 6350
F 0 "R1" V 2543 6350 50  0000 C CNN
F 1 "10K" V 2634 6350 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 2680 6350 50  0001 C CNN
F 3 "~" H 2750 6350 50  0001 C CNN
	1    2750 6350
	0    1    1    0   
$EndComp
Text Label 2900 7000 0    50   ~ 0
RTS
Text Label 2900 6350 0    50   ~ 0
DTR
Wire Wire Line
	2300 6800 2300 6700
Wire Wire Line
	2300 6700 2900 6700
Wire Wire Line
	2900 6700 2900 6350
Wire Wire Line
	2900 7000 2900 6800
Wire Wire Line
	2900 6800 2550 6800
Text Label 2300 6150 2    50   ~ 0
#EN
Wire Wire Line
	2300 7200 2300 7250
Text Label 2300 7250 2    50   ~ 0
IO0
Text Label 5000 3100 2    50   ~ 0
IO0
Text Label 4950 1350 2    50   ~ 0
#EN
$Comp
L Device:R R3
U 1 1 5C688503
P 6350 3700
F 0 "R3" V 6143 3700 50  0000 C CNN
F 1 "470" V 6234 3700 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 6280 3700 50  0001 C CNN
F 3 "~" H 6350 3700 50  0001 C CNN
	1    6350 3700
	0    1    1    0   
$EndComp
Text Label 6500 3700 0    50   ~ 0
TX
Wire Wire Line
	6200 3800 6500 3800
Text Label 6500 3800 0    50   ~ 0
RX
Text Notes 750  6000 0    50   ~ 0
Taken from the Ai-Thinker ESP32 eval board. This circuitry allows for in-system flashing.
$Comp
L Regulator_Linear:AMS1117-3.3 U2
U 1 1 5C6891DA
P 1200 2550
F 0 "U2" H 1200 2792 50  0000 C CNN
F 1 "AMS1117-3.3" H 1200 2701 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 1200 2750 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 1300 2300 50  0001 C CNN
	1    1200 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR04
U 1 1 5C6893F5
P 800 2550
F 0 "#PWR04" H 800 2400 50  0001 C CNN
F 1 "+5V" H 815 2723 50  0000 C CNN
F 2 "" H 800 2550 50  0001 C CNN
F 3 "" H 800 2550 50  0001 C CNN
	1    800  2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	900  2550 800  2550
Wire Wire Line
	1500 2550 1600 2550
$Comp
L power:+3.3V #PWR07
U 1 1 5C689C97
P 1600 2550
F 0 "#PWR07" H 1600 2400 50  0001 C CNN
F 1 "+3.3V" H 1615 2723 50  0000 C CNN
F 2 "" H 1600 2550 50  0001 C CNN
F 3 "" H 1600 2550 50  0001 C CNN
	1    1600 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR010
U 1 1 5C689E50
P 6450 2500
F 0 "#PWR010" H 6450 2350 50  0001 C CNN
F 1 "+3.3V" H 6465 2673 50  0000 C CNN
F 2 "" H 6450 2500 50  0001 C CNN
F 3 "" H 6450 2500 50  0001 C CNN
	1    6450 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 2500 6250 2500
$Comp
L Device:C C2
U 1 1 5C68A2A2
P 6250 2650
F 0 "C2" H 6365 2696 50  0000 L CNN
F 1 "47uF" H 6365 2605 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_5x5.8" H 6288 2500 50  0001 C CNN
F 3 "~" H 6250 2650 50  0001 C CNN
	1    6250 2650
	1    0    0    -1  
$EndComp
Connection ~ 6250 2500
Wire Wire Line
	6250 2500 6450 2500
$Comp
L Device:C C3
U 1 1 5C68A315
P 6700 2650
F 0 "C3" H 6815 2696 50  0000 L CNN
F 1 "100nF" H 6815 2605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 6738 2500 50  0001 C CNN
F 3 "~" H 6700 2650 50  0001 C CNN
	1    6700 2650
	1    0    0    -1  
$EndComp
Connection ~ 6450 2500
Wire Wire Line
	6250 2800 6450 2800
$Comp
L power:GND #PWR011
U 1 1 5C68A7E6
P 6450 2800
F 0 "#PWR011" H 6450 2550 50  0001 C CNN
F 1 "GND" H 6455 2627 50  0000 C CNN
F 2 "" H 6450 2800 50  0001 C CNN
F 3 "" H 6450 2800 50  0001 C CNN
	1    6450 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 4600 6500 4600
$Comp
L power:GND #PWR012
U 1 1 5C68B168
P 6500 4600
F 0 "#PWR012" H 6500 4350 50  0001 C CNN
F 1 "GND" H 6505 4427 50  0000 C CNN
F 2 "" H 6500 4600 50  0001 C CNN
F 3 "" H 6500 4600 50  0001 C CNN
	1    6500 4600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5C68B3E0
P 1200 2850
F 0 "#PWR06" H 1200 2600 50  0001 C CNN
F 1 "GND" H 1205 2677 50  0000 C CNN
F 2 "" H 1200 2850 50  0001 C CNN
F 3 "" H 1200 2850 50  0001 C CNN
	1    1200 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5C68B574
P 800 2700
F 0 "C1" H 915 2746 50  0000 L CNN
F 1 "47uF" H 915 2655 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_5x5.8" H 838 2550 50  0001 C CNN
F 3 "~" H 800 2700 50  0001 C CNN
	1    800  2700
	1    0    0    -1  
$EndComp
Connection ~ 800  2550
$Comp
L power:GND #PWR05
U 1 1 5C68B607
P 800 2850
F 0 "#PWR05" H 800 2600 50  0001 C CNN
F 1 "GND" H 805 2677 50  0000 C CNN
F 2 "" H 800 2850 50  0001 C CNN
F 3 "" H 800 2850 50  0001 C CNN
	1    800  2850
	1    0    0    -1  
$EndComp
$Comp
L deets:AI-1027-TWT U4
U 1 1 5C68CC18
P 5050 6400
F 0 "U4" H 5081 6725 50  0000 C CNN
F 1 "AI-1027-TWT" H 5081 6634 50  0000 C CNN
F 2 "deets:AI-1027-TWT" H 5400 6400 50  0001 C CNN
F 3 "" H 5400 6400 50  0001 C CNN
	1    5050 6400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5C68CD56
P 5300 6900
F 0 "#PWR09" H 5300 6650 50  0001 C CNN
F 1 "GND" H 5305 6727 50  0000 C CNN
F 2 "" H 5300 6900 50  0001 C CNN
F 3 "" H 5300 6900 50  0001 C CNN
	1    5300 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 6050 5300 6300
$Comp
L power:+5V #PWR08
U 1 1 5C68D2F1
P 5300 6050
F 0 "#PWR08" H 5300 5900 50  0001 C CNN
F 1 "+5V" H 5315 6223 50  0000 C CNN
F 2 "" H 5300 6050 50  0001 C CNN
F 3 "" H 5300 6050 50  0001 C CNN
	1    5300 6050
	1    0    0    -1  
$EndComp
Text Label 5000 6700 2    50   ~ 0
BUZZER
Wire Wire Line
	2700 4150 3100 4150
Wire Wire Line
	2700 4450 3100 4450
Wire Wire Line
	2700 4550 3100 4550
Wire Wire Line
	2700 4650 3100 4650
Text Label 3100 4450 0    50   ~ 0
VRX_CLOCK
Text Label 3100 4550 0    50   ~ 0
VRX_CS
Text Label 3100 4650 0    50   ~ 0
VRX_DATA
Text Label 3100 4150 0    50   ~ 0
VRX_RSSI
Text Label 6200 4000 0    50   ~ 0
VRX_RSSI
Text Label 5000 3700 2    50   ~ 0
VRX_CS
Text Label 5000 3500 2    50   ~ 0
VRX_CLOCK
Text Label 5000 3600 2    50   ~ 0
VRX_DATA
Text Label 5000 4300 2    50   ~ 0
SSD1306_DATA
Text Label 5000 4000 2    50   ~ 0
SSD1306_CLK
Text Label 5000 4200 2    50   ~ 0
SSD1306_CS
Text Label 5000 3800 2    50   ~ 0
SSD1306_DC
Text Label 5000 3900 2    50   ~ 0
SSD1306_RST
NoConn ~ 5000 3400
NoConn ~ 5000 3200
Text Notes 4950 3400 2    50   ~ 0
strapping pin\n
Text Notes 4950 3250 2    50   ~ 0
strapping pin\n
NoConn ~ 5000 2800
$Comp
L Connector:Conn_01x08_Female J2
U 1 1 5C69E4E5
P 7900 4000
F 0 "J2" H 7927 3976 50  0000 L CNN
F 1 "Conn_01x08_Female" H 7927 3885 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Horizontal" H 7900 4000 50  0001 C CNN
F 3 "~" H 7900 4000 50  0001 C CNN
	1    7900 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 3700 7350 3700
$Comp
L power:GND #PWR014
U 1 1 5C69EBDF
P 7350 3700
F 0 "#PWR014" H 7350 3450 50  0001 C CNN
F 1 "GND" H 7355 3527 50  0000 C CNN
F 2 "" H 7350 3700 50  0001 C CNN
F 3 "" H 7350 3700 50  0001 C CNN
	1    7350 3700
	1    0    0    -1  
$EndComp
NoConn ~ 7700 3800
Wire Wire Line
	7700 3900 7550 3900
Wire Wire Line
	7550 3900 7550 3600
Wire Wire Line
	7550 3600 7350 3600
$Comp
L power:+3.3V #PWR013
U 1 1 5C6A5EDE
P 7350 3600
F 0 "#PWR013" H 7350 3450 50  0001 C CNN
F 1 "+3.3V" H 7365 3773 50  0000 C CNN
F 2 "" H 7350 3600 50  0001 C CNN
F 3 "" H 7350 3600 50  0001 C CNN
	1    7350 3600
	1    0    0    -1  
$EndComp
Text Label 7700 4000 2    50   ~ 0
SSD1306_CS
Text Label 7700 4100 2    50   ~ 0
SSD1306_RST
Text Label 7700 4200 2    50   ~ 0
SSD1306_DC
Text Label 7700 4300 2    50   ~ 0
SSD1306_CLK
Text Label 7700 4400 2    50   ~ 0
SSD1306_DATA
NoConn ~ 6200 3100
NoConn ~ 6200 3200
NoConn ~ 6200 3300
NoConn ~ 6200 3400
NoConn ~ 6200 3500
NoConn ~ 6200 3600
Wire Wire Line
	10400 3900 10400 4000
Wire Wire Line
	10400 4000 10500 4000
Connection ~ 10400 4000
Wire Wire Line
	10400 4000 10400 4100
Wire Wire Line
	10400 5850 10400 5950
Wire Wire Line
	10400 5950 10500 5950
Connection ~ 10400 5950
Wire Wire Line
	10400 5950 10400 6050
Wire Wire Line
	10400 2850 10400 2950
Wire Wire Line
	10400 2950 10500 2950
Connection ~ 10400 2950
Wire Wire Line
	10400 2950 10400 3050
Wire Wire Line
	10400 4800 10400 4900
Wire Wire Line
	10400 4900 10500 4900
Connection ~ 10400 4900
Wire Wire Line
	10400 4900 10400 5000
$Comp
L power:GND #PWR017
U 1 1 5C6B19F0
P 10000 4100
F 0 "#PWR017" H 10000 3850 50  0001 C CNN
F 1 "GND" H 10005 3927 50  0000 C CNN
F 2 "" H 10000 4100 50  0001 C CNN
F 3 "" H 10000 4100 50  0001 C CNN
	1    10000 4100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR016
U 1 1 5C6B1A4E
P 10000 3050
F 0 "#PWR016" H 10000 2800 50  0001 C CNN
F 1 "GND" H 10005 2877 50  0000 C CNN
F 2 "" H 10000 3050 50  0001 C CNN
F 3 "" H 10000 3050 50  0001 C CNN
	1    10000 3050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR018
U 1 1 5C6B1BD8
P 10000 5000
F 0 "#PWR018" H 10000 4750 50  0001 C CNN
F 1 "GND" H 10005 4827 50  0000 C CNN
F 2 "" H 10000 5000 50  0001 C CNN
F 3 "" H 10000 5000 50  0001 C CNN
	1    10000 5000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR015
U 1 1 5C6B1C4A
P 10000 6050
F 0 "#PWR015" H 10000 5800 50  0001 C CNN
F 1 "GND" H 10005 5877 50  0000 C CNN
F 2 "" H 10000 6050 50  0001 C CNN
F 3 "" H 10000 6050 50  0001 C CNN
	1    10000 6050
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 5C6B1FDF
P 10500 2800
F 0 "R5" H 10570 2846 50  0000 L CNN
F 1 "10K" H 10570 2755 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 10430 2800 50  0001 C CNN
F 3 "~" H 10500 2800 50  0001 C CNN
	1    10500 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R7
U 1 1 5C6B206C
P 10500 4750
F 0 "R7" H 10570 4796 50  0000 L CNN
F 1 "10K" H 10570 4705 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 10430 4750 50  0001 C CNN
F 3 "~" H 10500 4750 50  0001 C CNN
	1    10500 4750
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5C6B223C
P 10500 5800
F 0 "R4" H 10570 5846 50  0000 L CNN
F 1 "10K" H 10570 5755 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 10430 5800 50  0001 C CNN
F 3 "~" H 10500 5800 50  0001 C CNN
	1    10500 5800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 5C6B22D1
P 10500 3850
F 0 "R6" H 10570 3896 50  0000 L CNN
F 1 "10K" H 10570 3805 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 10430 3850 50  0001 C CNN
F 3 "~" H 10500 3850 50  0001 C CNN
	1    10500 3850
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR021
U 1 1 5C6B23F6
P 10500 3700
F 0 "#PWR021" H 10500 3550 50  0001 C CNN
F 1 "+3.3V" H 10515 3873 50  0000 C CNN
F 2 "" H 10500 3700 50  0001 C CNN
F 3 "" H 10500 3700 50  0001 C CNN
	1    10500 3700
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR020
U 1 1 5C6B24E2
P 10500 2650
F 0 "#PWR020" H 10500 2500 50  0001 C CNN
F 1 "+3.3V" H 10515 2823 50  0000 C CNN
F 2 "" H 10500 2650 50  0001 C CNN
F 3 "" H 10500 2650 50  0001 C CNN
	1    10500 2650
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR022
U 1 1 5C6B254C
P 10500 4600
F 0 "#PWR022" H 10500 4450 50  0001 C CNN
F 1 "+3.3V" H 10515 4773 50  0000 C CNN
F 2 "" H 10500 4600 50  0001 C CNN
F 3 "" H 10500 4600 50  0001 C CNN
	1    10500 4600
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR019
U 1 1 5C6B25B6
P 10500 5650
F 0 "#PWR019" H 10500 5500 50  0001 C CNN
F 1 "+3.3V" H 10515 5823 50  0000 C CNN
F 2 "" H 10500 5650 50  0001 C CNN
F 3 "" H 10500 5650 50  0001 C CNN
	1    10500 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	10500 5950 10500 6050
Connection ~ 10500 5950
Wire Wire Line
	10500 4900 10500 5000
Connection ~ 10500 4900
Wire Wire Line
	10500 2950 10500 3050
Connection ~ 10500 2950
Wire Wire Line
	10500 4000 10500 4100
Connection ~ 10500 4000
Text Label 10500 5000 0    50   ~ 0
BUTTON_RIGHT
Text Label 10500 6050 0    50   ~ 0
BUTTON_DOWN
Text Label 10500 3050 0    50   ~ 0
BUTTON_UP
Text Label 10500 4100 0    50   ~ 0
BUTTON_LEFT
NoConn ~ 5000 2700
Wire Wire Line
	6450 2500 6700 2500
Wire Wire Line
	6700 2800 6450 2800
Connection ~ 6450 2800
Wire Wire Line
	2550 6800 2550 6550
Wire Wire Line
	2550 6550 2300 6550
Text Label 6200 4100 0    50   ~ 0
BUTTON_UP
Text Label 6200 4200 0    50   ~ 0
BUTTON_LEFT
Text Label 6200 4300 0    50   ~ 0
BUTTON_RIGHT
Text Label 6200 4400 0    50   ~ 0
BUTTON_DOWN
NoConn ~ 6200 4700
Text Label 5000 4100 2    50   ~ 0
BUZZER
NoConn ~ 5000 4400
$Comp
L dk_Transistors-Bipolar-BJT-Single:MMSS8050-H-TP Q2
U 1 1 5C8D54F9
P 2400 7000
F 0 "Q2" H 2588 6947 60  0000 L CNN
F 1 "MMSS8050-H-TP" H 2588 7053 60  0000 L CNN
F 2 "digikey-footprints:SOT-23-3" H 2600 7200 60  0001 L CNN
F 3 "http://www.mccsemi.com/up_pdf/MMSS8050(SOT-23).pdf" H 2600 7300 60  0001 L CNN
F 4 "MMSS8050-H-TPMSCT-ND" H 2600 7400 60  0001 L CNN "Digi-Key_PN"
F 5 "MMSS8050-H-TP" H 2600 7500 60  0001 L CNN "MPN"
F 6 "Discrete Semiconductor Products" H 2600 7600 60  0001 L CNN "Category"
F 7 "Transistors - Bipolar (BJT) - Single" H 2600 7700 60  0001 L CNN "Family"
F 8 "http://www.mccsemi.com/up_pdf/MMSS8050(SOT-23).pdf" H 2600 7800 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/micro-commercial-co/MMSS8050-H-TP/MMSS8050-H-TPMSCT-ND/2825945" H 2600 7900 60  0001 L CNN "DK_Detail_Page"
F 10 "TRANS NPN 25V 1.5A SOT23" H 2600 8000 60  0001 L CNN "Description"
F 11 "Micro Commercial Co" H 2600 8100 60  0001 L CNN "Manufacturer"
F 12 "Active" H 2600 8200 60  0001 L CNN "Status"
	1    2400 7000
	-1   0    0    1   
$EndComp
$Comp
L dk_Transistors-Bipolar-BJT-Single:MMSS8050-H-TP Q1
U 1 1 5C8D5889
P 2400 6350
F 0 "Q1" H 2588 6403 60  0000 L CNN
F 1 "MMSS8050-H-TP" H 2588 6297 60  0000 L CNN
F 2 "digikey-footprints:SOT-23-3" H 2600 6550 60  0001 L CNN
F 3 "http://www.mccsemi.com/up_pdf/MMSS8050(SOT-23).pdf" H 2600 6650 60  0001 L CNN
F 4 "MMSS8050-H-TPMSCT-ND" H 2600 6750 60  0001 L CNN "Digi-Key_PN"
F 5 "MMSS8050-H-TP" H 2600 6850 60  0001 L CNN "MPN"
F 6 "Discrete Semiconductor Products" H 2600 6950 60  0001 L CNN "Category"
F 7 "Transistors - Bipolar (BJT) - Single" H 2600 7050 60  0001 L CNN "Family"
F 8 "http://www.mccsemi.com/up_pdf/MMSS8050(SOT-23).pdf" H 2600 7150 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/micro-commercial-co/MMSS8050-H-TP/MMSS8050-H-TPMSCT-ND/2825945" H 2600 7250 60  0001 L CNN "DK_Detail_Page"
F 10 "TRANS NPN 25V 1.5A SOT23" H 2600 7350 60  0001 L CNN "Description"
F 11 "Micro Commercial Co" H 2600 7450 60  0001 L CNN "Manufacturer"
F 12 "Active" H 2600 7550 60  0001 L CNN "Status"
	1    2400 6350
	-1   0    0    -1  
$EndComp
$Comp
L dk_Transistors-Bipolar-BJT-Single:MMSS8050-H-TP Q3
U 1 1 5C8D798E
P 5200 6700
F 0 "Q3" H 5388 6753 60  0000 L CNN
F 1 "MMSS8050-H-TP" H 5388 6647 60  0000 L CNN
F 2 "digikey-footprints:SOT-23-3" H 5400 6900 60  0001 L CNN
F 3 "http://www.mccsemi.com/up_pdf/MMSS8050(SOT-23).pdf" H 5400 7000 60  0001 L CNN
F 4 "MMSS8050-H-TPMSCT-ND" H 5400 7100 60  0001 L CNN "Digi-Key_PN"
F 5 "MMSS8050-H-TP" H 5400 7200 60  0001 L CNN "MPN"
F 6 "Discrete Semiconductor Products" H 5400 7300 60  0001 L CNN "Category"
F 7 "Transistors - Bipolar (BJT) - Single" H 5400 7400 60  0001 L CNN "Family"
F 8 "http://www.mccsemi.com/up_pdf/MMSS8050(SOT-23).pdf" H 5400 7500 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/micro-commercial-co/MMSS8050-H-TP/MMSS8050-H-TPMSCT-ND/2825945" H 5400 7600 60  0001 L CNN "DK_Detail_Page"
F 10 "TRANS NPN 25V 1.5A SOT23" H 5400 7700 60  0001 L CNN "Description"
F 11 "Micro Commercial Co" H 5400 7800 60  0001 L CNN "Manufacturer"
F 12 "Active" H 5400 7900 60  0001 L CNN "Status"
	1    5200 6700
	1    0    0    -1  
$EndComp
$Comp
L dk_Tactile-Switches:FSM4JSMATR S1
U 1 1 5C8D94E0
P 10200 5950
F 0 "S1" H 10200 6297 60  0000 C CNN
F 1 "FSM4JSMATR" H 10200 6191 60  0000 C CNN
F 2 "deets:Switch_Tactile_SMD_6x6mm" H 10400 6150 60  0001 L CNN
F 3 "http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=2-1437565-7&DocType=Customer+Drawing&DocLang=English" H 10400 6250 60  0001 L CNN
F 4 "450-1759-1-ND" H 10400 6350 60  0001 L CNN "Digi-Key_PN"
F 5 "FSM4JSMATR" H 10400 6450 60  0001 L CNN "MPN"
F 6 "Switches" H 10400 6550 60  0001 L CNN "Category"
F 7 "Tactile Switches" H 10400 6650 60  0001 L CNN "Family"
F 8 "http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=2-1437565-7&DocType=Customer+Drawing&DocLang=English" H 10400 6750 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/te-connectivity-alcoswitch-switches/FSM4JSMATR/450-1759-1-ND/2271638" H 10400 6850 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH TACTILE SPST-NO 0.05A 24V" H 10400 6950 60  0001 L CNN "Description"
F 11 "TE Connectivity ALCOSWITCH Switches" H 10400 7050 60  0001 L CNN "Manufacturer"
F 12 "Active" H 10400 7150 60  0001 L CNN "Status"
	1    10200 5950
	1    0    0    -1  
$EndComp
$Comp
L dk_Tactile-Switches:FSM4JSMATR S2
U 1 1 5C8D9681
P 10200 2950
F 0 "S2" H 10200 3297 60  0000 C CNN
F 1 "FSM4JSMATR" H 10200 3191 60  0000 C CNN
F 2 "deets:Switch_Tactile_SMD_6x6mm" H 10400 3150 60  0001 L CNN
F 3 "http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=2-1437565-7&DocType=Customer+Drawing&DocLang=English" H 10400 3250 60  0001 L CNN
F 4 "450-1759-1-ND" H 10400 3350 60  0001 L CNN "Digi-Key_PN"
F 5 "FSM4JSMATR" H 10400 3450 60  0001 L CNN "MPN"
F 6 "Switches" H 10400 3550 60  0001 L CNN "Category"
F 7 "Tactile Switches" H 10400 3650 60  0001 L CNN "Family"
F 8 "http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=2-1437565-7&DocType=Customer+Drawing&DocLang=English" H 10400 3750 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/te-connectivity-alcoswitch-switches/FSM4JSMATR/450-1759-1-ND/2271638" H 10400 3850 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH TACTILE SPST-NO 0.05A 24V" H 10400 3950 60  0001 L CNN "Description"
F 11 "TE Connectivity ALCOSWITCH Switches" H 10400 4050 60  0001 L CNN "Manufacturer"
F 12 "Active" H 10400 4150 60  0001 L CNN "Status"
	1    10200 2950
	1    0    0    -1  
$EndComp
$Comp
L dk_Tactile-Switches:FSM4JSMATR S3
U 1 1 5C8D978E
P 10200 4000
F 0 "S3" H 10200 4347 60  0000 C CNN
F 1 "FSM4JSMATR" H 10200 4241 60  0000 C CNN
F 2 "deets:Switch_Tactile_SMD_6x6mm" H 10400 4200 60  0001 L CNN
F 3 "http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=2-1437565-7&DocType=Customer+Drawing&DocLang=English" H 10400 4300 60  0001 L CNN
F 4 "450-1759-1-ND" H 10400 4400 60  0001 L CNN "Digi-Key_PN"
F 5 "FSM4JSMATR" H 10400 4500 60  0001 L CNN "MPN"
F 6 "Switches" H 10400 4600 60  0001 L CNN "Category"
F 7 "Tactile Switches" H 10400 4700 60  0001 L CNN "Family"
F 8 "http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=2-1437565-7&DocType=Customer+Drawing&DocLang=English" H 10400 4800 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/te-connectivity-alcoswitch-switches/FSM4JSMATR/450-1759-1-ND/2271638" H 10400 4900 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH TACTILE SPST-NO 0.05A 24V" H 10400 5000 60  0001 L CNN "Description"
F 11 "TE Connectivity ALCOSWITCH Switches" H 10400 5100 60  0001 L CNN "Manufacturer"
F 12 "Active" H 10400 5200 60  0001 L CNN "Status"
	1    10200 4000
	1    0    0    -1  
$EndComp
$Comp
L dk_Tactile-Switches:FSM4JSMATR S4
U 1 1 5C8D9896
P 10200 4900
F 0 "S4" H 10200 5247 60  0000 C CNN
F 1 "FSM4JSMATR" H 10200 5141 60  0000 C CNN
F 2 "deets:Switch_Tactile_SMD_6x6mm" H 10400 5100 60  0001 L CNN
F 3 "http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=2-1437565-7&DocType=Customer+Drawing&DocLang=English" H 10400 5200 60  0001 L CNN
F 4 "450-1759-1-ND" H 10400 5300 60  0001 L CNN "Digi-Key_PN"
F 5 "FSM4JSMATR" H 10400 5400 60  0001 L CNN "MPN"
F 6 "Switches" H 10400 5500 60  0001 L CNN "Category"
F 7 "Tactile Switches" H 10400 5600 60  0001 L CNN "Family"
F 8 "http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=2-1437565-7&DocType=Customer+Drawing&DocLang=English" H 10400 5700 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/te-connectivity-alcoswitch-switches/FSM4JSMATR/450-1759-1-ND/2271638" H 10400 5800 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH TACTILE SPST-NO 0.05A 24V" H 10400 5900 60  0001 L CNN "Description"
F 11 "TE Connectivity ALCOSWITCH Switches" H 10400 6000 60  0001 L CNN "Manufacturer"
F 12 "Active" H 10400 6100 60  0001 L CNN "Status"
	1    10200 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 2850 10000 3050
Connection ~ 10000 3050
Wire Wire Line
	10000 3900 10000 4100
Connection ~ 10000 4100
Wire Wire Line
	10000 4800 10000 5000
Connection ~ 10000 5000
Wire Wire Line
	10000 5850 10000 6050
Connection ~ 10000 6050
$Comp
L deets:SMA-J-P-H-RA-TH1 J3
U 1 1 5C8E0D7B
P 1000 3950
F 0 "J3" H 1105 4317 50  0000 C CNN
F 1 "SMA-J-P-H-RA-TH1" H 1105 4226 50  0000 C CNN
F 2 "deets:SAMTEC_SMA-J-P-H-RA-TH1" H 1000 3950 50  0001 L BNN
F 3 "SMA-J-P-H-RA-TH1" H 1000 3950 50  0001 L BNN
F 4 "None" H 1000 3950 50  0001 L BNN "Field4"
F 5 "Samtec" H 1000 3950 50  0001 L BNN "Field5"
F 6 "None" H 1000 3950 50  0001 L BNN "Field6"
F 7 "Conn SMA F 0Hz to 6GHz 50Ohm Solder RA Thru-Hole Gold Tray" H 1000 3950 50  0001 L BNN "Field7"
F 8 "Unavailable" H 1000 3950 50  0001 L BNN "Field8"
	1    1000 3950
	1    0    0    -1  
$EndComp
Connection ~ 1600 4050
$Comp
L power:GNDA #PWR0102
U 1 1 5C8E173F
P 1850 950
F 0 "#PWR0102" H 1850 700 50  0001 C CNN
F 1 "GNDA" H 1855 777 50  0000 C CNN
F 2 "" H 1850 950 50  0001 C CNN
F 3 "" H 1850 950 50  0001 C CNN
	1    1850 950 
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5C8E1957
P 1850 950
F 0 "#FLG0101" H 1850 1025 50  0001 C CNN
F 1 "PWR_FLAG" H 1850 1124 50  0000 C CNN
F 2 "" H 1850 950 50  0001 C CNN
F 3 "~" H 1850 950 50  0001 C CNN
	1    1850 950 
	1    0    0    -1  
$EndComp
Text Label 1400 3950 0    50   ~ 0
ANT
Wire Wire Line
	1700 3950 1400 3950
Wire Wire Line
	1400 4050 1600 4050
Text Label 5000 3300 2    50   ~ 0
EXP1
Text Label 6200 3900 0    50   ~ 0
EXP2
$Comp
L Connector_Generic:Conn_01x05 J4
U 1 1 5CB1EFA4
P 7600 5650
F 0 "J4" H 7680 5692 50  0000 L CNN
F 1 "Conn_01x05" H 7680 5601 50  0000 L CNN
F 2 "Connector_JST:JST_PH_S5B-PH-K_1x05_P2.00mm_Horizontal" H 7600 5650 50  0001 C CNN
F 3 "~" H 7600 5650 50  0001 C CNN
	1    7600 5650
	1    0    0    -1  
$EndComp
Text Label 7400 5650 2    50   ~ 0
EXP1
Text Label 7400 5750 2    50   ~ 0
EXP2
$Comp
L power:GND #PWR024
U 1 1 5CB1F134
P 7400 5850
F 0 "#PWR024" H 7400 5600 50  0001 C CNN
F 1 "GND" H 7405 5677 50  0000 C CNN
F 2 "" H 7400 5850 50  0001 C CNN
F 3 "" H 7400 5850 50  0001 C CNN
	1    7400 5850
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR023
U 1 1 5CB1F237
P 7400 5450
F 0 "#PWR023" H 7400 5300 50  0001 C CNN
F 1 "+5V" H 7415 5623 50  0000 C CNN
F 2 "" H 7400 5450 50  0001 C CNN
F 3 "" H 7400 5450 50  0001 C CNN
	1    7400 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 5550 7250 5550
Wire Wire Line
	7250 5550 7250 5450
$Comp
L power:+3.3V #PWR01
U 1 1 5CB204F9
P 7250 5450
F 0 "#PWR01" H 7250 5300 50  0001 C CNN
F 1 "+3.3V" H 7265 5623 50  0000 C CNN
F 2 "" H 7250 5450 50  0001 C CNN
F 3 "" H 7250 5450 50  0001 C CNN
	1    7250 5450
	1    0    0    -1  
$EndComp
$Comp
L deets:XT60-M J5
U 1 1 5CB22143
P 3300 1400
F 0 "J5" H 3194 1033 50  0000 C CNN
F 1 "XT60-M" H 3194 1124 50  0000 C CNN
F 2 "deets:AMASS_XT60-M-SIDE-MOUNT" H 3300 1400 50  0001 L BNN
F 3 "AMASS" H 3300 1400 50  0001 L BNN
F 4 "None" H 3300 1400 50  0001 L BNN "Field4"
F 5 "Unavailable" H 3300 1400 50  0001 L BNN "Field6"
F 6 "Package Analog Devices" H 3300 1400 50  0001 L BNN "Field7"
F 7 "XT60-M" H 3300 1400 50  0001 L BNN "Field8"
	1    3300 1400
	1    0    0    1   
$EndComp
$Comp
L power:+BATT #PWR025
U 1 1 5CB23453
P 3000 1300
F 0 "#PWR025" H 3000 1150 50  0001 C CNN
F 1 "+BATT" H 3015 1473 50  0000 C CNN
F 2 "" H 3000 1300 50  0001 C CNN
F 3 "" H 3000 1300 50  0001 C CNN
	1    3000 1300
	1    0    0    -1  
$EndComp
$Comp
L power:-BATT #PWR026
U 1 1 5CB23505
P 3000 1500
F 0 "#PWR026" H 3000 1350 50  0001 C CNN
F 1 "-BATT" H 3015 1673 50  0000 C CNN
F 2 "" H 3000 1500 50  0001 C CNN
F 3 "" H 3000 1500 50  0001 C CNN
	1    3000 1500
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR027
U 1 1 5CB24161
P 1600 4050
F 0 "#PWR027" H 1600 3800 50  0001 C CNN
F 1 "GND" H 1605 3877 50  0000 C CNN
F 2 "" H 1600 4050 50  0001 C CNN
F 3 "" H 1600 4050 50  0001 C CNN
	1    1600 4050
	1    0    0    -1  
$EndComp
$Comp
L dk_Barrel-Audio-Connectors:SJ-3523-SMT-TR CON1
U 1 1 5CBA07DB
P 3600 3700
F 0 "CON1" H 3272 3647 60  0000 R CNN
F 1 "SJ-3523-SMT-TR" H 3272 3753 60  0000 R CNN
F 2 "deets:Switch_Slide_11.6x4mm_SJ-3523-SMT" H 3800 3900 60  0001 L CNN
F 3 "https://www.cui.com/product/resource/digikeypdf/sj-352x-smt_series.pdf" H 3800 4000 60  0001 L CNN
F 4 "CP-3523SJCT-ND" H 3800 4100 60  0001 L CNN "Digi-Key_PN"
F 5 "SJ-3523-SMT-TR" H 3800 4200 60  0001 L CNN "MPN"
F 6 "Connectors, Interconnects" H 3800 4300 60  0001 L CNN "Category"
F 7 "Barrel - Audio Connectors" H 3800 4400 60  0001 L CNN "Family"
F 8 "https://www.cui.com/product/resource/digikeypdf/sj-352x-smt_series.pdf" H 3800 4500 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/cui-inc/SJ-3523-SMT-TR/CP-3523SJCT-ND/669704" H 3800 4600 60  0001 L CNN "DK_Detail_Page"
F 10 "CONN JACK STEREO 3.5MM SMD R/A" H 3800 4700 60  0001 L CNN "Description"
F 11 "CUI Inc." H 3800 4800 60  0001 L CNN "Manufacturer"
F 12 "Active" H 3800 4900 60  0001 L CNN "Status"
	1    3600 3700
	-1   0    0    1   
$EndComp
Wire Wire Line
	2750 3850 3000 3850
Wire Wire Line
	3000 3850 3000 3600
Wire Wire Line
	3000 3600 3200 3600
Connection ~ 2750 3850
Wire Wire Line
	2700 3950 3100 3950
Wire Wire Line
	3100 3950 3100 3700
Wire Wire Line
	3100 3700 3200 3700
Wire Wire Line
	2700 4050 3200 4050
Wire Wire Line
	3200 4050 3200 3800
Wire Wire Line
	4950 1350 4550 1350
Wire Wire Line
	4550 1350 4550 1200
$Comp
L Device:R R8
U 1 1 5D0E4C72
P 4550 1050
F 0 "R8" H 4620 1096 50  0000 L CNN
F 1 "10K" H 4620 1005 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 4480 1050 50  0001 C CNN
F 3 "~" H 4550 1050 50  0001 C CNN
	1    4550 1050
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0101
U 1 1 5D0E5144
P 4550 900
F 0 "#PWR0101" H 4550 750 50  0001 C CNN
F 1 "+3.3V" H 4565 1073 50  0000 C CNN
F 2 "" H 4550 900 50  0001 C CNN
F 3 "" H 4550 900 50  0001 C CNN
	1    4550 900 
	1    0    0    -1  
$EndComp
Text Label 5000 2900 2    50   ~ 0
#EN
$Comp
L Device:C C4
U 1 1 5D0E76C3
P 4550 1500
F 0 "C4" H 4665 1546 50  0000 L CNN
F 1 "300nF" H 4665 1455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4588 1350 50  0001 C CNN
F 3 "~" H 4550 1500 50  0001 C CNN
	1    4550 1500
	1    0    0    -1  
$EndComp
Connection ~ 4550 1350
$Comp
L power:GND #PWR0103
U 1 1 5D0E7B34
P 4550 1650
F 0 "#PWR0103" H 4550 1400 50  0001 C CNN
F 1 "GND" H 4555 1477 50  0000 C CNN
F 2 "" H 4550 1650 50  0001 C CNN
F 3 "" H 4550 1650 50  0001 C CNN
	1    4550 1650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R9
U 1 1 5D0E975F
P 5400 1050
F 0 "R9" H 5470 1096 50  0000 L CNN
F 1 "10K" H 5470 1005 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 5330 1050 50  0001 C CNN
F 3 "~" H 5400 1050 50  0001 C CNN
	1    5400 1050
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0111
U 1 1 5D0E9878
P 5400 900
F 0 "#PWR0111" H 5400 750 50  0001 C CNN
F 1 "+3.3V" H 5415 1073 50  0000 C CNN
F 2 "" H 5400 900 50  0001 C CNN
F 3 "" H 5400 900 50  0001 C CNN
	1    5400 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 1200 5850 1200
Text Label 5850 1200 2    50   ~ 0
IO0
$EndSCHEMATC
