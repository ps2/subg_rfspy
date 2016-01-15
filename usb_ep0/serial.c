
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "hal.h"
#include "usb.h"
#include "usb.h"

extern int SERIAL_DATA_AVAILABLE = 0;

void configure_serial()
{

  setup_led( );
  GREEN_LED = 1;
  GREEN_LED = 0;
  usb_init( );
  usb_up( );
  /// GREEN_LED = 1;

}

uint8_t serial_rx_byte() {
  // return (uint8_t) usb_getc( );
  return (uint8_t) usb_getchar( );
} 

uint16_t serial_rx_word() {
  return (serial_rx_byte() << 8) + serial_rx_byte();
}

void serial_tx_byte(uint8_t tx_byte) {
  // return usb_putc(tx_byte);
  return usb_putchar((char) tx_byte);
}

void serial_tx_str(const char *str) {
  while(*str != 0) {
    serial_tx_byte(*str);
    str++;
  }
  serial_tx_byte(0);
}

