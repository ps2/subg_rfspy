#ifndef MOCK_HARDWARE_H
#define MOCK_HARDWARE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __GNUC__
#define inline static inline
#endif

#define __reentrant
#define __xdata
#define __interrupt
#define RFTXRX_VECTOR
#define RF_VECTOR
#define T1_VECTOR
#define URX1_VECTOR
#define UTX1_VECTOR

extern volatile uint8_t P0_0;
extern volatile uint8_t P0_1;
extern volatile uint8_t P0DIR;
extern volatile uint8_t P1DIR;
extern volatile uint8_t P1SEL;
extern volatile uint8_t SYNC1;
extern volatile uint8_t SYNC0;
extern volatile uint8_t PKTLEN;
extern volatile uint8_t PKTCTRL1;
extern volatile uint8_t PKTCTRL0;
extern volatile uint8_t FSCTRL1;
extern volatile uint8_t FSCTRL0;
extern volatile uint8_t FSCTRL1;
extern volatile uint8_t MDMCFG4;
extern volatile uint8_t MDMCFG3;
extern volatile uint8_t MDMCFG2;
extern volatile uint8_t MDMCFG1;
extern volatile uint8_t MDMCFG0;
extern volatile uint8_t TEST1;
extern volatile uint8_t TEST0;
extern volatile uint8_t PA_TABLE1;
extern volatile uint8_t PA_TABLE0;
extern volatile uint8_t AGCCTRL2;
extern volatile uint8_t AGCCTRL1;
extern volatile uint8_t AGCCTRL0;
extern volatile uint8_t FREQ2;
extern volatile uint8_t FREQ1;
extern volatile uint8_t FREQ0;
extern volatile uint8_t CHANNR;
extern volatile uint8_t IEN2;
extern volatile uint8_t IEN2_RFIE;
extern volatile uint8_t RFTXRXIE;
extern volatile uint8_t FSCAL3;
extern volatile uint8_t FSCAL2;
extern volatile uint8_t FSCAL1;
extern volatile uint8_t FREND1;
extern volatile uint8_t FREND0;
extern volatile uint8_t BSCFG;
extern volatile uint8_t DEVIATN;
extern volatile uint8_t MCSM2;
extern volatile uint8_t MCSM1;
extern volatile uint8_t MCSM0;
extern volatile uint8_t ADDR;
extern volatile uint8_t FOCCFG;
extern volatile uint8_t FSCAL0;
extern volatile uint8_t MARCSTATE;
extern volatile uint8_t RFD;
extern volatile uint8_t RSSI;
extern volatile uint8_t RFIF;
extern volatile uint8_t RFST;
extern volatile uint8_t S1CON;
extern volatile uint8_t TCON;
extern volatile uint8_t SLEEP;
extern volatile uint8_t CLKCON;
extern volatile uint8_t EA;
extern volatile uint8_t T1CTL;
extern volatile uint8_t T1CNTH;
extern volatile uint8_t T1CNTL;
extern volatile uint8_t T1CCTL0;
extern volatile uint8_t T1CC0H;
extern volatile uint8_t T1CC0L;
extern volatile uint8_t TIMIF;
extern volatile uint8_t OVFIM;
extern volatile uint8_t T1IE;
extern volatile uint8_t WDCTL;
extern volatile uint8_t PERCFG;
extern volatile uint8_t U1CSR;
extern volatile uint8_t U1BAUD;
extern volatile uint8_t U1GCR;
extern volatile uint8_t URX1IE;
extern volatile uint8_t IRCON2;
extern volatile uint8_t U1DBUF_write;
extern volatile uint8_t U1DBUF_read;

#define GREEN_LED_PIN P0_0
#define BLUE_LED_PIN P0_1
#define SYSTEM_CLOCK_MHZ 24
#define HARDWARE_LED_INIT P0DIR |= BIT0|BIT1;

void *run_tests(void *vargp);
void *run_mock_hardware(void *vargp);

void t1_isr(void);  // Timer Interrupt
void tx1_isr(void); // UTX1_VECTOR
void rx1_isr(void); // URX1_VECTOR

extern bool mock_hardware_should_exit;

#endif
