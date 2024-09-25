#ifndef CIRC_BUFFER_H_
#define CIRC_BUFFER_H_

#include <stdint.h>

#define CIRC_BUFFER_SIZE     64
#define CIRC_BUFFER_MASK     (CIRC_BUFFER_SIZE - 1)

typedef struct {
	uint8_t Buffer[CIRC_BUFFER_SIZE];
	uint32_t Tail;
	uint32_t Head;
} circ_buffer_t;

static inline void circBuffer_init(circ_buffer_t *buffer) {
	buffer->Head = 0;
	buffer->Tail = 0;
}

static inline uint8_t circBuffer_capacity(circ_buffer_t *buffer) {
	return (buffer->Tail - buffer->Head - 1) & CIRC_BUFFER_MASK;
}

static inline void circBuffer_put(circ_buffer_t *buffer, uint8_t element) {
	while (!circBuffer_capacity(buffer));
	buffer->Buffer[buffer->Head++ & CIRC_BUFFER_MASK] = element;
}

static inline uint8_t circBuffer_get(circ_buffer_t *buffer) {
	return buffer->Buffer[buffer->Tail++ & CIRC_BUFFER_MASK];
}

static inline uint8_t circBuffer_elements(circ_buffer_t *buffer) {
	return (buffer->Head - buffer->Tail);
}

#endif /* CIRC_BUFFER_H_ */