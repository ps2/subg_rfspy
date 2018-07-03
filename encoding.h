#ifndef ENCODING_H
#define ENCODING_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware.h"
#include "manchester_state.h"
#include "fourbsixb_state.h"

typedef enum {
  EncodingTypeNone = 0x0,
  EncodingTypeManchester = 0x1,
  EncodingTypeFourbSixb = 0x02,
  MaxEncodingTypeValue = EncodingTypeFourbSixb
} EncodingType;

typedef struct {
  union {
    struct {
      uint8_t data;
      uint8_t count;
    } passthrough;
    struct ManchesterEncoderState manchester;
    struct FourbSixbEncoderState fourbsixb;
  };
} EncoderState;

typedef struct {
  union {
    struct {
      uint8_t data;
      uint8_t count;
    } passthrough;
    struct ManchesterDecoderState manchester;
    struct FourbSixbDecoderState fourbsixb;
  };
} DecoderState;

typedef struct {
  // Adds a byte to be encoded
  void (*add_raw_byte)(EncoderState *state, uint8_t raw) __reentrant;
  // encoded will be set to next available encoded byte.
  // If flush is true, then encoder should return any partial bytes
  // Return value is 0 if no more bytes are available.
  uint8_t (*next_encoded_byte)(EncoderState *state, uint8_t *encoded, bool flush) __reentrant;
} Encoder;

typedef struct {
  // Adds a byte for decoding. The return value will be 1 if the byte
  // contains encoding errors, otherwise it will be 0.
  uint8_t (*add_encoded_byte)(DecoderState *state, uint8_t encoded) __reentrant;
  // decoded will be set to the next available decoded byte.
  // Return value is 0 if no more bytes are available.
  uint8_t (*next_decoded_byte)(DecoderState *state, uint8_t *decoded) __reentrant;
} Decoder;

void init_encoder(EncodingType encoding_type, Encoder *encoder, EncoderState *state);
void init_decoder(EncodingType encoding_type, Decoder *decoder, DecoderState *state);

#endif // ENCODING_H
