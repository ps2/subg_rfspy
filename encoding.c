
#include "encoding.h"

// Manchester encoder funcs

static void manchester_init_encoder(EncoderState *state) {
  state->manchester.offset = 2;
}

static void manchester_add_raw_byte(EncoderState *state, uint8_t raw) __reentrant {
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

static uint8_t manchester_next_encoded_byte(EncoderState *state, uint8_t *encoded) __reentrant {
  if (state->manchester.offset > 1) {
    return 0;
  }
  *encoded = state->manchester.output[state->manchester.offset];
  state->manchester.offset++;
  return 1;
}

// Manchester decoder funcs

static void manchester_init_decoder(DecoderState *state) {
  state->manchester.output = 0;
  state->manchester.bits_avail = 0;
}

static uint8_t manchester_add_encoded_byte(DecoderState *state, uint8_t raw) __reentrant {
  uint8_t acc = 0;
  int i;
  for (i=0; i<8; i++) {
    acc = (acc << 1);
    switch(raw & 0b11) {
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

static uint8_t manchester_next_decoded_byte(DecoderState *state, uint8_t *decoded) __reentrant {
  if (state->manchester.bits_avail < 8) {
    return 0;
  }
  *decoded = state->manchester.output;
  state->manchester.bits_avail = 0;
  return 1;
}

// Passthrough encoder
static void passthrough_init_encoder(EncoderState *state) {
  state->passthrough.count = 0;
}

static void passthrough_add_raw_byte(EncoderState *state, uint8_t raw) __reentrant {
  state->passthrough.count = 1;
  state->passthrough.data = raw;
}

static uint8_t passthrough_next_encoded_byte(EncoderState *state, uint8_t *encoded) __reentrant {
  if (state->passthrough.count < 1) {
    return 0;
  }
  *encoded = state->passthrough.data;
  state->passthrough.count--;
  return 1;
}

// passthrough decoder funcs
static void passthrough_init_decoder(DecoderState *state) {
  state->passthrough.count = 0;
}

static uint8_t passthrough_add_encoded_byte(DecoderState *state, uint8_t raw) __reentrant {
  state->passthrough.data = raw;
  state->passthrough.count = 1;
  return 0;
}

static uint8_t passthrough_next_decoded_byte(DecoderState *state, uint8_t *decoded) __reentrant {
  if (state->passthrough.count < 1) {
    return 0;
  }
  *decoded = state->passthrough.data;
  state->passthrough.count = 0;
  return 1;
}


// Init Encoder
void init_encoder(EncodingType encoding_type, Encoder *encoder, EncoderState *state) {
  switch (encoding_type) {
    case EncodingTypeNone:
      encoder->add_raw_byte = passthrough_add_raw_byte;
      encoder->next_encoded_byte = passthrough_next_encoded_byte;
      passthrough_init_encoder(state);
      break;
    case EncodingTypeManchester:
      encoder->add_raw_byte = manchester_add_raw_byte;
      encoder->next_encoded_byte = manchester_next_encoded_byte;
      manchester_init_encoder(state);
      break;
  }
}

// Init Decoder
void init_decoder(EncodingType encoding_type, Decoder *decoder, DecoderState *state) {
  switch (encoding_type) {
    case EncodingTypeNone:
      decoder->add_encoded_byte = passthrough_add_encoded_byte;
      decoder->next_decoded_byte = passthrough_next_decoded_byte;
      passthrough_init_decoder(state);
      break;
    case EncodingTypeManchester:
      decoder->add_encoded_byte = manchester_add_encoded_byte;
      decoder->next_decoded_byte = manchester_next_decoded_byte;
      manchester_init_decoder(state);
      break;
  }
}
