
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include "hardware.h"


volatile uint8_t P0_0;
volatile uint8_t P0_1;
volatile uint8_t P0_2;
volatile uint8_t P0_3;
volatile uint8_t P0_4;
volatile uint8_t P0_5;
volatile uint8_t P0DIR;
volatile uint8_t P1DIR;
volatile uint8_t P1SEL;
volatile uint8_t SYNC1;
volatile uint8_t SYNC0;
volatile uint8_t PKTLEN;
volatile uint8_t PKTCTRL1;
volatile uint8_t PKTCTRL0;
volatile uint8_t FSCTRL1;
volatile uint8_t FSCTRL0;
volatile uint8_t FSCTRL1;
volatile uint8_t MDMCFG4;
volatile uint8_t MDMCFG3;
volatile uint8_t MDMCFG2;
volatile uint8_t MDMCFG1;
volatile uint8_t MDMCFG0;
volatile uint8_t TEST1;
volatile uint8_t TEST0;
volatile uint8_t PA_TABLE1;
volatile uint8_t PA_TABLE0;
volatile uint8_t AGCCTRL2;
volatile uint8_t AGCCTRL1;
volatile uint8_t AGCCTRL0;
volatile uint8_t FREQ2;
volatile uint8_t FREQ1;
volatile uint8_t FREQ0;
volatile uint8_t CHANNR;
volatile uint8_t IEN2;
volatile uint8_t RFTXRXIE;
volatile uint8_t FSCAL3;
volatile uint8_t FSCAL2;
volatile uint8_t FSCAL1;
volatile uint8_t FREND1;
volatile uint8_t FREND0;
volatile uint8_t BSCFG;
volatile uint8_t DEVIATN;
volatile uint8_t MCSM2;
volatile uint8_t MCSM1;
volatile uint8_t MCSM0;
volatile uint8_t ADDR;
volatile uint8_t FOCCFG;
volatile uint8_t FSCAL0;
volatile uint8_t MARCSTATE;
volatile uint8_t RFD;
volatile uint8_t RSSI;
volatile uint8_t RFIF;
volatile uint8_t RFST;
volatile uint8_t S1CON;
volatile uint8_t TCON;
volatile uint8_t SLEEP    = 0b00000100;
volatile uint8_t CLKCON;
volatile uint8_t EA;
volatile uint8_t T1CTL;
volatile uint8_t T1CNTH;
volatile uint8_t T1CNTL;
volatile uint8_t T1CCTL0;
volatile uint8_t T1CC0H;
volatile uint8_t T1CC0L;
volatile uint8_t TIMIF;
volatile uint8_t OVFIM;
volatile uint8_t T1IE;
volatile uint8_t WDCTL;
volatile uint8_t PERCFG;
volatile uint8_t U1CSR;
volatile uint8_t U1BAUD;
volatile uint8_t U1GCR;
volatile uint8_t URX1IE;
volatile uint8_t IRCON2;
volatile uint8_t U1DBUF_write;
volatile uint8_t U1DBUF_read;

bool mock_hardware_should_exit;

// This is 10x normal clock speed
#define MOCK_CLOCK_TICK_RATE 0.0001

void *run_mock_hardware(void *vargp) {
  clock_t start_time;

  // Mark oscillator as powered up and stable
  SLEEP |= SLEEP_XOSC_S;

  printf("starting mock hardware thread\n");

  start_time = clock();

  while(!mock_hardware_should_exit) {

    double elapsed = ((double)(clock() - start_time))/CLOCKS_PER_SEC;

    if (elapsed > MOCK_CLOCK_TICK_RATE) {
      // Run counter
      if (T1CNTL == 255) {
        T1CNTH += 1;
      }
      T1CNTL += 1;
      t1_isr();
      start_time = clock();
    }

    // Watch radio strobe registers
    if(RFST == RFST_SIDLE) {
      MARCSTATE = MARC_STATE_IDLE;
      RFST = RFST_SNOP;
    }

    if(RFST == RFST_SRX) {
      MARCSTATE = MARC_STATE_RX;
      RFST = RFST_SNOP;
    }

    if(RFST == RFST_STX) {
      MARCSTATE = MARC_STATE_TX;
      RFST = RFST_SNOP;
    }
  }
  return NULL;
}
