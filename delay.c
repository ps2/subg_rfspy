
#include <stdint.h>
#include "hardware.h"

// This factor is based on the particular instructions that
// sdcc generates for the loops
#define CYCLES_PER_LOOP 18

void delay(uint8_t msec)
{
  volatile uint8_t mhz_ctr;
  volatile uint8_t adj_ctr;
  while(msec--){
    adj_ctr = 1000 /* usec to msec */ / CYCLES_PER_LOOP;
    while(adj_ctr--){
      mhz_ctr = SYSTEM_CLOCK_MHZ;
      while(mhz_ctr--);
    }
  }
}

