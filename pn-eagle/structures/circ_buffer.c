#include "circ_buffer.h"

void circBuffer_put(circ_buffer_t *base, uint8_t byte) {
	while (circBuffer_capacity(base) == 0);                
	base->buffer[base->head++ & CIRC_BUFFER_MASK] = byte;
}

uint8_t circBuffer_get(circ_buffer_t *base) {
	return base->buffer[base->tail++ & CIRC_BUFFER_MASK];
}

uint8_t circBuffer_elements(circ_buffer_t *base) {
	return (base->head - base->tail);
}

uint8_t circBuffer_capacity(circ_buffer_t *base) {
	return CIRC_BUFFER_SIZE - (base->head - base->tail);
}