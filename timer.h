#ifndef TIMER_H
#define TIMER_H

volatile extern uint32_t timerCounter;

void init_timer();
void reset_timer();
void delay(uint32_t msec);

#endif
