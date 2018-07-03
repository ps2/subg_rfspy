
#include <cc1110.h>
#include "subg_rfspy.h"

// All ISR functions must be present or included in the file that contains the
// function main, as described in SDCC manual section 3.8.1
void t1_isr(void) __interrupt T1_VECTOR;
void rftxrx_isr(void) __interrupt RFTXRX_VECTOR;
void rf_isr(void) __interrupt RF_VECTOR;

#ifdef USES_USART1_RX_ISR
void rx1_isr(void) __interrupt URX1_VECTOR;
#endif

#ifdef USES_USART1_TX_ISR
void tx1_isr(void) __interrupt UTX1_VECTOR;
#endif

#if USES_USB
void usb_isr() __interrupt 6;
#endif

int main(void)
{
  subg_rfspy_main();
}
