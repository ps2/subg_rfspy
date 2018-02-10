#ifndef PACKET_BUFFER_H
#define PACKET_BUFFER_H

// Returns 1 on success, 0 if not enough memory available.
uint8_t enqueue_packet(const uint8_t *packet_data, uint16_t packet_len);

// Returns 1 on success, 0 if no packets available.
uint8_t dequeue_packet(uint8_t *packet_data, uint16_t *packet_len);

#endif // PACKET_BUFFER_H
