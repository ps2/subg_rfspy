#ifndef RADIO_H
#define RADIO_H

void configure_radio();

// Return values:
//  0 = timed out
//  1 = got packet
//  2 = rx interrupted by serial
uint8_t get_packet_and_write_to_serial(uint8_t channel, uint32_t timeout_ms);

void send_packet_from_serial(uint8_t channel, uint8_t repeat_count, uint8_t delay_ms);

// Used for retry
void resend_from_tx_buf(uint8_t channel);

#endif
