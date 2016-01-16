# UART on RileyLink

Perform the build. The output file will be stored at output/uart1_alt2_RILEYLINK_US/uart1_alt2_RILEYLINK_US.hex

    make -f Makefile.uart1_alt2

Perform the install:

    make -f Makefile.uart1_alt2 install

UART/SPI pins exposed on cc1110 debug header:

    PIN - SPI_alt2 / UART1_alt2
    P1.4 - CT / SSN
    P1.5 - RT / SCK
    P1.6 - TX / MOSI
    P1.7 - RX / MISO

# Radio Frequency Selection

This code defaults to building firmware that works with US-based pumps. If your
pump model number ends with 'WW' then you need a 'WorldWide' firmware:

    make -f Makefile.uart1_alt2 RADIO_LOCALE=WW

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
