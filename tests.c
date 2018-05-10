#include <stdio.h>
#include <string.h>
#include "hardware.h"
#include "tests.h"

#define RESPONSE_BUFFER_SIZE 255

typedef struct {
  uint8_t response_code;
  uint8_t data[RESPONSE_BUFFER_SIZE];
  uint8_t response_length;
} CommandResponse;

void hexprint(const char *prefix, const uint8_t *data, int len) {
  printf("%s", prefix);
  for (int i=0; i<len; i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}

void do_spi(const uint8_t *input, uint8_t *output, uint8_t len) {
  if (input) {
    hexprint("tx: ", input, len);
  } else {
    printf("tx: NULL\n");
  }

  // Assert CSR
  U1CSR |= U1CSR_ACTIVE;

  uint8_t slave_byte;
  for (int i=0; i<len; i++) {
    slave_byte = U1DBUF_write;
    tx1_isr();
    U1DBUF_read = input ? input[i] : 0;
    if (output) {
      output[i] = slave_byte;
    }
    rx1_isr();
  }

  // De-assert CSR
  U1CSR &= ~U1CSR_ACTIVE;


  if (output) {
    hexprint("rx: ", output, len);
  } else {
    printf("rx: NULL\n");
  }
}

CommandResponse run_command(uint8_t len, const uint8_t *data, int timeout_s) {
  CommandResponse response;

  memset(&response, 0, sizeof(CommandResponse));
  uint8_t tmp[2];
  tmp[0] = 0x99;
  tmp[1] = len;
  do_spi(tmp, tmp, 2);
  uint8_t slave_bytes_avail = tmp[1];
  if (slave_bytes_avail > 0) {
    printf("Unexpected response data while sending command\n");
    if (slave_bytes_avail > len) {
      len = slave_bytes_avail;
    }
  }
  do_spi(data, NULL, len);
  while(1) {
    tmp[0] = 0x99;
    tmp[1] = 0;
    do_spi(tmp, tmp, 2);
    if (tmp[1] > 0) {
      response.response_length = tmp[1];
      do_spi(NULL, (uint8_t*)&response, response.response_length);
      printf("got response\n");
      break;
    } else {
      printf("no response yet...\n");
    }
    // TODO: check timeout
  }
  return response;
}

void test_version() {
  uint8_t cmd = 2;
  CommandResponse response;
  response = run_command(1, &cmd, 1);
  if (response.response_length > 0) {
    printf("response_code = %02x\n", response.response_code);
    printf("response.data = %s\n", response.data);
  } else {
    printf("No response\n");
  }

  response = run_command(1, &cmd, 1);
  if (response.response_length > 0) {
    printf("response_code = %02x\n", response.response_code);
    printf("response.data = %s\n", response.data);
  } else {
    printf("No response\n");
  }
}

void test_sync() {
  uint8_t cmd = 2;
  CommandResponse response;
  response = run_command(1, &cmd, 1);
  if (response.response_length > 0) {
    printf("response_code = %02x\n", response.response_code);
    printf("response.data = %s\n", response.data);
  } else {
    printf("No response\n");
  }

  uint8_t tmp[1];
  tmp[0] = 0x99;
  printf("************************\n");
  do_spi(tmp, tmp, 1);
  printf("************************\n");

  response = run_command(1, &cmd, 1);
  if (response.response_length > 0) {
    printf("response_code = %02x\n", response.response_code);
    printf("response.data = %s\n", response.data);
  } else {
    printf("No response\n");
  }
}
