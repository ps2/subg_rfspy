
#include <stdint.h>
#include <string.h>
#include "hardware.h"
#include "serial.h"
#include "commands.h"
#include "timer.h"
#include "encoding.h"
#include "fifo.h"
#include "radio.h"

#define RX_FIFO_SIZE 32
#define TX_BUF_SIZE 255

static volatile uint8_t __xdata radio_rx_buf[RX_FIFO_SIZE];
static fifo_buffer __xdata rx_fifo;
static uint8_t __xdata rx_len;

static volatile uint8_t __xdata radio_tx_buf[TX_BUF_SIZE];
static volatile uint8_t __xdata radio_tx_buf_read_idx;
static volatile uint8_t __xdata radio_tx_buf_len;

static volatile uint16_t __xdata preamble_word;
static volatile uint8_t stop_custom_preamble_semaphore;

// Error stats
volatile uint16_t __xdata rx_overflow;
volatile uint16_t __xdata tx_underflow;
volatile uint16_t __xdata packet_count;

// TX States

enum TxState {
  TxStatePreambleByte0 = 0x00,
  TxStatePreambleByte1 = 0x01,
  TxStatePreambleDefault = 0x02,
  TxStateSync0 = 0x03,
  TxStateSync1 = 0x04,
  TxStateData = 0x05,
  TxStateDone = 0x06,
};
volatile enum TxState tx_state;


EncodingType encoding_type = EncodingTypeNone;

void configure_radio()
{
  /* RF settings SoC: CC1110 */
  SYNC1     = 0xFF; // sync word, high byte
  SYNC0     = 0x00; // sync word, low byte
  PKTLEN    = 0xFF; // packet length
  PKTCTRL1  = 0x00; // packet automation control
  PKTCTRL0  = 0x00; // packet automation control
  ADDR      = 0x00;
  // CHANNR: See the locale-specific section below
  FSCTRL1   = 0x06; // frequency synthesizer control
  FSCTRL0   = 0x00;
  // FREQ0/FREQ1/FREQ2: See the locale-specific section below
  MDMCFG4   = 0x99; // 150.5 kHz rx filter bandwidth. Narrower can improve range,
                    // but then freq must be dialed in more tightly, which does not
                    // allow for variation we see with pump in free space vs on body.
  MDMCFG3   = 0x66; // modem configuration
  MDMCFG2   = 0x33; // modem configuration
  MDMCFG1   = 0x61; // modem configuration
  MDMCFG0   = 0x7E; // modem configuration
  DEVIATN   = 0x15; // modem deviation setting
  MCSM2     = 0x07;
  MCSM1     = 0x30;
  MCSM0     = 0x18; // main radio control state machine configuration
  FOCCFG    = 0x17; // frequency offset compensation configuration
  BSCFG     = 0x6C; // bit synchronization configuration
  FREND1    = 0xB6; // front end tx configuration
  FREND0    = 0x11; // front end tx configuration
  FSCAL3    = 0xE9; // frequency synthesizer calibration
  FSCAL2    = 0x2A; // frequency synthesizer calibration
  FSCAL1    = 0x00; // frequency synthesizer calibration
  FSCAL0    = 0x1F; // frequency synthesizer calibration
  TEST1     = 0x31; // various test settings
  TEST0     = 0x09; // various test settings
  PA_TABLE0 = 0x00; // needs to be explicitly set!
  // PA_TABLE1: See the locale-specific section below

  AGCCTRL2 = 0x07; // 0x03 to 0x07 - default: 0x03
  AGCCTRL1 = 0x00; // 0x00         - default: 0x40
  AGCCTRL0 = 0x91; // 0x91 or 0x92 - default: 0x91

#if US_RADIO_LOCALE
  FREQ2     = 0x26; // 916.541MHz is midpoint between freq of pump in free space,
  FREQ1     = 0x30; // and pump held close to the body.
  FREQ0     = 0x70; //
  CHANNR    = 0x02; // channel number
  PA_TABLE1 = 0xC0; // pa power setting 10 dBm
#else
  FREQ2     = 0x24; // frequency control word, high byte
  FREQ1     = 0x2E; // frequency control word, middle byte
  FREQ0     = 0x38; // frequency control word, low byte
  CHANNR    = 0x00; // channel number
  PA_TABLE1 = 0xC2; // Max configurable power output at this frequency is 0xC2
#endif

  IEN2 |= IEN2_RFIE;
  RFTXRXIE = 1;

  fifo_init(&rx_fifo, radio_rx_buf, RX_FIFO_SIZE);
}

