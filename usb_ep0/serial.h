#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_DATA_AVAILABLE serial_has_bytes( )

void configure_serial();
void serial_tx_byte(uint8_t);
void serial_tx_str(const char *str);
void run_command_from_serial();
uint8_t serial_rx_byte();
uint8_t serial_has_bytes();
uint16_t serial_rx_word();
void flush_serial( );

#endif
