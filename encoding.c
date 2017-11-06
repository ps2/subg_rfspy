
#include "encoding.h"
#include "manchester.h"
#include "fourbsixb.h"

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

static uint8_t passthrough_add_encoded_byte(DecoderState *state, uint8_t encoded) __reentrant {
  state->passthrough.data = encoded;
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
    case EncodingTypeFourbSixb:
      encoder->add_raw_byte = fourbsixb_add_raw_byte;
      encoder->next_encoded_byte = fourbsixb_next_encoded_byte;
      fourbsixb_init_encoder(state);
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
    case EncodingTypeFourbSixb:
      decoder->add_encoded_byte = fourbsixb_add_encoded_byte;
      decoder->next_decoded_byte = fourbsixb_next_decoded_byte;
      fourbsixb_init_decoder(state);
      break;
  }
}
