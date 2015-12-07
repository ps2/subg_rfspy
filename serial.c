
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"

#ifndef NON_NATIVE_TEST
#include <cc1110.h>  // /usr/share/sdcc/include/mcs51/cc1110.h
#include "ioCCxx10_bitdef.h"
#endif

typedef void (*CommandHandler)();

CommandHandler handlers[] = {
  /* 0 */ 0,
  /* 1 */ cmd_get_state,
  /* 2 */ cmd_get_version,
  /* 3 */ cmd_set_channel, 
  /* 4 */ cmd_get_packet
};

void cmd_set_channel() {
  CHANNR = serial_rx_byte();
}

void cmd_get_packet() {
  get_packet();
}

void cmd_get_state() {
  serial_tx_byte('O');
  serial_tx_byte('K');
  serial_tx_byte(0);
}

void cmd_get_version() {
  serial_tx_byte('0');
  serial_tx_byte('.');
  serial_tx_byte('1');
  serial_tx_byte(0);
}

void configure_uart()
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

uint8_t serial_rx_byte() {
  U1CSR |= 0x40; URX1IF = 0;
  while(!URX1IF);
  return U1DBUF;
} 

void run_command_from_serial() {
  uint8_t cmd;
  cmd = serial_rx_byte();
  if (cmd > 0 && cmd < sizeof(handlers)/sizeof(handlers[0])) {
    handlers[cmd]();
  }
}

void serial_tx_byte(uint8_t tx_byte) {
  UTX1IF = 0;
  U1DBUF = tx_byte;
  while ( !UTX1IF );
  UTX1IF = 0;
}

void serial_tx_buf(uint8_t *buf, uint8_t len) {
  uint8_t pos = 0;
  while(len-->0) {
    serial_tx_byte(buf[pos++]);
  }
}

