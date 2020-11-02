
#include <stdint.h>
#include <time.h>
#include "hardware.h"
#include "subg_rfspy.h"
#include "serial.h"
#include "radio.h"
#include "statistics.h"
#include "timer.h"

#define SPI_BUF_LEN 128
#define FLUSH_TIMEOUT_MS 5000

static volatile uint8_t input_buffer_read_idx = 0;
static volatile uint8_t input_buffer_write_idx = 0;
static uint8_t __xdata input_buffer_mem[SPI_BUF_LEN];
static volatile uint8_t ready_to_send = 0;

static volatile uint8_t output_buffer_read_idx = 0;
static volatile uint8_t output_buffer_write_idx = 0;
static uint8_t __xdata output_buffer_mem[SPI_BUF_LEN];
volatile uint8_t serial_data_available;

#define SPI_MODE_IDLE 0
#define SPI_MODE_SIZE 1
#define SPI_MODE_XFER 2
#define SPI_MODE_OUT_OF_SYNC 3
volatile uint8_t spi_mode;

volatile uint8_t master_send_size = 0;
volatile uint8_t slave_send_size = 0;
volatile uint8_t xfer_size = 0;


/***************************************************************************
 *
 * SPI encoding:
 *
 * Master sends a 0x99 byte, followed by number of bytes that will be sent.
 * Slave sends back number of bytes available during second xfer.
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

  U1DBUF_write = 0x44;
}

void rx1_isr(void) __interrupt URX1_VECTOR
{
  uint8_t value;
  value = U1DBUF_read;

  switch(spi_mode)
  {
    case SPI_MODE_OUT_OF_SYNC:
      if (value == 0x99) {
        spi_mode = SPI_MODE_SIZE;
      }
      break;
    case SPI_MODE_IDLE:
      if (value != 0x99) {
        spi_sync_failure_count++;
        spi_mode = SPI_MODE_OUT_OF_SYNC;
      } else {
        spi_mode = SPI_MODE_SIZE;
      }
      break;
    case SPI_MODE_SIZE:
      if (value > SPI_BUF_LEN) {
        spi_sync_failure_count++;
        spi_mode = SPI_MODE_OUT_OF_SYNC;
        return;
      }
      master_send_size = value;
      xfer_size = master_send_size;
      if (slave_send_size > xfer_size) {
        xfer_size = slave_send_size;
      }
      if (xfer_size > 0) {
        spi_mode = SPI_MODE_XFER;
      } else {
        spi_mode = SPI_MODE_IDLE;
      }
      break;
    case SPI_MODE_XFER:
      if(xfer_size > 0) {
        if (input_buffer_write_idx < master_send_size) {
          input_buffer_mem[input_buffer_write_idx++] = value;
          if (input_buffer_write_idx == master_send_size) {
            master_send_size = 0;
            serial_data_available = 1;
          }
        }
        xfer_size--;
      }
      if (xfer_size == 0) {
        slave_send_size = 0;
        spi_mode = SPI_MODE_IDLE;
      }
      break;
  }
}

void tx1_isr(void) __interrupt UTX1_VECTOR
{
  IRCON2 &= ~BIT2; // Clear UTX1IF
  if (spi_mode == SPI_MODE_IDLE) {
    if (ready_to_send) {
      slave_send_size = output_buffer_write_idx;
      U1DBUF_write = slave_send_size;
    } else {
      U1DBUF_write = 0;
    }
  }
  else if (slave_send_size > 0) {
    U1DBUF_write = output_buffer_mem[output_buffer_read_idx++];
    slave_send_size--;
  } else {
    // Filler for when we are receiving data, but not sending anything
    U1DBUF_write = 0x00;
  }
}

uint8_t serial_rx_avail()
{
  int remaining = input_buffer_write_idx - input_buffer_read_idx;
  if (remaining > 0) {
    return (uint8_t) remaining;
  } else {
    return 0;
  }
}

uint8_t serial_rx_byte()
{
  uint8_t s_data;
  if (!serial_data_available) {
    while(!serial_data_available && !subg_rfspy_should_exit) {
      feed_watchdog();
    }
  }
  s_data = input_buffer_mem[input_buffer_read_idx++];
  if (input_buffer_read_idx == input_buffer_write_idx) {
    serial_data_available = 0;
    input_buffer_read_idx = input_buffer_write_idx = 0;
  }
  return s_data;
}

uint16_t serial_rx_word()
{
  return (serial_rx_byte() << 8) + serial_rx_byte();
}

uint32_t serial_rx_long()
{
  return ((uint32_t)serial_rx_word() << 16) + serial_rx_word();
}

void serial_tx_byte(uint8_t tx_byte)
{
  output_buffer_mem[output_buffer_write_idx++] = tx_byte;
}

void serial_tx_word(uint16_t tx_word)
{
  serial_tx_byte(tx_word >> 8);
  serial_tx_byte(tx_word & 0xff);
}

void serial_tx_long(uint32_t tx_long)
{
  serial_tx_byte(tx_long >> 24);
  serial_tx_byte((tx_long >> 16) & 0xff);
  serial_tx_byte((tx_long >> 8) & 0xff);
  serial_tx_byte(tx_long & 0xff);
}

void serial_flush()
{
  uint32_t start_time;

  if (output_buffer_write_idx == 0) {
    return;
  }

  RESPONSE_AVAILABLE_SIGNAL_PIN = 1;

  // Waiting for tx isr to send the data
  read_timer(&start_time);
  ready_to_send = 1;
  while((output_buffer_read_idx != output_buffer_write_idx) && !subg_rfspy_should_exit) {
    feed_watchdog();
    if (check_elapsed(start_time, FLUSH_TIMEOUT_MS)) {
      break;
    }
  }

  output_buffer_read_idx = output_buffer_write_idx = 0;

  RESPONSE_AVAILABLE_SIGNAL_PIN = 0;

  // Waiting to finish spi transfer
  while(slave_send_size != 0 && !subg_rfspy_should_exit) {
    feed_watchdog();
    if (check_elapsed(start_time, FLUSH_TIMEOUT_MS)) {
      break;
    }
  }
  ready_to_send = 0;
}

void serial_tx_str(const char *str) {
  while(*str != 0) {
    serial_tx_byte(*str);
    str++;
  }
  serial_flush();
}
