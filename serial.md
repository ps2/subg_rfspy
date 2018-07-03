# Serial support
*Since the change to SPI framing carrying semantic information about the Length
of packets, the serial protocol no longer can be expected to work. It needs to
be updated to support some kind of framing.*

This information is provide for users of legacy versions of the software.

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
