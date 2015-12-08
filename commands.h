#ifndef COMMANDS_H
#define COMMANDS_H

void get_command();

// This is set when a command is received while processing a long running
// command, like get_packet
extern uint8_t interrupting_cmd; 

#endif
