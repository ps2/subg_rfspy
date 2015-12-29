
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"

void configure_serial()
{

  // UART0 Alt. 1
  // P0.2 - RX
  // P0.3 - TX
  // P0.4 - CT
  // P0.5 - RT

  U0CSR |= 0x80; // UART Mode
  PERCFG &= ~0x01; // Alternative 1
  P0SEL |= 0x3C; // P0.5 -> P0.2
  P1SEL &= ~0x3C;

  ///////////////////////////////////////////////////////////////
  // Initialize bitrate (U0BAUD.BAUD_M, U0GCR.BAUD_E)
  // Bitrate 19200
  U0BAUD = 163;
  U0GCR = (U0GCR&~0x1F) | 9;
  U0UCR |= HARDWARE_FLOW_CONTROL_CONFIG; // Flush, and configure hw flow control

  // Enable receive
  U0CSR |= 0x40;
  URX0IF = 0;

}

uint8_t serial_rx_byte() {
  uint8_t s_data;
  while(!SERIAL_DATA_AVAILABLE);  // URX0IF
  s_data = U0DBUF;
  URX0IF = 0;
  return s_data;
}

uint16_t serial_rx_word() {
  return (serial_rx_byte() << 8) + serial_rx_byte();
}

void serial_tx_byte(uint8_t tx_byte) {
  UTX0IF = 0;
  U0DBUF = tx_byte;
  while ( !UTX0IF );
  UTX0IF = 0;
}

void serial_tx_str(const char *str) {
  while(*str != 0) {
    serial_tx_byte(*str);
    str++;
  }
  serial_tx_byte(0);
}
