
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
  /* 4 */ cmd_get_packet
};

void cmd_set_channel() {
  CHANNR = serial_rx_byte();
}

void cmd_get_packet() {
  
  get_packet();
}

void cmd_get_state() {
  serial_tx_byte('O');
  serial_tx_byte('K');
  serial_tx_byte(0);
}

void cmd_get_version() {
  serial_tx_byte('0');
  serial_tx_byte('.');
  serial_tx_byte('1');
  serial_tx_byte(0);
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

