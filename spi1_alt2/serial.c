
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "fifo.h"

#define SPI_BUF_LEN 128

static fifo_buffer __xdata input_buffer;
static volatile uint8_t __xdata input_buffer_mem[SPI_BUF_LEN];

static fifo_buffer __xdata output_buffer;
static volatile uint8_t __xdata output_buffer_mem[SPI_BUF_LEN];

volatile uint8_t ready_to_send = 0;

volatile uint8_t serial_data_available;

volatile uint8_t xfer_remaining = 0;

#define SPI_MODE_IDLE 0
#define SPI_MODE_TX_SIZE 1
#define SPI_MODE_RX_SIZE 2
#define SPI_MODE_XFER 3
volatile uint8_t spi_mode;

volatile uint8_t master_send_size = 0;
volatile uint8_t slave_send_size = 0;


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

  U1DBUF = 0x44;

  // Initialize fifos
  fifo_init(&input_buffer, input_buffer_mem, SPI_BUF_LEN);
  fifo_init(&output_buffer, output_buffer_mem, SPI_BUF_LEN);
}

void rx1_isr(void) __interrupt URX1_VECTOR {
  uint8_t value;
  value = U1DBUF;

  if (spi_mode == SPI_MODE_TX_SIZE) {
    if (value != 0x99) {
      // Error!
      return;
    }
    spi_mode = SPI_MODE_RX_SIZE;
    return;
  }

  if (spi_mode == SPI_MODE_RX_SIZE) {
    master_send_size = value;
    if (master_send_size > 0 || slave_send_size > 0) {
      spi_mode = SPI_MODE_XFER;
      xfer_remaining = (slave_send_size > master_send_size) ? slave_send_size : master_send_size;
      IRCON2 |= BIT2; // Trigger UTX1IF
    } else {
      spi_mode = SPI_MODE_IDLE;
      led_set_state(1, 0); //BLUE_LED = 0;
    }
    return;
  }

  if (spi_mode == SPI_MODE_XFER) {
    if (fifo_count(&input_buffer) < master_send_size) {
      fifo_put(&input_buffer, value);
      if (fifo_count(&input_buffer) == master_send_size) {
        master_send_size = 0;
        serial_data_available = 1;
      }
    }
    xfer_remaining--;
    if (xfer_remaining == 0) {
      spi_mode = SPI_MODE_IDLE;
      led_set_state(1, 0); //BLUE_LED = 0;
    }
  }
}

void tx1_isr(void) __interrupt UTX1_VECTOR {
  IRCON2 &= ~BIT2; // Clear UTX1IF

  if (spi_mode == SPI_MODE_IDLE) {
    if (ready_to_send) {
      slave_send_size = fifo_count(&output_buffer);
      U1DBUF = slave_send_size;
      led_set_state(1, 1); //BLUE_LED = 0;
    } else {
      U1DBUF = 0;
    }
    spi_mode = SPI_MODE_TX_SIZE;
    return;
  }

  if (spi_mode == SPI_MODE_XFER) {
    if (slave_send_size > 0) {
      U1DBUF = fifo_get(&output_buffer);
      if (fifo_empty(&output_buffer)) {
        slave_send_size = 0;
      }
    } else {
      // Filler for when we are receiving data, but not sending anything
      U1DBUF = 0x98;
    }
  } else {
    // Filler
    U1DBUF = 0x97;
  }
}

uint8_t serial_rx_avail() {
  return fifo_count(&input_buffer);
}

uint8_t serial_rx_byte() {
  uint8_t s_data;
  while(!serial_data_available);
  s_data = fifo_get(&input_buffer);
  if (fifo_empty(&input_buffer)) {
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
  fifo_put(&output_buffer, tx_byte);
}

void serial_flush() {
  if (fifo_empty(&output_buffer)) {
    return;
  }
  ready_to_send = 1;
  led_set_state(0, 1); //GREEN_LED = 0;
  while(!fifo_empty(&output_buffer));
  led_set_state(0, 0); //GREEN_LED = 0;
  ready_to_send = 0;
}

void serial_tx_str(const char *str) {
  while(*str != 0) {
    serial_tx_byte(*str);
    str++;
  }
  serial_flush();
}
