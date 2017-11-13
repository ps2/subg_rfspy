#ifndef MANCHESTER_STATE_H
#define MANCHESTER_STATE_H

struct ManchesterEncoderState {
  uint8_t output[2];
  uint8_t offset;
};

struct ManchesterDecoderState {
  uint8_t output;
  uint8_t bits_avail;
};

#endif // MANCHESTER_STATE_H
