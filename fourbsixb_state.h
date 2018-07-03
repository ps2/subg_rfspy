#ifndef FOURBSIXB_STATE_H
#define FOURBSIXB_STATE_H

struct FourbSixbEncoderState {
  uint16_t acc;
  uint8_t bits_avail;
};

struct FourbSixbDecoderState {
  uint16_t input_acc;
  uint8_t input_bits_avail;
  uint16_t output_acc;
  uint8_t output_bits_avail;
};

#endif // FOURBSIXB_STATE_H
