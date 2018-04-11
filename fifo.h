
/* fifo queue for use with interrupt routines.  Size must be power of 2 */

#ifndef FIFO_H
#define FIFO_H

#include <stdint.h>
#include <stdbool.h>

struct fifo_buffer_t {
    volatile uint8_t head;     /* first byte of data */
    volatile uint8_t tail;     /* last byte of data */
    volatile uint8_t *buffer;  /* block of memory or array of data */
    uint8_t buffer_len;          /* length of the data */
};
typedef struct fifo_buffer_t fifo_buffer;

bool fifo_empty(fifo_buffer const *b);

bool fifo_full(fifo_buffer const *b);

uint8_t fifo_peek(fifo_buffer const *b);

uint8_t fifo_get(fifo_buffer *b);

bool fifo_put(fifo_buffer *b, uint8_t data_byte);

 /* size must be a power of 2 */
void fifo_init(fifo_buffer *b, volatile uint8_t *buffer, uint8_t size);


volatile uint8_t fifo_count(fifo_buffer const *b);

#endif // FIFO_H
