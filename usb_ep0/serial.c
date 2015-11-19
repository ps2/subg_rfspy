
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "hal.h"
#include "usb.h"

// extern int SERIAL_DATA_AVAILABLE = 0;

void configure_serial()
{

  setup_led( );
  GREEN_LED = 1;
  GREEN_LED = 0;
  usb_init( );
  usb_enable( );
  usb_up( );
  usb_flush( );
  /// GREEN_LED = 1;

}

uint8_t serial_has_bytes()
{

  usb_flush( );
  if (usb_pollchar() == USB_READ_AGAIN) {
    GREEN_LED = 0;
    return 0;
  }
  GREEN_LED = 1;
  return 1;
}


uint8_t serial_rx_byte() {
  // return (uint8_t) usb_getc( );
  char c =  usb_getchar( );
  return (uint8_t) c;
} 

uint16_t serial_rx_word() {
  return (serial_rx_byte() << 8) + serial_rx_byte();
}

uint32_t serial_rx_long() {
  return ((uint32_t)serial_rx_word() << 16) + serial_rx_word();
}


void flush_serial( ) {
  GREEN_LED ^= 1;
  usb_flush( );
  GREEN_LED ^= 1;
}

void serial_tx_byte(uint8_t tx_byte) {
  // return usb_putc(tx_byte);
  usb_putchar(  tx_byte);
  usb_flush( );
}

void serial_tx_str(const char *str) {
  while(*str != 0) {
    serial_tx_byte(*str);
    str++;
  }
  serial_tx_byte(0);
  usb_flush( );
}