// Set software based encoding
bool set_encoding_type(EncodingType new_type) {
  if (new_type <= MaxEncodingTypeValue) {
    encoding_type = new_type;
    return true;
  } else {
    return false;
  }
}

static void put_rx(uint8_t data) __reentrant {
  if (!fifo_put(&rx_fifo, data)) {
    rx_overflow++;
  }
}

void rftxrx_isr(void) __interrupt RFTXRX_VECTOR {
  uint8_t d_byte;
  if (MARCSTATE==MARC_STATE_RX) {
    d_byte = RFD;
    if (rx_len == 0) {
      put_rx(RSSI);
      put_rx(packet_count);
      packet_count++;
      rx_len = 2;
    }
    put_rx(d_byte);
    rx_len++;
  }
  else if (MARCSTATE==MARC_STATE_TX) {
    switch (tx_state) {
    case TxStatePreambleByte0:
      RFD = preamble_word >> 8;
      tx_state = TxStatePreambleByte1;
      break;
    case TxStatePreambleByte1:
      RFD = preamble_word & 0xff;
      if (stop_custom_preamble_semaphore) {
        tx_state = TxStateSync1;
      } else {
        tx_state = TxStatePreambleByte0;
      }
      break;
    case TxStateSync1:
      RFD = SYNC1;
      tx_state = TxStateSync0;
      break;
    case TxStateSync0:
      RFD = SYNC0;
      tx_state = TxStateData;
      break;
    case TxStateData:
      RFD = radio_tx_buf[radio_tx_buf_read_idx++];
      if (radio_tx_buf_read_idx >= radio_tx_buf_len) {
        tx_state = TxStateDone;
      }
      break;
    case TxStateDone:
      tx_underflow++;
      // Letting RFD go empty will make the radio stop TX mode.
      //RFD = 0;
      break;
    }

  }
}

void rf_isr(void) __interrupt RF_VECTOR {
  S1CON &= ~0x03; // Clear CPU interrupt flag
  if(RFIF & 0x80) // TX underflow
  {
    // Underflow
    RFST = RFST_SIDLE;
    RFIF &= ~0x80; // Clear module interrupt flag
  }
  else if(RFIF & 0x40) // RX overflow
  {
    RFIF &= ~0x40; // Clear module interrupt flag
  }
  else if(RFIF & 0x20) // RX timeout
  {
    RFIF &= ~0x20; // Clear module interrupt flag
  }
  // Use ”else if” to check and handle other RFIF flags

}

void send_packet_from_serial(uint8_t channel, uint8_t repeat_count, uint16_t delay_ms, uint16_t preamble_extend_ms, uint8_t len) {
  uint8_t s_byte;
  uint16_t send_count = 0;
  uint16_t total_send_count = repeat_count + 1;

  Encoder encoder;
  EncoderState encoder_state;

  init_encoder(encoding_type, &encoder, &encoder_state);

  mode_registers_enact(&tx_registers);

  CHANNR = channel;

  radio_tx_buf_len = 0;
  while (len > 0) {
    s_byte = serial_rx_byte();
    len--;
    encoder.add_raw_byte(&encoder_state, s_byte);
    while (encoder.next_encoded_byte(&encoder_state, &s_byte, len == 0)) {
      if (radio_tx_buf_len+1 < TX_BUF_SIZE) {
        radio_tx_buf[radio_tx_buf_len++] = s_byte;
      }
    }
  }

  while(send_count < total_send_count) {

    // delay
    if (send_count > 0 && delay_ms > 0) {
      delay(delay_ms);
    }

    send_from_tx_buf(channel, preamble_extend_ms);

    send_count++;
  }
}

