#ifndef MANCHESTER_H
#define MANCHESTER_H

#include "encoding.h"

void manchester_init_encoder(EncoderState *state);
void manchester_add_raw_byte(EncoderState *state, uint8_t raw) __reentrant;
uint8_t manchester_next_encoded_byte(EncoderState *state, uint8_t *encoded, bool flush) __reentrant;
void manchester_init_decoder(DecoderState *state);
uint8_t manchester_add_encoded_byte(DecoderState *state, uint8_t encoded) __reentrant;
uint8_t manchester_next_decoded_byte(DecoderState *state, uint8_t *decoded) __reentrant;

#endif //MANCHESTER_H
