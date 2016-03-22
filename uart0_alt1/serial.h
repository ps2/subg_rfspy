#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_DATA_AVAILABLE URX0IF

void configure_serial();
void serial_tx_byte(uint8_t);
void serial_tx_str(const char *str);
uint8_t serial_rx_byte();
uint32_t serial_rx_long();

#endif
