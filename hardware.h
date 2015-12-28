#ifndef HARDWARE_H
#define HARDWARE_H

#ifndef NON_NATIVE_TEST
#include <cc1110.h>  // /usr/share/sdcc/include/mcs51/cc1110.h
#include "ioCCxx10_bitdef.h"
#endif

#define BIT0 0x1
#define BIT1 0x2
#define BIT2 0x4
#define BIT3 0x8
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

#ifdef RILEYLINK
#define HARDWARE_LED_INIT P0DIR |= BIT0|BIT1;
#define GREEN_LED P0_0
#define BLUE_LED P0_1
#define SYSTEM_CLOCK_MHZ 24
#elif SRF_ERF
#define HARDWARE_LED_INIT P1DIR |= BIT7;
#define GREEN_LED P1_7
#define BLUE_LED P1_7
#define SYSTEM_CLOCK_MHZ 24
#endif


#endif
