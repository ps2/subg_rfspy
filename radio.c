
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "commands.h"


void configure_radio()
{
  /* RF settings SoC: CC1110 */
  SYNC1     = 0xFF; // sync word, high byte
  SYNC0     = 0x00; // sync word, low byte
  PKTLEN    = 0xFF; // packet length
  PKTCTRL1  = 0x00; // packet automation control
  PKTCTRL0  = 0x00; // packet automation control
  ADDR      = 0x00;
  CHANNR    = 0x02; // channel number
  FSCTRL1   = 0x06; // frequency synthesizer control
  FSCTRL0   = 0x00;
  FREQ2     = 0x26; // frequency control word, high byte
  FREQ1     = 0x2F; // frequency control word, middle byte
  FREQ0     = 0xE4; // frequency control word, low byte
  MDMCFG4   = 0xB9; // modem configuration
  MDMCFG3   = 0x66; // modem configuration
  MDMCFG2   = 0x33; // modem configuration
  MDMCFG1   = 0x61; // modem configuration
  MDMCFG0   = 0xe6; // modem configuration
  DEVIATN   = 0x15; // modem deviation setting
  MCSM2     = 0x07;
  MCSM1     = 0x30;
  MCSM0     = 0x18; // main radio control state machine configuration
  FOCCFG    = 0x17; // frequency offset compensation configuration BSCFG     = 0x6C;
  FREND1    = 0x56; // front end tx configuration
  FREND0    = 0x11; // front end tx configuration
  FSCAL3    = 0xE9; // frequency synthesizer calibration
  FSCAL2    = 0x2A; // frequency synthesizer calibration
  FSCAL1    = 0x00; // frequency synthesizer calibration
  FSCAL0    = 0x1F; // frequency synthesizer calibration
  TEST1     = 0x31; // various test settings
  TEST0     = 0x09; // various test settings
  PA_TABLE0 = 0x00; // needs to be explicitly set!
  PA_TABLE1 = 0x57; // pa power setting 0 dBm

  IEN2 |= IEN2_RFIE;
  RFTXRXIE = 1;
}

#define MAX_PACKET_LEN 192
volatile uint8_t __xdata buf[MAX_PACKET_LEN];
volatile uint8_t buf_idx = 0;
volatile uint8_t packet_count = 1;

void rftxrx_isr(void) __interrupt RFTXRX_VECTOR {
  uint8_t d_byte;
  if (MARCSTATE==MARC_STATE_RX) {
    d_byte = RFD;
    if (buf_idx == 0) {
      buf[0] = RSSI; 
      buf[1] = packet_count; 
      packet_count++;
      buf_idx = 2;
    }
    if (packet_count == 0) {
      packet_count = 1;
    }
    buf[buf_idx] = d_byte;
    buf_idx++;
    if (d_byte == 0) {
      RFST = RFST_SIDLE;
      while(MARCSTATE!=MARC_STATE_IDLE);
    }
  }
}

void get_packet() {

  uint8_t read_idx = 0;
  uint8_t d_byte = 0;

  RFST = RFST_SIDLE;
  while(MARCSTATE!=MARC_STATE_IDLE);

  RFST = RFST_SRX;
  while(MARCSTATE!=MARC_STATE_RX);

  // Waiting for isr to put radio bytes into buf
  // Also going to watch serial in case client wants to interrupt rx
  URX1IF = 0;

  while(1) {
    if (buf_idx > read_idx) {
      d_byte = buf[read_idx];
      serial_tx_byte(d_byte);
      read_idx++;
      if (read_idx > 1 && read_idx == buf_idx && d_byte == 0) {
        break;
      }
    }
    if (URX1IF) {
      // Got a byte on serial (interruption)
      interrupting_cmd = U1DBUF;
      RFST = RFST_SIDLE;
      return;
    }
  }
  buf_idx = 0;
  GREEN_LED = 1;
}

