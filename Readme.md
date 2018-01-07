# subg_rfspy

## Pre-Requisites for Building subg_rfspy

[sdcc] (http://sdcc.sourceforge.net/) package is required for this build.

    sudo apt-get install sdcc

It's also possible to build it under Windows. The best way to do this is to:

1. install SDCC
2. install Cygwin
3. use Cygwin to install make
4. Use cygwin bash to build the project

# Building

You'll need to select a build type, like `spi1_alt2`.  The examples below use this value, but you'll want to use the correct one for your hardware and use case.  See below for different hardware types and use cases.

Perform the build. The output file will be stored at output/uart0_alt1_RILEYLINK_US/uart0_alt1_RILEYLINK_US.hex

    make -f Makefile.spi1_alt2

Perform the install:

    make -f Makefile.spi1_alt2 install

# Radio Frequency Selection

This code defaults to building firmware that is tuned to 916.5 Mhz. You can also build a 'WorldWide' firmware. This changes the default frequency to 868 and tweaks a few other settings.

    make -f Makefile.spi1_alt2 RADIO_LOCALE=WW

# RileyLink

If you are using a [RileyLink](https://github.com/ps2/rileylink) via the onboard bluetooth module (which should be loaded with ble_rfspy), then you'll want to use `spi1_alt2` as your build type.

# Protocol

See [protocol.md](protocol.md)


# Addendum

## Serial/Uart Support

Serial support is no longer supported.  See [serial.md](serial.md) for
historical information.

## Frequency Channel Selection

Each channel number corresponds to a different specific frequency. Channels start with 916.5 MHz at
Channel 0 and end with 934.4 MHz at Channel 255, in steps of about 0.07 MHz. You can select different
channels by adjusting the CHANNR.CHAN register. Available channels are governed by the following equation:

`f_carrier = (24MHz/(2^16))*(FREQ + CHAN(256 + CHANSPC_M)*2^(CHANSPC_E - 2))`

The base frequency, FREQ, is set by the FREQ2, FREQ1, and FREQ0 registers. The value default value is
2502768 (0x263070).  
CHANSPC_M and CHANSPC_E are used to set the channel spacing, and are contained in the registers MDMCFG0
and MDMCFG1 (bits 1:0), respectively. Their default values are 126 (0x7E) and 1.

## Supported Registers

### SYNC1 (0x00)

Sync Word, High Byte. 8 MSB of 16-bit sync word.

### SYNC0 (0x01)

Sync Word, Low Byte. 8 LSB of 16-bit sync word.

### PKTLEN (0x02)

Packet Length. Indicates the packet length when fixed length packets are enabled. If
variable length packets are used, this value indicates the maximum length
packets allowed.

### PKTCTRL1 (0x03)

Packet Automation Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### PKTCTRL0 (0x04)

Packet Automation Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### ADDR (0x05)

Device Address. Address used for packet filtration. Optional broadcast addresses are 0
(0x00) and 255 (0xFF).

### CHANNR (0x06)

Channel Number. The 8-bit unsigned channel number, which is multiplied by the channel
spacing setting and added to the base frequency.

### FSCTRL1 (0x07)

Frequency Synthesizer Control.

### FSCTRL0 (0x08)

Frequency Synthesizer Control. Frequency offset added to the base frequency before being used by the
FS.

### FREQ2 (0x09)

Frequency Control Word, High Byte.

### FREQ1 (0x0A)

Frequency Control Word, Middle Byte.

### FREQ0 (0x0B)

Frequency Control Word, Low Byte.

### MDMCFG4 (0x0C)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MDMCFG3 (0x0D)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MDMCFG2 (0x0E)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MDMCFG1 (0x0F)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MDMCFG0 (0x10)

Modem Configuration. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### DEVIATN (0x11)

Modem Deviation Setting. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### MCSM2 (0x12)

Main Radio Control State Machine Configuration. Different bit ranges have
different functions. See CC1110/CC1111 datasheet for more information.

### MCSM1 (0x13)

Main Radio Control State Machine Configuration. Different bit ranges have
different functions. See CC1110/CC1111 datasheet for more information.

### MCSM0 (0x14)

Main Radio Control State Machine Configuration. Different bit ranges have
different functions. See CC1110/CC1111 datasheet for more information.

### FOCCFG (0x15)

Frequency Offset Compensation Configuration. Different bit ranges have
different functions. See CC1110/CC1111 datasheet for more information.

### BSCFG (0x16)

Bit Synchronization Configuration. Different bit ranges have different
functions. See CC1110/CC1111 datasheet for more information.

### AGCCTRL2 (0x17)

AGC Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### AGCCTRL1 (0x18)

AGC Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### AGCCTRL0 (0x19)

AGC Control. Different bit ranges have different functions.
See CC1110/CC1111 datasheet for more information.

### FREND1 (0x1A)

Front End RX Configuration. Different bit ranges have different
functions. See CC1110/CC1111 datasheet for more information.

### FREND0 (0x1B)

Front End RX Configuration. Different bit ranges have different
functions. See CC1110/CC1111 datasheet for more information.

### FSCAL3 (0x1C)

Frequency Synthesizer Calibration. Different bit ranges have
different functions. See CC1110/CC1111 datasheet for more information.

### FSCAL2 (0x1D)

Frequency Synthesizer Calibration. Different bit ranges have different
functions. See CC1110/CC1111 datasheet for more information.

### FSCAL1 (0x1E)

Frequency Synthesizer Calibration. Different bit ranges have different
functions. See CC1110/CC1111 datasheet for more information.

### FSCAL0 (0x1F)

Frequency Synthesizer Calibration. Different bit ranges have different
functions. See CC1110/CC1111 datasheet for more information.

### PA_TABLE1 (0x2D)

Power amplifier output power setting 1.

### PA_TABLE0 (0x2E)

Power amplifier output power setting 0.
