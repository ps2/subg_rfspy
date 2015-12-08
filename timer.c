
#include <stdint.h>
#include "hardware.h"

volatile uint8_t timerCounter = 0;

void init_timer() {
   // Configure timer
  T1CTL = 0x0e;  // TickFreq/128, Free Running
  IEN1 |= 0x02;   // Enable Timer 1 interrupts
  TIMIF |= OVFIM; // Enable Timer 1 overflow interrupt mask
  T1CNTL = 0x00; // Clear counter low
  T1CC0H = 0xFF;
  T1CC0L = 0xFF;
// Set Timer 1 mode
  T1CCTL0 = 0x44;

  // Clear any pending Timer 1 Interrupt Flag
  IRCON &= ~0x02;

  // Start Timer 1
  //T1CTL = 0x0E;
}

void reset_timer() {
  timerCounter = 0;
}

void t1_isr(void) __interrupt T1_VECTOR
{
  timerCounter++;
  //GREEN_LED = !GREEN_LED;
}

