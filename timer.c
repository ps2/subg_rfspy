
#include <stdint.h>
#include "hardware.h"
#include "timer.h"

volatile uint32_t __timerCounter = 0;

void init_timer() {
	union {
	uint8_t byte[2];
	unsigned short val;
	} t1cnt;

	// Configure timer to increment timerCounter once every 1ms
	// 1000 is scaling mhz to cycles/msec, 128 is T1CTL.DIV, 2 is TICKSPD
	uint16_t timer_ticks_per_ms = (SYSTEM_CLOCK_MHZ * 1000) / 128 / 2;

	__timerCounter = 0;

	T1CTL = 0x00; // disable timer
	T1CNTL = 0x00; // Clear counter low

	// Start timer in free running mode, no prescaler
	T1CTL = 0x01;

	do {
	t1cnt.byte[0] = T1CNTL;
	t1cnt.byte[1] = T1CNTH;
	} while (t1cnt.val <= 0xA000);

	// Stop timer
	T1CTL = 0x00;

	// Set period
	T1CC0H = timer_ticks_per_ms >> 8;
	T1CC0L = timer_ticks_per_ms & 0xff;

	T1CCTL0 = 0x44; // Enable int, set output on compare, compare mode, no capture
	TIMIF |= OVFIM; // Enable Timer 1 overflow interrupt mask
	T1IE = 1;
	EA = 1;

	T1CTL = 0x06;  // TickFreq/8, modulo
}

void t1_isr(void) __interrupt T1_VECTOR
{
  __timerCounter++;
}

void delay(uint32_t msec) {
  uint32_t start_time;
  read_timer(&start_time);
  while(!check_elapsed(start_time, msec)) {
    feed_watchdog();
  }
}
