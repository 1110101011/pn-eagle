#ifndef CIRC_BUFFER_H_
#define CIRC_BUFFER_H_

#include <stdint.h>

typedef struct {
	uint8_t *Buffer;
	uint32_t Tail;
	uint32_t Head;
	uint8_t Size;
	uint8_t Mask;
} circ_buffer_t;

static inline void circBuffer_init(circ_buffer_t *buffer, uint8_t *bufferData, uint8_t size) {
	buffer->Buffer = bufferData;
	buffer->Tail = 0;
	buffer->Head = 0;
	buffer->Size = size;
	buffer->Mask = size - 1;
}

static inline void circBuffer_clear(circ_buffer_t *buffer) {
	buffer->Tail = 0;
	buffer->Head = 0;
}

static inline uint8_t circBuffer_capacity(circ_buffer_t *buffer) {
	return (buffer->Tail - buffer->Head - 1) & buffer->Mask;
}

static inline void circBuffer_put(circ_buffer_t *buffer, uint8_t element) {
	while (!circBuffer_capacity(buffer));
	buffer->Buffer[buffer->Head++ & buffer->Mask] = element;
}

static inline uint8_t circBuffer_get(circ_buffer_t *buffer) {
	return buffer->Buffer[buffer->Tail++ & buffer->Mask];
}

static inline uint8_t circBuffer_elements(circ_buffer_t *buffer) {
	return (buffer->Head - buffer->Tail);
}

#endif /* CIRC_BUFFER_H_ */