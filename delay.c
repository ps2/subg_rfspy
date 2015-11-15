
#include <stdint.h>
#include "hardware.h"

void delay(uint16_t usec)
{
  volatile uint16_t repeat = SYSTEM_CLOCK_MHZ*usec;
  while(repeat--);
}

