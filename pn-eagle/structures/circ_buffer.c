#include "circ_buffer.h"

void circBuffer_init(circ_buffer_t *buffer, uint8_t *bufferData, uint8_t size) {
	buffer->buffer = bufferData;
	buffer->tail = 0;
	buffer->head = 0;
	buffer->size = size;
	buffer->mask = size - 1;
}