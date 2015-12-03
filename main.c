/* Control a cc1110 for sub-ghz RF comms over uart. */

#ifndef NON_NATIVE_TEST
#include <cc1110.h>  // /usr/share/sdcc/include/mcs51/cc1110.h
#include "ioCCxx10_bitdef.h"
#endif

// RileyLink HW
#define GREEN_LED P0_0
#define BLUE_LED P0_1

void configureUART()
{
  // UART1 Alt. 2 
  // P1.4 - CT
  // P1.5 - RT
  // P1.6 - TX
  // P1.7 - RX

  U1CSR |= 0x80; // UART Mode
  PERCFG |= 0x02; // Alternative 2
  P1SEL |= 0xf0; // P1.7 -> P1.4
  P0SEL &= ~0x3c;

  ///////////////////////////////////////////////////////////////
  // This initial code section ensures that the SoC system clock is driven
  // by the HS XOSC:
  // Clear CLKCON.OSC to make the SoC operate on the HS XOSC.
  // Set CLKCON.TICKSPD/CLKSPD = 000 => system clock speed = HS RCOSC speed.
  CLKCON &= 0x80;
  // Monitor CLKCON.OSC to ensure that the HS XOSC is stable and actually
  // applied as system clock source before continuing code execution
  while(CLKCON & 0x40);
  // Set SLEEP.OSC_PD to power down the HS RCOSC.
  SLEEP |= 0x04;
  ///////////////////////////////////////////////////////////////
  // Initialize bitrate (U0BAUD.BAUD_M, U0GCR.BAUD_E)
  // Bitrate 19200
  U1BAUD = 163;
  U1GCR = (U0GCR&~0x1F) | 9; 

  
  U1UCR |= 0xc0; // Flush, and turn on hw flow control
}

void configureRadio()
{
#ifndef NON_NATIVE_TEST
  /* RF settings SoC: CC1110 */
  SYNC1     = 0xFF; // sync word, high byte
  SYNC0     = 0x00; // sync word, low byte
  PKTLEN    = 0xFF; // packet length
  PKTCTRL1  = 0x00; // packet automation control
  PKTCTRL0  = 0x00; // packet automation control
  ADDR      = 0x00;
  CHANNR    = 0x02; // channel number
  FSCTRL1   = 0x06; // frequency synthesizer control
  FSCTRL0   = 0x00;
  FREQ2     = 0x26; // frequency control word, high byte
  FREQ1     = 0x2F; // frequency control word, middle byte
  FREQ0     = 0xE4; // frequency control word, low byte
  MDMCFG4   = 0xB9; // modem configuration
  MDMCFG3   = 0x66; // modem configuration
  MDMCFG2   = 0x33; // modem configuration
  MDMCFG1   = 0x61; // modem configuration
  MDMCFG0   = 0xe6; // modem configuration
  DEVIATN   = 0x15; // modem deviation setting
  MCSM2     = 0x07;
  MCSM1     = 0x30;
  MCSM0     = 0x18; // main radio control state machine configuration
  FOCCFG    = 0x17; // frequency offset compensation configuration
  BSCFG     = 0x6C;
  FREND1    = 0x56; // front end tx configuration
  FREND0    = 0x11; // front end tx configuration
  FSCAL3    = 0xE9; // frequency synthesizer calibration
  FSCAL2    = 0x2A; // frequency synthesizer calibration
  FSCAL1    = 0x00; // frequency synthesizer calibration
  FSCAL0    = 0x1F; // frequency synthesizer calibration
  TEST1     = 0x31; // various test settings
  TEST0     = 0x09; // various test settings
  PA_TABLE0 = 0x00; // needs to be explicitly set!
  PA_TABLE1 = 0x57; // pa power setting 0 dBm
#endif
}

int main(void)
{
  // init LEDS
  P0DIR |= 0x03;
  GREEN_LED = 0;
  BLUE_LED = 0;

  configureRadio();
  configureUART();
  GREEN_LED = 0;
  BLUE_LED = 1;
  UTX1IF = 0;
  while(1) {
    U1DBUF = 0x41; // 'A'
    //U1DBUF = 0b11110101;
    while ( !UTX1IF );
    UTX1IF = 0;

    U1DBUF = 0x42; // 'B'
    while ( !UTX1IF );
    UTX1IF = 0;

    U1DBUF = 0x43; // 'C'
    while ( !UTX1IF );
    UTX1IF = 0;
    GREEN_LED = !GREEN_LED;
    BLUE_LED = !BLUE_LED;
  }
}
