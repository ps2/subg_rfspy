#!/bin/bash

set -x
set -e

############################################################################
# Rileylink and similar
############################################################################
# USA:
#make -f Makefile.uart1_alt2
#make -f Makefile.uart1_alt2 CODE_LOC=0x400 CODE_LOC_NAME=CCTL
make -f Makefile.spi1_alt2 
# Worldwide:
#make -f Makefile.uart1_alt2 RADIO_LOCALE=WW
#make -f Makefile.uart1_alt2 CODE_LOC=0x400 CODE_LOC_NAME=CCTL RADIO_LOCALE=WW
make -f Makefile.spi1_alt2 RADIO_LOCALE=WW

############################################################################
# ERF stick
############################################################################
# USA:
#make -f Makefile.uart0_alt1 BOARD_TYPE=SRF_ERF
#make -f Makefile.uart0_alt1 BOARD_TYPE=SRF_ERF CODE_LOC=0x400 CODE_LOC_NAME=CCTL
# Worldwide:
#make -f Makefile.uart0_alt1 BOARD_TYPE=SRF_ERF RADIO_LOCALE=WW
#make -f Makefile.uart0_alt1 BOARD_TYPE=SRF_ERF CODE_LOC=0x400 CODE_LOC_NAME=CCTL RADIO_LOCALE=WW

############################################################################
# TI stick
############################################################################
# USA:
make -f Makefile.usb_ep0
make -f Makefile.spi1_alt2 BOARD_TYPE=TI_DONGLE
make -f Makefile.usb_ep0 CODE_LOC=0x1400 CODE_LOC_NAME=CCBOOTLOADER
# Worldwide:
make -f Makefile.usb_ep0 RADIO_LOCALE=WW
make -f Makefile.spi1_alt2 BOARD_TYPE=TI_DONGLE RADIO_LOCALE=WW
make -f Makefile.usb_ep0 RADIO_LOCALE=WW CODE_LOC=0x1400 CODE_LOC_NAME=CCBOOTLOADER

exit 0
