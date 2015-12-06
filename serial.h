#ifndef SERIAL_H
#define SERIAL_H

void configure_uart();
void serial_rx();
void serial_tx_byte(uint8_t);
void serial_tx_buf(uint8_t *buf, uint8_t len);

#endif
