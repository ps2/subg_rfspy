
#include <stdint.h>
#include "hardware.h"

#ifndef NON_NATIVE_TEST
#include <cc1110.h>  // /usr/share/sdcc/include/mcs51/cc1110.h
#include "ioCCxx10_bitdef.h"
#endif

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
  // Initialize bitrate (U0BAUD.BAUD_M, U0GCR.BAUD_E)
  // Bitrate 19200
  U1BAUD = 163;
  U1GCR = (U0GCR&~0x1F) | 9; 

  
  U1UCR |= 0xc0; // Flush, and turn on hw flow control
}


void rx() {
  uint8_t key;
  while(1) {
    U1CSR |= 0x40; URX1IF = 0;
    while(!URX1IF);
    key = U1DBUF;
    if (key == 'a') {
      GREEN_LED = ! GREEN_LED;
    }
  }
}

void tx() {
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

