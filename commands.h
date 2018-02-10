#ifndef COMMANDS_H
#define COMMANDS_H

#define RESPONSE_CODE_RX_TIMEOUT 0xaa
#define RESPONSE_CODE_CMD_INTERRUPTED 0xbb
#define RESPONSE_CODE_SUCCESS 0xdd
#define RESPONSE_CODE_PARAM_ERROR 0x11
#define RESPONSE_CODE_UNKNOWN_COMMAND 0x22

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
