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

#define HARDWARE_FLOW_CONTROL_CONFIG 0xc0; /* 8N1, hw flow control, high stop bit */
#define HARDWARE_LED_INIT P0DIR |= BIT0|BIT1;
#define GREEN_LED P0_0
#define BLUE_LED P0_1
#define SYSTEM_CLOCK_MHZ 24

#elif EDISON_EXPLORER

#define HARDWARE_FLOW_CONTROL_CONFIG 0xc0; /* 8N1, hw flow control, high stop bit */
#define HARDWARE_LED_INIT P2DIR |= BIT3|BIT4;
#define GREEN_LED P2_3
#define BLUE_LED P2_4
#define SYSTEM_CLOCK_MHZ 24


#elif TI_DONGLE

#define HARDWARE_LED_INIT P1DIR |= 2;
#define GREEN_LED P1_1
#define BLUE_LED P1_1
#define SYSTEM_CLOCK_MHZ 24

#elif SRF_ERF

#define HARDWARE_FLOW_CONTROL_CONFIG 0x02; /* 8N1, NO flow control, high stop bit */
#define HARDWARE_LED_INIT P1DIR |= BIT7;
#define GREEN_LED P1_7
#define BLUE_LED P1_7
#define SYSTEM_CLOCK_MHZ 24
#elif SRF_STICK
#define HARDWARE_FLOW_CONTROL_CONFIG 0x02; /* 8N1, NO flow control, high stop bit */
#define HARDWARE_LED_INIT P1DIR |= BIT7;
#define GREEN_LED P1_7
#define BLUE_LED P1_6
#define SYSTEM_CLOCK_MHZ 24
#elif TI_MINIDEV
#define HARDWARE_FLOW_CONTROL_CONFIG 0xc0; /* 8N1, hw flow control, high stop bit */
#define HARDWARE_LED_INIT P1DIR |= BIT0|BIT1;
#define GREEN_LED P1_0
#define BLUE_LED P1_1
#define SYSTEM_CLOCK_MHZ 26
#endif

void led_set_mode(uint8_t led, uint8_t new_mode);

void led_set_state(uint8_t led, uint8_t command);

#endif
