#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "interrupts.h"

volatile extern uint8_t timerCounter;

void initTimer();
void resetTimer();

#endif

