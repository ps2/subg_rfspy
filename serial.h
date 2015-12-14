#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_DATA_AVAILABLE URX1IF

void configure_uart();
void serial_tx_byte(uint8_t);
void serial_tx_buf(uint8_t *buf, uint8_t len);
void run_command_from_serial();
uint8_t serial_rx_byte();

#endif
