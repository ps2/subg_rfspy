
#include <stdint.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "commands.h"

uint8_t interrupting_cmd = 0;

typedef void (*CommandHandler)();

CommandHandler handlers[] = {
  /* 0 */ 0,
  /* 1 */ cmd_get_state,
  /* 2 */ cmd_get_version,
  /* 3 */ cmd_get_packet,
  /* 4 */ cmd_send_packet,
  /* 5 */ cmd_send_and_listen,
  /* 6 */ cmd_update_register,
  /* 7 */ cmd_reset
};

void cmd_get_packet() {
  uint8_t channel;
  uint32_t timeout_ms;
  uint8_t result;
  channel = serial_rx_byte();
  timeout_ms = serial_rx_long();
  result = get_packet_and_write_to_serial(channel, timeout_ms);
  if (result != 0) {
    serial_tx_byte(result);
    serial_tx_byte(0);
  }
}

void cmd_get_state() {
  serial_tx_str("OK");
}

void cmd_get_version() {
  serial_tx_str("subg_rfspy 0.7");
}

void do_cmd(uint8_t cmd) {
  if (cmd > 0 && cmd < sizeof(handlers)/sizeof(handlers[0])) {
    handlers[cmd]();
  }
}

void get_command() {
  uint8_t cmd;
  cmd = serial_rx_byte();
  do_cmd(cmd);
  if (interrupting_cmd) {
    do_cmd(interrupting_cmd);
    interrupting_cmd = 0;
  }
}

void cmd_send_packet() {
  uint8_t channel;
  uint8_t repeat_count;
  uint8_t delay_ms;
  channel = serial_rx_byte();
  repeat_count = serial_rx_byte();
  delay_ms = serial_rx_byte();
  send_packet_from_serial(channel, repeat_count, delay_ms);
  serial_tx_byte(0);
}

/* Combined send and receive */
void cmd_send_and_listen() {
  uint8_t send_channel;
  uint8_t repeat_count;
  uint8_t delay_ms;
  uint8_t listen_channel;
  uint32_t timeout_ms;
  uint8_t retry_count;
  uint8_t result;

  send_channel = serial_rx_byte();
  repeat_count = serial_rx_byte();
  delay_ms = serial_rx_byte();
  listen_channel = serial_rx_byte();
  timeout_ms = serial_rx_long();
  retry_count = serial_rx_byte();

  send_packet_from_serial(send_channel, repeat_count, delay_ms);
  result = get_packet_and_write_to_serial(listen_channel, timeout_ms);

  while (result == ERROR_RX_TIMEOUT && retry_count > 0) {
    resend_from_tx_buf(send_channel);
    result = get_packet_and_write_to_serial(listen_channel, timeout_ms);
    retry_count--;
  }

  if (result != 0) {
    // Error, and no retries left
    serial_tx_byte(result);
    serial_tx_byte(0);
  }
}

void cmd_update_register() {
  uint8_t addr;
  uint8_t value;
  uint8_t rval;
  addr = serial_rx_byte();
  value = serial_rx_byte();
  rval = 1;
  switch(addr) {
    case 0x00:
      SYNC1 = value;
      break;
    case 0x01:
      SYNC0 = value;
      break;
    case 0x02:
      PKTLEN = value;
      break;
    case 0x03:
      PKTCTRL1 = value;
      break;
    case 0x04:
      PKTCTRL0 = value;
      break;
    case 0x05:
      ADDR = value;
      break;
    case 0x06:
      CHANNR = value;
      break;
    case 0x07:
      FSCTRL1 = value;
      break;
    case 0x08:
      FSCTRL0 = value;
      break;
    case 0x09:
      FREQ2 = value;
      break;
    case 0x0A:
      FREQ1 = value;
      break;
    case 0x0B:
      FREQ0 = value;
      break;
    case 0x0C:
      MDMCFG4 = value;
      break;
    case 0x0D:
      MDMCFG3 = value;
      break;
    case 0x0E:
      MDMCFG2 = value;
      break;
    case 0x0F:
      MDMCFG1 = value;
      break;
    case 0x10:
      MDMCFG0 = value;
      break;
    case 0x11:
      DEVIATN = value;
      break;
    case 0x12:
      MCSM2 = value;
      break;
    case 0x13:
      MCSM1 = value;
      break;
    case 0x14:
      MCSM0 = value;
      break;
    case 0x15:
      FOCCFG = value;
      break;
    case 0x16:
      BSCFG = value;
      break;
    case 0x17:
      AGCCTRL2 = value;
      break;
    case 0x18:
      AGCCTRL1= value;
      break;
    case 0x19:
      AGCCTRL0 = value;
      break;
    case 0x1A:
      FREND1 = value;
      break;
    case 0x1B:
      FREND0 = value;
      break;
    case 0x1C:
      FSCAL3 = value;
      break;
    case 0x1D:
      FSCAL2 = value;
      break;
    case 0x1E:
      FSCAL1 = value;
      break;
    case 0x1F:
      FSCAL0 = value;
      break;
    case 0x20:
      PA_TABLE1 = value;
      break;
    case 0x21:
      PA_TABLE0 = value;
      break;
    default:
      rval = 2;
  }
  serial_tx_byte(rval);
  serial_tx_byte(0);
}

void cmd_reset() {
  EA = 0;
  WDCTL = BIT3 | BIT0;
}
