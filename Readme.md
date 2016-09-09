# Pre-Requisites for Building subg_rfspy

[sdcc] (http://sdcc.sourceforge.net/) package is required for this build.

    sudo apt-get install sdcc

It's also possible to build it under Windows. The best way to do this is to:

1. install SDCC
2. install Cygwin
3. use Cygwin to install make
4. Use cygwin bash to build the project

# Building

You'll need to select a build type, like `uart0_alt1`.  The examples below use this value, but you'll want to use the correct one for your hardware and use case.  See below for different hardware types and use cases.

Perform the build. The output file will be stored at output/uart0_alt1_RILEYLINK_US/uart0_alt1_RILEYLINK_US.hex

    make -f Makefile.uart0_alt1

Perform the install:

    make -f Makefile.uart0_alt1 install
    
# Radio Frequency Selection

This code defaults to building firmware that is tuned to 916.5 Mhz. You can also build a 'WorldWide' firmware. This changes the default frequency to 868 and tweaks a few other settings.

    make -f Makefile.uart1_alt2 RADIO_LOCALE=WW

# RileyLink

If you are using a RileyLink via the onboard bluetooth module (which should be loaded with ble_rfspy), then you'll want to use `spi1_alt2` as your build type.

If you are going to manually wire up to the cc1110 on the RileyLink via the debug header, use `uart1_alt2` as your build type, and use the pinout below.

UART/SPI pins exposed on cc1110 debug header:

    PIN - SPI_alt2 / UART1_alt2
    P1.4 - CT / SSN
    P1.5 - RT / SCK
    P1.6 - TX / MOSI
    P1.7 - RX / MISO

# cc111x UART1_alt2 connected to the Intel Edison UART_1

    P1.4 - CT / SSN -> UART_1_RTS (GP129) (pin 63)
    P1.5 - RT / SCK -> UART_1_CTS (GP128) (pin 65)
    P1.6 - TX / MOSI -> UART_1_RX (GP130) (pin 61)
    P1.7 - RX / MISO -> UART_1_TX (GP131) (pin 46)


# UART on the WirelessThings ERF stick

Perform the build. The output file will be stored at output/uart0_alt1_SRF_ERF_US/uart0_alt1_SRF_ERF_US.hex

    make -f Makefile.uart0_alt1 BOARD_TYPE=SRF_ERF

Installation is bit more complicated, as you will need to attach connectors to
the SRF pins manually (you can't use the wiring points on the board itself).

This [XRF blog post](http://paulswasteland.blogspot.co.uk/2015/01/building-your-own-firmware-for-ciseco.html)
maps cc-debugger connector to the XRF pin names.

However, the XRF pins are not the same as the SRF pin locations. For that, you
can use the [SRF OpenMicros Data](http://openmicros.org/index.php/articles/88-ciseco-product-documentation/259-srf-technical-data)
to map things to the correct ERF locations.

- SRF Pin 5 - DDATA (also known as DD)
- SRF Pin 6 - DCLOCK (also known as DC)
- SRF Pin 9 - 3.3v (also known as VDD)
- SRF Pin 10 - Ground (also known as GND)
- SRF Pin 15 - Reset

To install the firmware:

    make -f Makefile.uart0_alt1 BOARD_TYPE=SRF_ERF install

# USB on support on TI cc1111 USB stick (CC1111EMK868-915) AKA "Don's Dongle"

    make -f Makefile.usb_ep0 install

Shows up as a serial device on linux.


# CCTL Support

If you have [CCTL](https://github.com/oskarpearson/cctl/tree/24mhz_clock_and_erf_stick_hack)
on your device stick, you can re-program the firmware without requiring the cc-debugger.
To compile firmware that's compatible with CCTL, set the CODE_LOC and CODE_LOC_NAME parameters:

    make -f Makefile.uart0_alt1 BOARD_TYPE=SRF_ERF CODE_LOC=0x400 CODE_LOC_NAME=CCTL

Then, compile the cctl writer program:

    cd /where/you/want/the/cctl/code/to/live
    git clone https://github.com/oskarpearson/cctl.git
    cd cctl
    git checkout 24mhz_clock_and_erf_stick_hack
    cd cctl-prog
    make clean all

Then connect the ERF stick over the serial port (normally /dev/ttyUSB0), and write the firmware:

    ./cctl-prog -d /dev/ttyUSB0 -f /path/to/subg_rfspy/output/uart0_alt1_SRF_ERF_WW_CCTL/uart0_alt1_SRF_ERF_WW_CCTL.hex
    Waiting 10s for bootloader, reset board now

Reset the board by disconnecting the +ve lead, and you should then see:

    ....Bootloader detected
    Erasing, programming and verifying page 1
    ...
    Erasing page 31
    Programming complete

# Addendum

## Command List

Different commands can be selected by sending the following numbers through the serial interface.

### Interrupt: 0

Interrupts the current command.

### Get State: 1

Check to make sure serial communications are functioning. Should return "OK" message.

### Get Version: 2

Prints the current version of the software e.g. "subg_rfspy 0.8".

### Get Packet: 3

Gets the latest packet of wireless data and displays it on the serial interface. 

Follow command with the desired channel to receive data from and how long to wait to 
receive a packet (in milliseconds).

### Send Packet: 4

Sends a packet of data from the serial interface over the wireless connection.

Follow command with desired channel to send data over, numbers of times to resend data, the 
delay between each resend (in milliseconds).

### Send and Listen: 5

Sends a packet of data from the serial interface over the wireless connection and waits
to receive a packet to display over the serial interface.

Follow command with desired channel to send data over, numbers of times to resend data, the 
delay between each resend (in milliseconds), the desired channel to receive data from, the 
delay between each receive retry (in milliseconds), and the number of times to retry 
receiving.

### Update Register: 6

Use this command to update one of the major registers with a desired value. See section Major 
Registers for a list of available registers and their functions.

Follow command with the register you want to update, and the value you want to update it to.

### Reset: 7

Use this command to reset the device.

### LED: 8

Use this command to manually control the LEDs on the device.

Follow this command with the LED you want to configure (0 for green, 1 for blue) and the mode
(0 for off, 1 for on, 2 for auto).

## Frequency List

Each channel number corresponds to a different specific frequency. Channels start with 916.5 MHz at 
Channel 0 and end with 934.4 MHz at Channel 255, in steps of about 0.07 MHz. The following is a list
of available channels and their corresponding frequencies.

0: 916.54 MHz  
1: 916.61 MHz  
2: 916.68 MHz  
3: 916.75 MHz  
4: 916.82 MHz  
5: 916.89 MHz  
6: 916.96 MHz  
7: 917.03 MHz  
8: 917.10 MHz  
9: 917.17 MHz  

10: 917.24 MHz  
11: 917.31 MHz  
12: 917.38 MHz  
13: 917.45 MHz  
14: 917.52 MHz  
15: 917.59 MHz  
16: 917.66 MHz  
17: 917.73 MHz  
18: 917.80 MHz  
19: 917.87 MHz  

20: 917.94 MHz  
21: 918.01 MHz  
22: 918.08 MHz  
23: 918.15 MHz  
24: 918.22 MHz  
25: 918.29 MHz  
26: 918.36 MHz  
27: 918.43 MHz  
28: 918.50 MHz  
29: 918.57 MHz  

30: 918.64 MHz  
31: 918.71 MHz  
32: 918.78 MHz  
33: 918.85 MHz  
34: 918.92 MHz  
35: 918.99 MHz  
36: 919.06 MHz  
37: 919.13 MHz  
38: 919.20 MHz  
39: 919.27 MHz  

40: 919.34 MHz  
41: 919.41 MHz  
42: 919.48 MHz  
43: 919.55 MHz  
44: 919.62 MHz  
45: 919.69 MHz  
46: 919.76 MHz  
47: 919.83 MHz  
48: 919.90 MHz  
49: 919.97 MHz  

50: 920.04 MHz  
51: 920.11 MHz  
52: 920.18 MHz  
53: 920.25 MHz  
54: 920.32 MHz  
55: 920.39 MHz  
56: 920.46 MHz  
57: 920.53 MHz  
58: 920.60 MHz  
59: 920.67 MHz  

60: 920.74 MHz  
61: 920.81 MHz  
62: 920.88 MHz  
63: 920.95 MHz  
64: 921.02 MHz  
65: 921.09 MHz  
66: 921.16 MHz  
67: 921.23 MHz  
68: 921.30 MHz  
69: 921.37 MHz  

70: 921.44 MHz  
71: 921.51 MHz  
72: 921.58 MHz  
73: 921.65 MHz  
74: 921.72 MHz  
75: 921.79 MHz  
76: 921.86 MHz  
77: 921.93 MHz  
78: 922.00 MHz  
79: 922.07 MHz  

80: 922.14 MHz  
81: 922.21 MHz  
82: 922.28 MHz  
83: 922.35 MHz  
84: 922.42 MHz  
85: 922.49 MHz  
86: 922.56 MHz  
87: 922.63 MHz  
88: 922.70 MHz  
89: 922.77 MHz  

90: 922.84 MHz  
91: 922.91 MHz  
92: 922.98 MHz  
93: 923.05 MHz  
94: 923.12 MHz  
95: 923.19 MHz  
96: 923.26 MHz  
97: 923.33 MHz  
98: 923.40 MHz  
99: 923.47 MHz  

100: 923.54 MHz  
101: 923.61 MHz  
102: 923.68 MHz  
103: 923.75 MHz  
104: 923.82 MHz  
105: 923.89 MHz  
106: 923.96 MHz  
107: 924.03 MHz  
108: 924.10 MHz  
109: 924.17 MHz  

110: 924.24 MHz  
111: 924.31 MHz  
112: 924.38 MHz  
113: 924.44 MHz  
114: 924.51 MHz  
115: 924.58 MHz  
116: 924.65 MHz  
117: 924.72 MHz  
118: 924.79 MHz  
119: 924.86 MHz  

120: 924.93 MHz  
121: 925.00 MHz  
122: 925.07 MHz  
123: 925.14 MHz  
124: 925.21 MHz  
125: 925.28 MHz  
126: 925.35 MHz  
127: 925.42 MHz  
128: 925.49 MHz  
129: 925.56 MHz  

130: 925.63 MHz  
131: 925.70 MHz  
132: 925.77 MHz  
133: 925.84 MHz  
134: 925.91 MHz  
135: 925.98 MHz  
136: 926.05 MHz  
137: 926.12 MHz  
138: 926.19 MHz  
139: 926.26 MHz  

140: 926.33 MHz  
141: 926.40 MHz  
142: 926.47 MHz  
143: 926.54 MHz  
144: 926.61 MHz  
145: 926.68 MHz  
146: 926.75 MHz  
147: 926.82 MHz  
148: 926.89 MHz  
149: 926.96 MHz  

150: 927.03 MHz  
151: 927.10 MHz  
152: 927.17 MHz  
153: 927.24 MHz  
154: 927.31 MHz  
155: 927.38 MHz  
156: 927.45 MHz  
157: 927.52 MHz  
158: 927.59 MHz  
159: 927.66 MHz  

160: 927.73 MHz  
161: 927.80 MHz  
162: 927.87 MHz  
163: 927.94 MHz  
164: 928.01 MHz  
165: 928.08 MHz  
166: 928.15 MHz  
167: 928.22 MHz  
168: 928.29 MHz  
169: 928.36 MHz  

170: 928.43 MHz  
171: 928.50 MHz  
172: 928.57 MHz  
173: 928.64 MHz  
174: 928.71 MHz  
175: 928.78 MHz  
176: 928.85 MHz  
177: 928.92 MHz  
178: 928.99 MHz  
179: 929.06 MHz  

180: 929.13 MHz  
181: 929.20 MHz  
182: 929.27 MHz  
183: 929.34 MHz  
184: 929.41 MHz  
185: 929.48 MHz  
186: 929.55 MHz  
187: 929.62 MHz  
188: 929.69 MHz  
189: 929.76 MHz  

190: 929.83 MHz  
191: 929.90 MHz  
192: 929.97 MHz  
193: 930.04 MHz  
194: 930.11 MHz  
195: 930.18 MHz  
196: 930.25 MHz  
197: 930.32 MHz  
198: 930.39 MHz  
199: 930.46 MHz  

200: 930.53 MHz  
201: 930.60 MHz  
202: 930.67 MHz  
203: 930.74 MHz  
204: 930.81 MHz  
205: 930.88 MHz  
206: 930.95 MHz  
207: 931.02 MHz  
208: 931.09 MHz  
209: 931.16 MHz  

210: 931.23 MHz  
211: 931.30 MHz  
212: 931.37 MHz  
213: 931.44 MHz  
214: 931.51 MHz  
215: 931.58 MHz  
216: 931.65 MHz  
217: 931.72 MHz  
218: 931.79 MHz  
219: 931.86 MHz  

220: 931.93 MHz  
221: 932.00 MHz  
222: 932.07 MHz  
223: 932.14 MHz  
224: 932.21 MHz  
225: 932.28 MHz  
226: 932.35 MHz  
227: 932.42 MHz  
228: 932.49 MHz  
229: 932.56 MHz  

230: 932.63 MHz  
231: 932.70 MHz  
232: 932.77 MHz  
233: 932.84 MHz  
234: 932.91 MHz  
235: 932.98 MHz  
236: 933.05 MHz  
237: 933.12 MHz  
238: 933.19 MHz  
239: 933.26 MHz  

240: 933.33 MHz  
241: 933.40 MHz  
242: 933.47 MHz  
243: 933.54 MHz  
244: 933.61 MHz  
245: 933.68 MHz  
246: 933.75 MHz  
247: 933.82 MHz  
248: 933.89 MHz  
249: 933.96 MHz  

250: 934.03 MHz  
251: 934.10 MHz  
252: 934.17 MHz  
253: 934.24 MHz  
254: 934.31 MHz  
255: 934.38 MHz  

## Major Registers

### SYNC1 (0xDF00)

Sync Word, High Byte. 8 MSB of 16-bit sync word.

### SYNC0 (0xDF01)

Sync Word, Low Byte. 8 LSB of 16-bit sync word.

### PKTLEN (0xDF02)

Packet Length. Indicates the packet length when fixed length packets are enabled. If
variable length packets are used, this value indicates the maximum length
packets allowed.

### PKTCTRL1 (0xDF03)

Packet Automation Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### PKTCTRL0 (0xDF04)

Packet Automation Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### ADDR (0xDF05)

Device Address. Address used for packet filtration. Optional broadcast addresses are 0
(0x00) and 255 (0xFF).

### CHANNR (0xDF06)

Channel Number. The 8-bit unsigned channel number, which is multiplied by the channel
spacing setting and added to the base frequency.

### FSCTRL1 (0xDF07)

Frequency Synthesizer Control.

### FSCTRL0 (0xDF08)

Frequency Synthesizer Control. Frequency offset added to the base frequency before being used by the
FS.

### FREQ2 (0xDF09)

Frequency Control Word, High Byte.

### FREQ1 (0xDF0A)

Frequency Control Word, Middle Byte.

### FREQ0 (0xDF0B)

Frequency Control Word, Low Byte.

### MDMCFG4 (0xDF0C)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MDMCFG3 (0xDF0D)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MDMCFG2 (0xDF0E)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MDMCFG1 (0xDF0F)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MDMCFG0 (0xDF10)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### DEVIATN (0xDF11)

Modem Deviation Setting. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MCSM2 (0xDF12)

Main Radio Control State Machine Configuration. Different bit ranges have 
different functions. See CC1110/CC1111 datasheet for more information.

### MCSM1 (0xDF13)

Main Radio Control State Machine Configuration. Different bit ranges have 
different functions. See CC1110/CC1111 datasheet for more information.

### MCSM0 (0xDF14)

Main Radio Control State Machine Configuration. Different bit ranges have 
different functions. See CC1110/CC1111 datasheet for more information.

### FOCCFG (0xDF15)

Frequency Offset Compensation Configuration. Different bit ranges have 
different functions. See CC1110/CC1111 datasheet for more information.

### BSCFG (0xDF16)

Bit Synchronization Configuration. Different bit ranges have different 
functions. See CC1110/CC1111 datasheet for more information.
  
### AGCCTRL2 (0xDF17)

AGC Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### AGCCTRL1 (0xDF18)

AGC Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### AGCCTRL0 (0xDF19)

AGC Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### FREND1 (0xDF1A)

Front End RX Configuration. Different bit ranges have different 
functions. See CC1110/CC1111 datasheet for more information.

### FREND0 (0xDF1B)

Front End RX Configuration. Different bit ranges have different 
functions. See CC1110/CC1111 datasheet for more information.

### FSCAL3 (0xDF1C)

Frequency Synthesizer Calibration. Different bit ranges have 
different functions. See CC1110/CC1111 datasheet for more information.

### FSCAL2 (0xDF1D)

Frequency Synthesizer Calibration. Different bit ranges have different 
functions. See CC1110/CC1111 datasheet for more information.

### FSCAL1 (0xDF1E)

Frequency Synthesizer Calibration. Different bit ranges have different 
functions. See CC1110/CC1111 datasheet for more information.

### FSCAL0 (0xDF1F)

Frequency Synthesizer Calibration. Different bit ranges have different 
functions. See CC1110/CC1111 datasheet for more information.
	  
### PA_TABLE1 (0xDF2D)

Power amplifier output power setting 1.

### PA_TABLE0 (0xDF2E)

Power amplifier output power setting 0.