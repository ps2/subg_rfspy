/* Control a cc1110 for sub-ghz RF comms over uart. */

#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "timer.h"
#include "commands.h"
#include "delay.h"

// SDCC needs prototypes of all ISR functions in main. not sure why, but described in section 3.8.1
void t1_isr(void) __interrupt T1_VECTOR;
void rftxrx_isr(void) __interrupt RFTXRX_VECTOR;
void rf_isr(void) __interrupt RF_VECTOR;

#ifdef USES_USART1_RX_ISR
void rx1_isr(void) __interrupt URX1_VECTOR;
#endif 

#ifdef USES_USART1_TX_ISR
void tx1_isr(void) __interrupt UTX1_VECTOR;
#endif

#if TI_DONGLE || SRF_STICK
void usb_isr() __interrupt 6;
#endif

int main(void)
{

  // Set the system clock source to HS XOSC and max CPU speed,
  // ref. [clk]=>[clk_xosc.c]
  SLEEP &= ~SLEEP_OSC_PD;
  while( !(SLEEP & SLEEP_XOSC_S) );
  CLKCON = (CLKCON & ~(CLKCON_CLKSPD | CLKCON_OSC)) | CLKSPD_DIV_1;
  while (CLKCON & CLKCON_OSC);
  SLEEP |= SLEEP_OSC_PD;


  // init LEDS
  HARDWARE_LED_INIT;       // see hardware.h
  led_set_state(0, 0); //GREEN_LED = 0;
  led_set_state(1, 0); //BLUE_LED = 0;

  // Global interrupt enable
  init_timer();
  EA = 1;

/*  //LED test
  led_set_state(0, 1); //GREEN_LED = 1;
  delay(1000);
  led_set_state(0, 0); //GREEN_LED = 0;
  led_set_state(1, 1); //BLUE_LED = 1;
  delay(1000);
  led_set_state(1, 0); //BLUE_LED = 0;
*/
  configure_radio();
  configure_serial();

  while(1) {
    //led_set_state(0,2);
    get_command();
  }
}
