
#include "statistics.h"

volatile uint16_t radio_rx_overflow_count;
volatile uint16_t radio_rx_buf_overflow_count;
uint16_t packet_rx_count;
uint16_t packet_tx_count;
uint16_t crc_failure_count;
volatile uint16_t spi_sync_failure_count;
