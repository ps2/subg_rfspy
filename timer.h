#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __GNUC__
#define inline static inline
#endif


volatile extern uint32_t __timerCounter;

void init_timer();

void delay(uint32_t msec);

inline void read_timer(uint32_t *result) {
  EA = 0;
  *result = __timerCounter;
  EA = 1;
}

// Returns true if time has elapsed
inline bool check_elapsed(uint32_t start_time, uint32_t duration) {
  bool rval;
  EA = 0;
  rval = (__timerCounter - start_time) > duration;
  EA = 1;
  return rval;
}

#endif
