/* Control a cc1110 for sub-ghz RF comms over uart. */

#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"

#ifndef NON_NATIVE_TEST
#include <cc1110.h>  // /usr/share/sdcc/include/mcs51/cc1110.h
#include "ioCCxx10_bitdef.h"
#endif


int main(void)
{
  // init LEDS
  P0DIR |= 0x03;
  GREEN_LED = 0;
  BLUE_LED = 0;

  configureRadio();
  configureUART();

  GREEN_LED = 0;
  BLUE_LED = 1;
  rx();
  return 0;
}
