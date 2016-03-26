
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"

void configure_serial()
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
  // Initialize bitrate (U1BAUD.BAUD_M, U1GCR.BAUD_E)
  // Bitrate 19200
#if SYSTEM_CLOCK_MHZ == 26
  U1BAUD = 131; 
#else
  U1BAUD = 163; 
#endif
  U1GCR = (U0GCR&~0x1F) | 9; 
  U1UCR |= HARDWARE_FLOW_CONTROL_CONFIG; // Flush, and configure hw flow control

  // Enable receive
  U1CSR |= 0x40;
  URX1IF = 0;

}

uint8_t serial_rx_byte() {
  uint8_t s_data;
  while(!SERIAL_DATA_AVAILABLE);  // URX1IF
  s_data = U1DBUF;
  URX1IF = 0;
  return s_data;
} 

uint16_t serial_rx_word() {
  return (serial_rx_byte() << 8) + serial_rx_byte();
}

uint32_t serial_rx_long() {
  return ((uint32_t)serial_rx_word() << 16) + serial_rx_word();
}


void serial_tx_byte(uint8_t tx_byte) {
  UTX1IF = 0;
  U1DBUF = tx_byte;
  while ( !UTX1IF );
  UTX1IF = 0;
}

void serial_tx_str(const char *str) {
  while(*str != 0) {
    serial_tx_byte(*str);
    str++;
  }
  serial_tx_byte(0);
}