void send_from_tx_buf(uint8_t channel, uint16_t preamble_extend_ms) {
  uint8_t pktlen_save;
  uint8_t mdmcfg2_save;
  uint8_t pktctrl0_save;

  mdmcfg2_save = MDMCFG2;
  pktlen_save = PKTLEN;
  pktctrl0_save = PKTCTRL0;


  if (preamble_word != 0) {
    // save and turn off preamble/sync registers
    MDMCFG2 &= 0b11111100;  // Disable PREAMBLE/SYNC
    PKTCTRL0 = (PKTCTRL0 & ~0b11) | 0b10; // Enter "infinite" tx mode
    PKTLEN = 0;
    stop_custom_preamble_semaphore = 0;
    tx_state = TxStatePreambleByte0;
  } else {
    if (preamble_extend_ms) {
      tx_state = TxStatePreambleDefault;
    } else {
      tx_state = TxStateData;
    }
    PKTLEN = radio_tx_buf_len;
  }

  RFST = RFST_SIDLE;
  while(MARCSTATE!=MARC_STATE_IDLE);

  CHANNR = channel;

  radio_tx_buf_read_idx = 0;

  // Turn on radio (interrupts will start again)
  RFST = RFST_STX;
  while(MARCSTATE!=MARC_STATE_TX);

  if (preamble_extend_ms > 0) {
    delay(preamble_extend_ms);
    if(preamble_word==0) {
      TCON |= 0b10;  // Manually trigger RFTXRX vector.
    }
  }

  if (preamble_word != 0) {
    stop_custom_preamble_semaphore = 1;
  } else {
    tx_state = TxStateData;
  }

  // wait for sending to finish
  while(MARCSTATE==MARC_STATE_TX);

  if (MARCSTATE==MARC_STATE_TX_UNDERFLOW) {
    RFST = RFST_SIDLE;
  }

  PKTLEN = pktlen_save;
  MDMCFG2 = mdmcfg2_save;
  PKTCTRL0 = pktctrl0_save;

}

uint8_t get_packet_and_write_to_serial(uint8_t channel, uint32_t timeout_ms, uint8_t use_pktlen) {

  uint8_t read_idx = 0;
  uint8_t d_byte = 0;
  uint8_t rval = 0;
  uint8_t encoding_error = 0;

  Decoder __xdata decoder;
  DecoderState __xdata decoder_state;

  reset_timer();

  mode_registers_enact(&rx_registers);

  init_decoder(encoding_type, &decoder, &decoder_state);

  RFST = RFST_SIDLE;
  while(MARCSTATE!=MARC_STATE_IDLE);

  CHANNR = channel;

  rx_len = 0;
  memset(radio_rx_buf, 0x11, RX_FIFO_SIZE);

  RFST = RFST_SRX;
  while(MARCSTATE!=MARC_STATE_RX);

  while(1) {
    // Waiting for isr to put radio bytes into rx_fifo
    if (!fifo_empty(&rx_fifo)) {

      d_byte = fifo_get(&rx_fifo);
      read_idx++;

      // Send status code
      if (read_idx == 1) {
        led_set_state(1, 1); //BLUE_LED;
        serial_tx_byte(RESPONSE_CODE_SUCCESS);
      }
      // First two bytes are rssi and packet #
      if (read_idx < 3) {
        serial_tx_byte(d_byte);
      } else {
        encoding_error = decoder.add_encoded_byte(&decoder_state, d_byte);

        while (decoder.next_decoded_byte(&decoder_state, &d_byte)) {
          serial_tx_byte(d_byte);
        }
      }

      if (encoding_error) {
        break;
      }

      // Check for end of packet
      if (use_pktlen && read_idx == PKTLEN) {
        break;
      }
    }

    if (timeout_ms > 0 && timerCounter > timeout_ms) {
      rval = RESPONSE_CODE_RX_TIMEOUT;
      break;
    }

    // Also going to watch serial in case the client wants to interrupt rx
    if (SERIAL_DATA_AVAILABLE) {
      // Received a byte from uart while waiting for radio packet
      // We will interrupt the RX and go handle the command.
      interrupting_cmd = serial_rx_byte();
      rval = RESPONSE_CODE_CMD_INTERRUPTED;
      break;
    }
  }

  RFST = RFST_SIDLE;
  led_set_state(1, 0); //BLUE_LED;
  return rval;
}

// Set software based preamble, 0 = disable
void radio_set_preamble(uint16_t p) {
  preamble_word = p;
}
