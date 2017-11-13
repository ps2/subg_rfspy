
#include "manchester.h"

void manchester_init_encoder(EncoderState *state) {
  state->manchester.offset = 2;
}

void manchester_add_raw_byte(EncoderState *state, uint8_t raw) __reentrant {
  uint16_t acc = 0;
  int i;
  for (i=0; i<8; i++) {
    acc = (acc << 2) + 1 + (raw >> 7);
    raw = raw << 1;
  }
  state->manchester.output[0] = acc >> 8;
  state->manchester.output[1] = acc & 0xff;
  state->manchester.offset = 0;
}

uint8_t manchester_next_encoded_byte(EncoderState *state, uint8_t *encoded) __reentrant {
  if (state->manchester.offset > 1) {
    return 0;
  }
  *encoded = state->manchester.output[state->manchester.offset];
  state->manchester.offset++;
  return 1;
}

// Manchester decoder funcs

void manchester_init_decoder(DecoderState *state) {
  state->manchester.output = 0;
  state->manchester.bits_avail = 0;
}

uint8_t manchester_add_encoded_byte(DecoderState *state, uint8_t encoded) __reentrant {
  uint8_t acc = 0;
  int i;
  for (i=0; i<8; i++) {
    acc = (acc << 1);
    switch(encoded & 0b11) {
      case 0b00:
      case 0b11:
        return 1;  // Encoding error
      case 0b10:
        acc++;
    }
  }
  state->manchester.output = (state->manchester.output << 4) + acc;
  state->manchester.bits_avail += 4;
  return 0;
}

uint8_t manchester_next_decoded_byte(DecoderState *state, uint8_t *decoded) __reentrant {
  if (state->manchester.bits_avail < 8) {
    return 0;
  }
  *decoded = state->manchester.output;
  state->manchester.bits_avail = 0;
  return 1;
}
