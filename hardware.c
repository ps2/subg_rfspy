/* Control a cc1110 for sub-ghz RF comms over uart. */

#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "timer.h"
#include "commands.h"

static LEDMode green_mode = 0;
static LEDMode blue_mode = 0;

mode_registers __xdata tx_registers;
mode_registers __xdata rx_registers;

void init_gpios() {
	// init LEDS
	HARDWARE_LED_INIT;       // see hardware.h
	GREEN_LED_PIN = 0;
	BLUE_LED_PIN = 0;

  led_set_mode(GreenLED, LEDModeOff);
  led_set_mode(BlueLED, LEDModeOff);

	RESPONSE_AVAILABLE_SIGNAL_PIN = 0;
	DEBUG_PIN1 = 0;
	DEBUG_PIN2 = 0;
	DEBUG_PIN3 = 0;
}

void led_set_mode(LEDNumber led, LEDMode new_mode)
{
	if(led == GreenLED){
		green_mode = new_mode;
		if(new_mode == LEDModeOn) {
			GREEN_LED_PIN = LEDStateOn;
		} else if (new_mode == LEDModeOff) {
			GREEN_LED_PIN = LEDStateOff;
		}
	}
	else if(led == BlueLED){
		blue_mode = new_mode;
		if(new_mode == LEDModeOn) {
			BLUE_LED_PIN = LEDStateOn;
		} else if (new_mode == LEDModeOff) {
			BLUE_LED_PIN = LEDStateOff;
		}
	}
}

void led_set_diagnostic(LEDNumber led, LEDState state)
{
	if(led == GreenLED){
		if(green_mode == LEDModeDiagnostic){
			GREEN_LED_PIN = state;
		}
	}
	else if(led == BlueLED){
		if(blue_mode == LEDModeDiagnostic){
			BLUE_LED_PIN = state;
		}
	}
}

uint8_t get_register(uint8_t addr) {
  uint8_t value;
  switch(addr) {
    case 0x00:
      value = SYNC1;
      break;
    case 0x01:
      value = SYNC0;
      break;
    case 0x02:
      value = PKTLEN;
      break;
    case 0x03:
      value = PKTCTRL1;
      break;
    case 0x04:
      value = PKTCTRL0;
      break;
    case 0x05:
      value = ADDR;
      break;
    case 0x06:
      value = CHANNR;
      break;
    case 0x07:
      value = FSCTRL1;
      break;
    case 0x08:
      value = FSCTRL0;
      break;
    case 0x09:
      value = FREQ2;
      break;
    case 0x0A:
      value = FREQ1;
      break;
    case 0x0B:
      value = FREQ0;
      break;
    case 0x0C:
      value = MDMCFG4;
      break;
    case 0x0D:
      value = MDMCFG3;
      break;
    case 0x0E:
      value = MDMCFG2;
      break;
    case 0x0F:
      value = MDMCFG1;
      break;
    case 0x10:
      value = MDMCFG0;
      break;
    case 0x11:
      value = DEVIATN;
      break;
    case 0x12:
      value = MCSM2;
      break;
    case 0x13:
      value = MCSM1;
      break;
    case 0x14:
      value = MCSM0;
      break;
    case 0x15:
      value = FOCCFG;
      break;
    case 0x16:
      value = BSCFG;
      break;
    case 0x17:
      value = AGCCTRL2;
      break;
    case 0x18:
      value = AGCCTRL1;
      break;
    case 0x19:
      value = AGCCTRL0;
      break;
    case 0x1A:
      value = FREND1;
      break;
    case 0x1B:
      value = FREND0;
      break;
    case 0x1C:
      value = FSCAL3;
      break;
    case 0x1D:
      value = FSCAL2;
      break;
    case 0x1E:
      value = FSCAL1;
      break;
    case 0x1F:
      value = FSCAL0;
      break;
    case 0x24:
      value = TEST1;
      break;
    case 0x25:
      value = TEST0;
      break;
    case 0x2D:
      value = PA_TABLE1;
      break;
    case 0x2E:
      value = PA_TABLE0;
      break;
    default:
      value = 0x5A;
  }
	return value;
}

uint8_t set_register(uint8_t addr, uint8_t value) {
  uint8_t rval;
  rval = RESPONSE_CODE_SUCCESS;
  switch(addr) {
    case 0x00:
      SYNC1 = value;
      break;
    case 0x01:
      SYNC0 = value;
      break;
    case 0x02:
      PKTLEN = value;
      break;
    case 0x03:
      PKTCTRL1 = value;
      break;
    case 0x04:
      PKTCTRL0 = value;
      break;
    case 0x05:
      ADDR = value;
      break;
    case 0x06:
      CHANNR = value;
      break;
    case 0x07:
      FSCTRL1 = value;
      break;
    case 0x08:
      FSCTRL0 = value;
      break;
    case 0x09:
      FREQ2 = value;
      break;
    case 0x0A:
      FREQ1 = value;
      break;
    case 0x0B:
      FREQ0 = value;
      break;
    case 0x0C:
      MDMCFG4 = value;
      break;
    case 0x0D:
      MDMCFG3 = value;
      break;
    case 0x0E:
      MDMCFG2 = value;
      break;
    case 0x0F:
      MDMCFG1 = value;
      break;
    case 0x10:
      MDMCFG0 = value;
      break;
    case 0x11:
      DEVIATN = value;
      break;
    case 0x12:
      MCSM2 = value;
      break;
    case 0x13:
      MCSM1 = value;
      break;
    case 0x14:
      MCSM0 = value;
      break;
    case 0x15:
      FOCCFG = value;
      break;
    case 0x16:
      BSCFG = value;
      break;
    case 0x17:
      AGCCTRL2 = value;
      break;
    case 0x18:
      AGCCTRL1= value;
      break;
    case 0x19:
      AGCCTRL0 = value;
      break;
    case 0x1A:
      FREND1 = value;
      break;
    case 0x1B:
      FREND0 = value;
      break;
    case 0x1C:
      FSCAL3 = value;
      break;
    case 0x1D:
      FSCAL2 = value;
      break;
    case 0x1E:
      FSCAL1 = value;
      break;
    case 0x1F:
      FSCAL0 = value;
      break;
    case 0x24:
      TEST1 = value;
      break;
    case 0x25:
      TEST0 = value;
      break;
    case 0x2D:
      PA_TABLE1 = value;
      break;
    case 0x2E:
      PA_TABLE0 = value;
      break;
    default:
      rval = RESPONSE_CODE_PARAM_ERROR;
  }
	return rval;
}

void mode_registers_clear(mode_registers *mode) {
	mode->count = 0;
}

void mode_registers_add(mode_registers *mode, uint8_t addr, uint8_t value) {
	if (mode->count < MAX_MODE_REGISTERS - 1) {
		mode->registers[mode->count].addr = addr;
		mode->registers[mode->count].value = value;
		mode->count++;
	}
}

void mode_registers_enact(mode_registers const *mode) {
  int i;
	for (i=0; i<mode->count; i++) {
		set_register(mode->registers[i].addr, mode->registers[i].value);
	}
}
