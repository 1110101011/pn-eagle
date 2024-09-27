#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>

void protocol_init(void);
void protocol_newByte(uint8_t byte);
void protocol_parseFrame(char *frame);

#endif /* PROTOCOL_H_ */