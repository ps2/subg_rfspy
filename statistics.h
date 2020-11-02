#ifndef STATISTICS_H
#define STATISTICS_H

#include <stdint.h>

volatile extern uint16_t radio_rx_overflow_count;
volatile extern uint16_t radio_rx_buf_overflow_count;
volatile extern uint16_t serial_rx_overflow_count;
extern uint16_t packet_tx_count;
extern uint16_t packet_rx_count;
extern uint16_t crc_failure_count;
volatile extern uint16_t spi_sync_failure_count;

#endif //STATISTICS_H
