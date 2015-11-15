#ifndef RADIO_H
#define RADIO_H

void configure_radio();
void get_packet_and_write_to_serial();
void send_packet_from_serial(uint8_t repeat_count, uint16_t repeat_delay);

#endif
