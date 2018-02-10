#ifndef FOURBSIXB_H
#define FOURBSIXB_H

#include "encoding.h"

void fourbsixb_init_encoder(EncoderState *state);
void fourbsixb_add_raw_byte(EncoderState *state, uint8_t raw) __reentrant;
uint8_t fourbsixb_next_encoded_byte(EncoderState *state, uint8_t *encoded, bool flush) __reentrant;
void fourbsixb_init_decoder(DecoderState *state);
uint8_t fourbsixb_add_encoded_byte(DecoderState *state, uint8_t raw) __reentrant;
uint8_t fourbsixb_next_decoded_byte(DecoderState *state, uint8_t *decoded) __reentrant;

#endif //FOURBSIXB_H
