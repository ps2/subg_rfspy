
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
  /* 3 */ cmd_set_channel, 
  /* 4 */ cmd_get_packet,
  /* 5 */ cmd_send_packet
};

void cmd_set_channel() {
  CHANNR = serial_rx_byte();
  serial_tx_byte(0);
}

void cmd_get_packet() {
  uint8_t timeout_ms;
  timeout_ms = serial_rx_byte();
  get_packet_and_write_to_serial(timeout_ms);
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
  uint8_t repeat_count;
  uint16_t delay_ms;
  repeat_count = serial_rx_byte();
  delay_ms = serial_rx_byte();
  send_packet_from_serial(repeat_count, delay_ms);
  serial_tx_byte(0);
}

