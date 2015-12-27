
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
  /* 5 */ cmd_send_and_listen
};

void cmd_get_packet() {
  uint8_t channel;
  uint16_t timeout_ms;
  channel = serial_rx_byte();
  timeout_ms = serial_rx_word();
  get_packet_and_write_to_serial(channel, timeout_ms);
}

void cmd_get_state() {
  serial_tx_str("OK");
}

void cmd_get_version() {
  serial_tx_str("subg_rfspy 0.2");
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
  uint16_t timeout_ms;
  
  send_channel = serial_rx_byte();
  repeat_count = serial_rx_byte();
  delay_ms = serial_rx_byte();
  listen_channel = serial_rx_byte();
  timeout_ms = serial_rx_word();
  send_packet_from_serial(send_channel, repeat_count, delay_ms);
  get_packet_and_write_to_serial(listen_channel, timeout_ms);
}
