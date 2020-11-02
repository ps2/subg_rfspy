#ifndef HARDWARE_H
#define HARDWARE_H

#ifdef MOCK_HARDWARE
#include "mock_hardware.h"
#else
#include <cc1110.h>  // /usr/share/sdcc/include/mcs51/cc1110.h
#define U1DBUF_write U1DBUF
#define U1DBUF_read U1DBUF
#endif

#include "ioCCxx10_bitdef.h"

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
#define HARDWARE_LED_INIT P0DIR |= BIT0|BIT1|BIT2; // Set P0 0,1,2 as outputs
#define GREEN_LED_PIN P0_0
#define BLUE_LED_PIN P0_1
#define SYSTEM_CLOCK_MHZ 24
#define RESPONSE_AVAILABLE_SIGNAL_PIN P0_2

#elif EDISON_EXPLORER

#define HARDWARE_FLOW_CONTROL_CONFIG 0xc0; /* 8N1, hw flow control, high stop bit */
#define HARDWARE_LED_INIT P2DIR |= BIT3|BIT4;
#define GREEN_LED_PIN P2_3
#define BLUE_LED_PIN P2_4
#define SYSTEM_CLOCK_MHZ 24


#elif TI_DONGLE

#define HARDWARE_LED_INIT P1DIR |= 2;
#define GREEN_LED_PIN P1_1
#define BLUE_LED_PIN P1_1
#define SYSTEM_CLOCK_MHZ 24

#elif SRF_ERF

#define HARDWARE_FLOW_CONTROL_CONFIG 0x02; /* 8N1, NO flow control, high stop bit */
#define HARDWARE_LED_INIT P1DIR |= BIT7;
#define GREEN_LED_PIN P1_7
#define BLUE_LED_PIN P1_7
#define SYSTEM_CLOCK_MHZ 24
#elif SRF_STICK
#define HARDWARE_FLOW_CONTROL_CONFIG 0x02; /* 8N1, NO flow control, high stop bit */
#define HARDWARE_LED_INIT P1DIR |= BIT7;
#define GREEN_LED_PIN P1_7
#define BLUE_LED_PIN P1_6
#define SYSTEM_CLOCK_MHZ 24
#elif TI_MINIDEV
#define HARDWARE_FLOW_CONTROL_CONFIG 0xc0; /* 8N1, hw flow control, high stop bit */
#define HARDWARE_LED_INIT P1DIR |= BIT0|BIT1;
#define GREEN_LED_PIN P1_0
#define BLUE_LED_PIN P1_1
#define SYSTEM_CLOCK_MHZ 26
#endif

#define MAX_MODE_REGISTERS 8

typedef struct register_setting {
	uint8_t	addr;
	uint8_t	value;
} register_setting;

typedef struct mode_registers {
  uint8_t count;
  register_setting registers[MAX_MODE_REGISTERS];
} mode_registers;

typedef enum {
  GreenLED = 0x00,
  BlueLED = 0x01
} LEDNumber;

typedef enum {
	LEDModeOff = 0x00,
	LEDModeOn = 0x01,
	LEDModeDiagnostic = 0x02
} LEDMode;

typedef enum LEDState {
	LEDStateOff = 0x00,
	LEDStateOn = 0x01
} LEDState;

void init_leds();
void led_set_mode(LEDNumber led, LEDMode new_mode);
void led_set_diagnostic(LEDNumber led, LEDState state);

uint8_t get_register(uint8_t addr);
uint8_t set_register(uint8_t addr, uint8_t value);

void mode_registers_clear(mode_registers *mode);
void mode_registers_add(mode_registers *mode, uint8_t addr, uint8_t value);
void mode_registers_enact(mode_registers const *mode);

extern mode_registers __xdata tx_registers;
extern mode_registers __xdata rx_registers;

#define WDCLP1     0xA0  // Clear pattern 1
#define WDCLP2     0x50  // Clear pattern 2

inline void feed_watchdog() {
	WDCTL = WDCLP1 | WDCTL_EN;
	WDCTL = WDCLP2 | WDCTL_EN;
}

#endif
