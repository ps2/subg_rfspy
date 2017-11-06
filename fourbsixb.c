
#include "fourbsixb.h"

//fileprivate let codesRev:Dictionary<Int, UInt8> = [21: 0, 49: 1, 50: 2, 35: 3, 52: 4, 37: 5, 38: 6, 22: 7, 26: 8, 25: 9, 42: 10, 11: 11, 44: 12, 13: 13, 14: 14, 28: 15]

static uint8_t __xdata codes[] = {21,49,50,35,52,37,38,22,26,25,42,11,44,13,14,28};

void fourbsixb_init_encoder(EncoderState *state) {
  state->fourbsixb.acc = 0;
  state->fourbsixb.bits_avail = 0;
}

void fourbsixb_add_raw_byte(EncoderState *state, uint8_t raw) __reentrant {
  uint16_t new_bits = (codes[raw >> 4] << 6) | codes[raw & 0xf];
  state->fourbsixb.acc = (state->fourbsixb.acc << 12) | new_bits;
  state->fourbsixb.bits_avail += 12;
}

uint8_t fourbsixb_next_encoded_byte(EncoderState *state, uint8_t *encoded) __reentrant {
  if (state->fourbsixb.bits_avail < 8) {
    return 0;
  }
  *encoded = state->fourbsixb.acc >> (8-state->fourbsixb.bits_avail);
  state->fourbsixb.bits_avail -= 8;
  return 1;
}

void fourbsixb_init_decoder(DecoderState *state) {
  state->fourbsixb.input_acc = 0;
  state->fourbsixb.input_bits_avail = 0;
  state->fourbsixb.output_acc = 0;
  state->fourbsixb.output_bits_avail = 0;
}

uint8_t fourbsixb_add_encoded_byte(DecoderState *state, uint8_t encoded) __reentrant {
  uint8_t code, i;
  state->fourbsixb.input_acc = (state->fourbsixb.input_acc << 8) | encoded;
  state->fourbsixb.input_bits_avail += 8;
  while (state->fourbsixb.input_bits_avail >= 6) {
    code = (state->fourbsixb.input_acc >> (state->fourbsixb.input_bits_avail - 6)) & 0b111111;
    for (i=0; i<16; i++) {
      if (codes[i] == code) {
        break;
      }
    }
    if (i == 16) {
      return 1; // Encoding error
    }
    state->fourbsixb.output_acc = (state->fourbsixb.output_acc << 4) | i;
    state->fourbsixb.output_bits_avail += 4;
  }
  return 0;
}

uint8_t fourbsixb_next_decoded_byte(DecoderState *state, uint8_t *decoded) __reentrant {
  if (state->fourbsixb.output_bits_avail < 8) {
    return 0;
  }
  *decoded = state->fourbsixb.output_acc >> (state->fourbsixb.output_bits_avail - 8);
  state->fourbsixb.output_bits_avail -= 8;
  return 1;
}
