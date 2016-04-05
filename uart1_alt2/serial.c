
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"

#define SERIAL_BUF_LEN 220

volatile uint8_t __xdata serial_input_buf[SERIAL_BUF_LEN];
volatile uint8_t input_size = 0;
volatile uint8_t input_head_idx = 0;
volatile uint8_t input_tail_idx = 0;

volatile uint8_t __xdata serial_output_buf[SERIAL_BUF_LEN];
volatile uint8_t output_size = 0;
volatile uint8_t output_head_idx = 0;
volatile uint8_t output_tail_idx = 0;

volatile uint8_t ready_to_send = 0;

volatile uint8_t serial_data_available;

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
  IEN0 |= 0x88;
}

void rx1_isr(void) __interrupt URX1_VECTOR {
  URX1IF = 0;
  if (input_size < SERIAL_BUF_LEN) {
    serial_input_buf[input_head_idx] = U1DBUF;
    input_head_idx++;
    if (input_head_idx >= SERIAL_BUF_LEN) {
      input_head_idx = 0;
    }
    input_size++;
    serial_data_available = 1;
  } else {
    // overflow
  }
}

uint8_t serial_rx_byte() {
  uint8_t s_data;
  while(!SERIAL_DATA_AVAILABLE);  // URX1IF
  s_data = serial_input_buf[input_tail_idx];
  input_tail_idx++;
  if (input_tail_idx >= SERIAL_BUF_LEN) {
    input_tail_idx = 0;
  }
  input_size--;
  if (input_size == 0) {
    serial_data_available = 0;
  }
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



