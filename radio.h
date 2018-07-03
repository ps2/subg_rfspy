#ifndef RADIO_H
#define RADIO_H

#include "encoding.h"
#include "stdbool.h"

void configure_radio();

// Return values:
//  0 = timed out
//  1 = got packet
//  2 = rx interrupted by serial
uint8_t get_packet_and_write_to_serial(uint8_t channel, uint32_t timeout_ms, uint8_t use_pktlen);

void send_packet_from_serial(uint8_t channel, uint8_t repeat_count, uint16_t delay_ms, uint16_t preamble_extend_ms, uint8_t len);

void send_from_tx_buf(uint8_t channel, uint16_t preamble_extend_ms);

// Set software based encoding. Returns false if encoding type is not supported
bool set_encoding_type(EncodingType encoding_type);

// Set software based preamble, 0 = disable
void radio_set_preamble(uint16_t preamble_word);

#endif
