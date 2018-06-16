
/* fifo queue for use with interrupt routines.  Size must be power of 2 */

#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware.h"

#ifdef __GNUC__
#define inline static inline
#endif

struct fifo_buffer_t {
    volatile uint8_t head;     /* first byte of data */
    volatile uint8_t tail;     /* last byte of data */
    volatile uint8_t *buffer;  /* block of memory or array of data */
    uint8_t buffer_len;          /* length of the data */
};
typedef struct fifo_buffer_t fifo_buffer;

inline uint8_t fifo_count(fifo_buffer const *b) {
    return (b ? (b->head - b->tail) : 0);
}

inline bool fifo_empty(fifo_buffer const *b) {
    return (b ? (fifo_count(b) == 0) : true);
}

inline uint8_t fifo_get(fifo_buffer * b) {
    uint8_t data_byte = 0;

    if (!fifo_empty(b)) {
        data_byte = b->buffer[b->tail % b->buffer_len];
        b->tail++;
    }
    return data_byte;
}

inline bool fifo_full(fifo_buffer const *b) {
    return (b ? (fifo_count(b) == b->buffer_len) : true);
}

inline uint8_t fifo_peek(fifo_buffer const *b) {
    if (b) {
        return (b->buffer[b->tail % b->buffer_len]);
    }
    return 0;
}

inline bool fifo_put(fifo_buffer * b, uint8_t data_byte) {
    bool status = false;        /* return value */

    if (b) {
        /* limit the ring to prevent overwriting */
        if (!fifo_full(b)) {
            b->buffer[b->head % b->buffer_len] = data_byte;
            b->head++;
            status = true;
        }
    }
    return status;
}

inline void fifo_init(fifo_buffer * b, volatile uint8_t *buffer, uint8_t size) {
    if (b) {
        b->head = 0;
        b->tail = 0;
        b->buffer = buffer;
        b->buffer_len = size;
    }
}

#endif // FIFO_H
