#include <stdint.h>

// FIFO queue for packets

// Adjust this based on available space as indicated in linker .mem file
#define PACKET_DATA_BUF_LEN 2000

// This should be PACKET_DATA_LEN / average packet size
#define PACKET_COUNT_MAX 30

// Circular buffers
static uint8_t __xdata packet_data_buf[PACKET_DATA_BUF_LEN];
static uint8_t __xdata packet_lengths[PACKET_COUNT_MAX];

static uint16_t __xdata packet_data_buf_head;
static uint16_t __xdata packet_data_buf_tail;
static uint16_t __xdata packet_data_buf_len;
static uint8_t __xdata packet_lengths_head;
static uint8_t __xdata packet_lengths_tail;
static uint8_t __xdata packet_count;

uint8_t enqueue_packet(const uint8_t *packet_data, uint16_t packet_len) {
  uint16_t i;

  if (packet_data_buf_len + packet_len >= PACKET_DATA_BUF_LEN) {
    return 0;
  }

  if (packet_count + 1 >= PACKET_COUNT_MAX) {
    return 0;
  }

  packet_lengths[packet_lengths_head] = packet_len;
  packet_lengths_head = (packet_lengths_head + 1) % PACKET_COUNT_MAX;
  packet_count++;

  for (i=0; i<packet_len; i++) {
    packet_data_buf[packet_data_buf_head] = packet_data[i];
    packet_data_buf_head = (packet_data_buf_head+1) % PACKET_DATA_BUF_LEN;
  }
  packet_data_buf_len += packet_len;
  return 1;
}

uint8_t dequeue_packet(uint8_t *packet_data, uint16_t *packet_len) {
  uint16_t i;

  if (packet_count == 0) {
    return 0;
  }

  *packet_len = packet_lengths[packet_lengths_tail];
  packet_lengths_tail = (packet_lengths_tail + 1) % PACKET_COUNT_MAX;
  packet_count--;

  for(i=0; i<*packet_len; i++) {
    packet_data[i] = packet_data_buf[packet_data_buf_tail];
    packet_data_buf_tail = (packet_data_buf_tail+1) % PACKET_DATA_BUF_LEN;
  }
  packet_data_buf_len -= *packet_len;
  return 1;
}
