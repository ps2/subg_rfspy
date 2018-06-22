/* Control a cc1110 for sub-ghz RF comms over uart. */

#include <stdint.h>
#include <stdio.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "timer.h"
#include "commands.h"
#include "subg_rfspy.h"

bool __xdata subg_rfspy_init_finished;
bool __xdata subg_rfspy_should_exit;

void subg_rfspy_main() {
  // Set the system clock source to HS XOSC and max CPU speed,
  // ref. [clk]=>[clk_xosc.c]
  SLEEP &= ~SLEEP_OSC_PD;
  while( !(SLEEP & SLEEP_XOSC_S) );
  CLKCON = (CLKCON & ~(CLKCON_CLKSPD | CLKCON_OSC)) | CLKSPD_DIV_1;
  while (CLKCON & CLKCON_OSC);
  SLEEP |= SLEEP_OSC_PD;

  init_leds();

  // Global interrupt enable
  init_timer();
  EA = 1;

  configure_serial();
  configure_radio();

  //LED test
  GREEN_LED_PIN = 1;
  delay(100);
  GREEN_LED_PIN = 0;
  BLUE_LED_PIN = 1;
  delay(100);
  BLUE_LED_PIN = 0;

  subg_rfspy_init_finished = true;

  while(!subg_rfspy_should_exit) {
    get_command();
  }
}
