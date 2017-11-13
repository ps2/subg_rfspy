#ifndef COMMANDS_H
#define COMMANDS_H

#define ERROR_RX_TIMEOUT 0xaa
#define ERROR_CMD_INTERRUPTED 0xbb

enum RegisterMode {
  RegisterModeTx = 0x01,
  RegisterModeRx = 0x02
  /* maybe idle in future? */
};

void get_command();

// This is set when a command is received while processing a long running
// command, like get_packet
extern uint8_t interrupting_cmd;

#endif
