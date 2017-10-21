
#include <stdint.h>
#include <stddef.h>
#include "hardware.h"
#include "serial.h"
#include "radio.h"
#include "commands.h"

uint8_t interrupting_cmd = 0;

// If use_pktlen is 0, then a sentinal value of 0 determines end of packet.
// If use_pktlen is non-zero, then rx will stop at PKTLEN
uint8_t use_pktlen = 0;

typedef void (*CommandHandler)();

CommandHandler handlers[] = {
  /* 0  */ 0,
  /* 1  */ cmd_get_state,
  /* 2  */ cmd_get_version,
  /* 3  */ cmd_get_packet,
  /* 4  */ cmd_send_packet,
  /* 5  */ cmd_send_and_listen,
  /* 6  */ cmd_update_register,
  /* 7  */ cmd_reset,
  /* 8  */ cmd_led,
  /* 9  */ cmd_read_register,
  /* 10 */ cmd_set_mode_registers,
};

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

void cmd_set_mode_registers() {
  uint8_t register_mode;
  uint8_t count;
  uint8_t addr;
  uint8_t value;
  int i;
  mode_registers *registers;

  register_mode = serial_rx_byte();
  count = serial_rx_avail() / 2;

  switch(register_mode) {
    case RegisterModeTx:
      registers = &tx_registers;
      break;
    case RegisterModeRx:
      registers = &rx_registers;
      break;
    default:
      registers = 0x0;
      break;
  }

  if (registers != NULL) {
    mode_registers_clear(registers);
  }
  for (i=0; i<count; i++) {
    addr = serial_rx_byte();
    value = serial_rx_byte();
    if (registers != NULL) {
      mode_registers_add(registers, addr, value);
    }
  }
}

void cmd_get_packet() {
  uint8_t channel;
  uint32_t timeout_ms;
  uint8_t result;
  channel = serial_rx_byte();
  timeout_ms = serial_rx_long();
  result = get_packet_and_write_to_serial(channel, timeout_ms, use_pktlen);
  if (result != 0) {
    serial_tx_byte(result);
  }
  serial_flush();
}

void cmd_get_state() {
  serial_tx_str("OK");
}

void cmd_get_version() {
  serial_tx_str("subg_rfspy 2.0");
}

void cmd_send_packet() {
  uint8_t channel;
  uint8_t repeat_count;
  uint8_t delay_ms;
  uint8_t len;
  channel = serial_rx_byte();
  repeat_count = serial_rx_byte();
  delay_ms = serial_rx_byte();
  len = serial_rx_avail();
  send_packet_from_serial(channel, repeat_count, delay_ms, len);
  serial_tx_byte(0);
  serial_flush();
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
  uint8_t len;

  send_channel = serial_rx_byte();
  repeat_count = serial_rx_byte();
  delay_ms = serial_rx_byte();
  listen_channel = serial_rx_byte();
  timeout_ms = serial_rx_long();
  retry_count = serial_rx_byte();
  len = serial_rx_avail();

  send_packet_from_serial(send_channel, repeat_count, delay_ms, len);
  result = get_packet_and_write_to_serial(listen_channel, timeout_ms, use_pktlen);

  while (result == ERROR_RX_TIMEOUT && retry_count > 0) {
    resend_from_tx_buf(send_channel);
    result = get_packet_and_write_to_serial(listen_channel, timeout_ms, use_pktlen);
    retry_count--;
  }

  if (result != 0) {
    // Error, and no retries left
    serial_tx_byte(result);
  }
  serial_flush();
}

void cmd_read_register() {
  uint8_t addr;
  uint8_t value;
  addr = serial_rx_byte();
  value = get_register(addr);
  serial_tx_byte(value);
  serial_flush();
}

void cmd_update_register() {
  uint8_t addr;
  uint8_t value;
  uint8_t rval;
  addr = serial_rx_byte();
  value = serial_rx_byte();
  rval = set_register(addr, value);

  // If pktlen is modified, then we set use_pktlen
  if (addr == 0x02) {
    use_pktlen = 1;
  }

  serial_tx_byte(rval);
  serial_flush();
}

void cmd_reset() {
  EA = 0;
  WDCTL = BIT3 | BIT0;
}

void cmd_led() {
  uint8_t led;
  uint8_t mode;
  led = serial_rx_byte();
  mode = serial_rx_byte();
  led_set_mode(led, mode);//0, 1, 2 = Off, On, Auto
}
