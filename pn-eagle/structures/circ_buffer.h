#ifndef CIRC_BUFFER_H_
#define CIRC_BUFFER_H_

#define CIRC_BUFFER_SIZE		64                      
#define CIRC_BUFFER_MASK		(CIRC_BUFFER_SIZE - 1)  

#include <stdint.h>

typedef struct {
	uint8_t buffer[CIRC_BUFFER_SIZE];
	uint32_t tail;
	uint32_t head;
} circ_buffer_t;

void circBuffer_put(circ_buffer_t *base, uint8_t byte);
uint8_t circBuffer_get(circ_buffer_t *base);
uint8_t circBuffer_elements(circ_buffer_t *base);
uint8_t circBuffer_capacity(circ_buffer_t *base);

#endif /* CIRC_BUFFER_H_ */