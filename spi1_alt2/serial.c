
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"

#define SPI_BUF_LEN 220 

volatile uint8_t __xdata spi_input_buf[SPI_BUF_LEN];
volatile uint8_t input_size = 0;
volatile uint8_t input_head_idx = 0;
volatile uint8_t input_tail_idx = 0;

volatile uint8_t __xdata spi_output_buf[SPI_BUF_LEN];
volatile uint8_t output_size = 0;
volatile uint8_t output_head_idx = 0;
volatile uint8_t output_tail_idx = 0;

volatile uint8_t serial_data_available;

/***************************************************************************
 * 
 * SPI encoding: 
 * 
 * In order to handle the SPI master polling us when we have no data to send,
 * we use a special encoding. 
 * 
 * 0x99 = No data
 * 0x98 = Escape character.  Next character will be a special character
 *        (0x99 or 0x98), but will be bitwise inverted
 *
 */


void configure_serial()
{

  /***************************************************************************
   * Setup slave SPI on USART1, Alt 2
   *
   * (SS): P1_4
   *  (C): P1_5
   * (MO): P1_6
   * (MI): P1_7
   */

  // configure USART1 for Alternative 2 => Port P1 (PERCFG.U1CFG = 1)
  // To avoid potential I/O conflict with USART0:
  // Configure USART0 for Alternative 1 => Port P0 (PERCFG.U0CFG = 0)
  PERCFG = (PERCFG & ~PERCFG_U0CFG) | PERCFG_U1CFG;

  // Give priority to USART 1 over USART 0 for port 0 pins
  P2DIR = 0x01;

  // Set pins 2, 3 and 5 as peripheral I/O and pin 4 as GPIO output
  P1SEL = P1SEL | BIT4 | BIT5 | BIT6 | BIT7;
  P1DIR = P1DIR & ~(BIT4 | BIT5 | BIT6 | BIT7);

  /***************************************************************************
   * Configure SPI
   */

  // Set USART to SPI mode and Slave mode
  U1CSR = (U1CSR & ~U1CSR_MODE) | U1CSR_SLAVE;

  // Set:
  // - mantissa value
  // - exponent value
  // - clock phase to be centered on first edge of SCK period
  // - negative clock polarity (SCK low when idle)
  // - bit order for transfers to LSB first

// These values will give a baud rate of approx. 62.5kbps for 26 MHz clock
#define SPI_BAUD_M 59
#define SPI_BAUD_E 11

  U1BAUD = SPI_BAUD_M;
  U1GCR = (U1GCR & ~(U1GCR_BAUD_E | U1GCR_CPOL | U1GCR_CPHA | U1GCR_ORDER))
        | SPI_BAUD_E;

  TCON &= ~BIT3; // Clear URX1IF
  URX1IE = 1;    // Enable URX1IE interrupt

  IRCON2 &= ~BIT2; // Clear UTX1IF
  IEN2 |= BIT3;    // Enable UTX1IE interrupt
}

void rx1_isr(void) __interrupt URX1_VECTOR {
  if (input_size < SPI_BUF_LEN) {
    spi_input_buf[input_head_idx] = U1DBUF;
    input_head_idx++;
    if (input_head_idx >= SPI_BUF_LEN) {
      input_head_idx = 0;
    }
    input_size++;
    serial_data_available = 1;
  }
}

void tx1_isr(void) __interrupt UTX1_VECTOR {
  if (output_size > 0) {
    GREEN_LED = 1;
    U1DBUF = spi_output_buf[output_tail_idx];
    output_size--;
    output_tail_idx++;
    if (output_tail_idx >= SPI_BUF_LEN) {
      output_tail_idx = 0;
    }
  } else {
    U1DBUF = 0x99; 
  }
}

uint8_t serial_rx_byte() {
  uint8_t s_data;
  while(!SERIAL_DATA_AVAILABLE);
  s_data = spi_input_buf[input_tail_idx];
  input_tail_idx++;
  if (input_tail_idx >= SPI_BUF_LEN) {
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

void add_byte_to_tx_buf_without_escaping(uint8_t tx_byte) {
  if (output_size >= SPI_BUF_LEN) {
    // drop oldest byte
    output_size--;
    output_tail_idx++;
    if (output_tail_idx >= SPI_BUF_LEN) {
      output_tail_idx = 0;
    }
  }
  spi_output_buf[output_head_idx] = tx_byte;
  output_head_idx++;
  if (output_head_idx >= SPI_BUF_LEN) {
    output_head_idx = 0;
  }
  output_size++;
}

void serial_tx_byte(uint8_t tx_byte) {
  // Escape special characters
  if(tx_byte == 0x99 || tx_byte == 0x98) {
    add_byte_to_tx_buf_without_escaping(0x98);
    add_byte_to_tx_buf_without_escaping(~tx_byte);
  }
}

void serial_tx_str(const char *str) {
  while(*str != 0) {
    serial_tx_byte(*str);
    str++;
  }
  serial_tx_byte(0);
}


