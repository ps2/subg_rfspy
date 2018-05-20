#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include "subg_rfspy.h"
#include "serial.h"
#include "commands.h"
#include "hardware.h"

#define RESPONSE_BUFFER_SIZE 128

typedef struct {
  uint8_t response_code;
  uint8_t data[RESPONSE_BUFFER_SIZE];
  uint8_t response_length;
  bool    timed_out;
} CommandResponse;

void hexprint(const char *prefix, const uint8_t *data, int len)
{
  printf("%s", prefix);
  for (int i=0; i<len; i++) {
    printf("%02x", data[i]);
  }
  printf("\n");
}

void tiny_sleep()
{
  // Sleep for 1ms
  struct timespec rqtp;
  rqtp.tv_sec  = 0;
  rqtp.tv_nsec = 1000000L;
  nanosleep(&rqtp, NULL);
}

void do_spi(const uint8_t *input, uint8_t *output, uint8_t len)
{
  if (len > RESPONSE_BUFFER_SIZE) {
    printf("Bad response size: %d\n", len);
    len = RESPONSE_BUFFER_SIZE;
  }

  uint8_t throwaway_rx_buf[RESPONSE_BUFFER_SIZE];
  // if (input) {
  //   hexprint("tx: ", input, len);
  // } else {
  //   printf("tx: NULL\n");
  // }

  if (output == NULL) {
    output = throwaway_rx_buf;
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
    tiny_sleep();
  }

  // De-assert CSR
  U1CSR &= ~U1CSR_ACTIVE;

  // if (output) {
  //   hexprint("rx: ", output, len);
  // } else {
  //   printf("rx: NULL\n");
  // }
}

CommandResponse run_command(uint8_t len, const uint8_t *data, int max_xfer_cycles)
{
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
  while(max_xfer_cycles--) {
    tmp[0] = 0x99;
    tmp[1] = 0;
    do_spi(tmp, tmp, 2);
    if (tmp[1] > 0) {
      response.response_length = tmp[1];
      do_spi(NULL, (uint8_t*)&response, response.response_length);
      break;
    }
    // TODO: check timeout
  }
  if (max_xfer_cycles <= 0) {
    response.timed_out = true;
  }
  return response;
}

void check_version()
{
  uint8_t cmd = CmdGetVersion;
  CommandResponse response;
  response = run_command(1, &cmd, 20);
  assert(!response.timed_out);
  assert(strcmp((const char*)response.data, "subg_rfspy 2.0") == 0);
}

void check_sync_error_extra_byte()
{
  CommandResponse response;
  uint8_t tmp[1];

  // Send extra byte (not conforming to protocol)
  tmp[0] = 0x99;
  do_spi(tmp, tmp, 1);

  tmp[0] = CmdGetVersion;
  response = run_command(1, tmp, 20);
  // Expected behavior: timeout
  assert(response.timed_out);

  // Should recover
  tmp[0] = CmdGetVersion;
  response = run_command(1, tmp, 20);
  assert(!response.timed_out);
  assert(strcmp((const char*)response.data, "subg_rfspy 2.0") == 0);
}

void check_sync_error_dropped_byte()
{
  CommandResponse response;
  uint8_t tmp[3];

  // Drop byte (not conforming to protocol)
  // CmdUpdateRegister should be 3 bytes, but only send 2
  tmp[0] = CmdUpdateRegister;
  tmp[1] = 1; // SYNC0
  tmp[2] = 0xff;
  response = run_command(2, tmp, 20);
  // Expected behavior: timeout
  assert(response.timed_out);

  // one failure (this data finishes the previous update register command)
  // subg_rfspy responds with success, since it received enough data for
  // the update register command.  But we're out of sync now.
  tmp[0] = CmdGetVersion;
  response = run_command(1, tmp, 20);
  assert(!response.timed_out);
  assert(response.response_length == 1);

  // Recovery
  tmp[0] = CmdGetVersion;
  response = run_command(1, tmp, 20);
  assert(!response.timed_out);
  assert(strcmp((const char*)response.data, "subg_rfspy 2.0") == 0);
}

void *run_main(void *vargp) {
  subg_rfspy_main();
  return NULL;
}

int main(void)
{
  #ifdef MOCK_HARDWARE
     pthread_t mock_hardware_thread;
     pthread_create(&mock_hardware_thread, NULL, run_mock_hardware, NULL);
  #endif

  #ifdef RUN_TESTS
    pthread_t run_main_thread;
    pthread_create(&run_main_thread, NULL, run_main, NULL);
  #endif

  while(!subg_rfspy_init_finished);

  // Run tests!
  check_version();
  check_sync_error_extra_byte();
  check_sync_error_dropped_byte();

  subg_rfspy_should_exit = true;
  pthread_join(run_main_thread, NULL);

  mock_hardware_should_exit = true;
  pthread_join(mock_hardware_thread, NULL);

  // If we are here, all tests passed.
  printf("Tests succeeded!\n");
}
