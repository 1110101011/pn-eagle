#ifndef CIRC_BUFFER_H_
#define CIRC_BUFFER_H_

#include <stdint.h>

typedef struct {
	uint8_t *buffer;
	uint32_t tail;
	uint32_t head;
	uint8_t size;
	uint8_t mask;
} circ_buffer_t;

void circBuffer_init(circ_buffer_t *buffer, uint8_t *bufferData, uint8_t size);

static inline void circBuffer_clear(circ_buffer_t *buffer) {
	buffer->tail = 0;
	buffer->head = 0;
}

static inline uint8_t circBuffer_capacity(circ_buffer_t *buffer) {
	return (buffer->tail - buffer->head - 1) & buffer->mask;
}

static inline void circBuffer_putBlocking(circ_buffer_t *buffer, uint8_t element) {
//	while (!circBuffer_capacity(buffer));
	buffer->buffer[buffer->head++ & buffer->mask] = element;
}

static inline void circBuffer_putOverwrite(circ_buffer_t *buffer, uint8_t element) {
	buffer->buffer[buffer->head++ & buffer->mask] = element;
}

static inline uint8_t circBuffer_get(circ_buffer_t *buffer) {
	return buffer->buffer[buffer->tail++ & buffer->mask];
}

static inline uint8_t circBuffer_elements(circ_buffer_t *buffer) {
	return (buffer->head - buffer->tail);
}

#endif /* CIRC_BUFFER_H_ */