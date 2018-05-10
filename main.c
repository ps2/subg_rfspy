/* Control a cc1110 for sub-ghz RF comms over uart. */

#include <stdint.h>
#include <stdio.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "timer.h"
#include "commands.h"

#ifdef MOCK_HARDWARE
#include <pthread.h>
#endif

// All ISR functions must be present or included in the file that contains the
// function main, as described in SDCC manual section 3.9.1
void t1_isr(void) __interrupt T1_VECTOR;
void rftxrx_isr(void) __interrupt RFTXRX_VECTOR;
void rf_isr(void) __interrupt RF_VECTOR;

#ifdef USES_USART1_RX_ISR
void rx1_isr(void) __interrupt URX1_VECTOR;
#endif

#ifdef USES_USART1_TX_ISR
void tx1_isr(void) __interrupt UTX1_VECTOR;
#endif

#if USES_USB
void usb_isr() __interrupt 6;
#endif

void main_init() {
  // Set the system clock source to HS XOSC and max CPU speed,
  // ref. [clk]=>[clk_xosc.c]
  printf("here1\n");
  SLEEP &= ~SLEEP_OSC_PD;
  while( !(SLEEP & SLEEP_XOSC_S) );
  CLKCON = (CLKCON & ~(CLKCON_CLKSPD | CLKCON_OSC)) | CLKSPD_DIV_1;
  while (CLKCON & CLKCON_OSC);
  SLEEP |= SLEEP_OSC_PD;

  printf("here2\n");
  init_leds();

  // Global interrupt enable
  printf("here3\n");
  init_timer();
  EA = 1;
  printf("here4\n");

  configure_serial();
  printf("here5\n");
  configure_radio();
  printf("here6\n");

  //LED test
  GREEN_LED_PIN = 1;
  delay(100);
  printf("here7\n");
  GREEN_LED_PIN = 0;
  BLUE_LED_PIN = 1;
  delay(100);
  printf("here8\n");
  BLUE_LED_PIN = 0;
}

void main_loop() {
  printf("here (main_loop)\n");
  while(1) {
    get_command();
  }
}

int main(void)
{
#ifdef MOCK_HARDWARE
   pthread_t mock_hardware_thread;
   pthread_create(&mock_hardware_thread, NULL, run_mock_hardware, NULL);
#endif

  main_init();

#ifdef RUN_TESTS
  pthread_t run_tests_thread;
  pthread_create(&run_tests_thread, NULL, run_tests, NULL);
#endif

  main_loop();
}
