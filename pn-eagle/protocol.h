#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>

typedef struct {
	uint8_t data[48];
	uint8_t dataIndex;
	uint8_t prefixIndex;
} protocol_frame_t;

void protocol_init(void);
void protocol_newByte(uint8_t byte);

#endif /* PROTOCOL_H_ */