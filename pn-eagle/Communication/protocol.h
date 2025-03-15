#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PROTOCOL_CHANNELS				2
#define PROTOCOL_REQUEST_PREFIX			"MMNR33,"
#define PROTOCOL_RESPONSE_PREFIX		"MMNT44,"
#define PROTOCOL_REQUEST_PREFIX_LEN		7
#define PROTOCOL_RESPONSE_PREFIX_LEN	7

#include <stdint.h>

void protocol_init(void (*protocolFrameParsedEvent)(int16_t*, uint8_t));
void protocol_newByte(uint8_t byte);
void protocol_parseFrame(char *frame, int16_t *fieldArray, uint8_t *fieldNum);
char* protocol_generateAnswer(int16_t *fieldArray, uint8_t fieldNum);

#endif /* PROTOCOL_H_ */