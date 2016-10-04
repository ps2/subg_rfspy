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

### Read Register: 9

Use this command to read the current value of one of the major registers. See Major Registers section
for a list of available registers and their functions.

## Frequency Channel Selection

Each channel number corresponds to a different specific frequency. Channels start with 916.5 MHz at 
Channel 0 and end with 934.4 MHz at Channel 255, in steps of about 0.07 MHz. You can select different
channels by adjusting the CHANNR.CHAN register. Available channels are governed by the following equation:

	f_carrier = (24MHz/(2^16))*(FREQ + CHAN(256 + CHANSPC_M)*2^(CHANSPC_E - 2))

The base frequency, FREQ, is set by the FREQ2, FREQ1, and FREQ0 registers. The value default value is 
2502768 (0x263070).  
CHANSPC_M and CHANSPC_E are used to set the channel spacing, and are contained in the registers MDMCFG0
and MDMCFG1 (bits 1:0), respectively. Their default values are 126 (0x7E) and 1.

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