#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "protocol.h"
#include "logger.h"
#include "crc.h"
#include "util.h"

static const char requestPrefix[] = PROTOCOL_REQUEST_PREFIX;
static const char responsePrefix[] = PROTOCOL_RESPONSE_PREFIX;

static void (*protocolFrameParsedCallback)(int16_t *fieldArray, uint8_t fieldCount);

static char answerFrame[64];
static char requestFrame[64];
uint8_t frameByteIndex = 0;

uint8_t prefixByteIndex = 0;
uint8_t lastCommaIndex = 0;

char frameFieldBuffer[8] = {0};
uint8_t frameFieldBufferIndex = 0;

int16_t frameFieldArray[16];
uint8_t frameFieldNum = 0;

void protocol_init(void (*protocolFrameParsedEvent)(int16_t*, uint8_t)) {
	protocolFrameParsedCallback = protocolFrameParsedEvent;
}

static inline void protocol_reset(void) {
	frameFieldBufferIndex = 0;
	prefixByteIndex = 0;
	frameByteIndex = 0;
	frameFieldNum = 0;
	lastCommaIndex = 0;
}

// Wersja oryginalna na prawid³ow¹ ramkê
/*void protocol_newByte(uint8_t byte) {
	if (byte == '\r' || byte == '\n') {
		if (prefixByteIndex < PROTOCOL_REQUEST_PREFIX_LEN) {
			protocol_reset();
			return;
		}
		
		frameFieldArray[frameFieldNum++] = fast_atoin(frameFieldBuffer, frameFieldBufferIndex);
		
		uint8_t frameCrc = frameFieldArray[frameFieldNum - 1];
		uint8_t crc = crc_calc((uint8_t*) requestFrame, lastCommaIndex);
		
		if (crc == frameCrc || frameCrc == 0) {
			protocolFrameParsedCallback(frameFieldArray, frameFieldNum - 1);
		}	
		
		protocol_reset();
		
	} else if (prefixByteIndex >= PROTOCOL_REQUEST_PREFIX_LEN) {
		requestFrame[frameByteIndex] = byte;
		
		if (frameByteIndex < sizeof(requestFrame) - 1) {
			frameByteIndex++;
		} else {
			protocol_reset();
		}
		
		if (byte == ',') {
			frameFieldArray[frameFieldNum] = fast_atoin(frameFieldBuffer, frameFieldBufferIndex);
			lastCommaIndex = frameByteIndex;
			frameFieldBufferIndex = 0;
			
			if (frameFieldNum < sizeof(frameFieldArray) - 2) {
				frameFieldNum++;
			} else {
				protocol_reset();
			}
		} else if (frameFieldBufferIndex < sizeof(frameFieldBuffer) - 1) {
			frameFieldBuffer[frameFieldBufferIndex++] = byte;
		} else {
			protocol_reset();
		}
	
	} else if (byte == requestPrefix[prefixByteIndex]) {
		requestFrame[frameByteIndex++] = byte;
		prefixByteIndex++;
		
	} else {
		protocol_reset();
	}
}*/

// Wersja na ramkê skrócon¹ w formacie MMNR33,pozA,pozB,
void protocol_newByte(uint8_t byte) {
	if (prefixByteIndex >= PROTOCOL_REQUEST_PREFIX_LEN) {
		requestFrame[frameByteIndex] = byte;
		
		if (frameByteIndex < sizeof(requestFrame) - 1) {
			frameByteIndex++;
		} else {
			protocol_reset();
			return;
		}

		if (byte == ',') {
			frameFieldArray[frameFieldNum] = fast_atoin(frameFieldBuffer, frameFieldBufferIndex);
			frameFieldBufferIndex = 0;
			
			if (frameFieldNum < PROTOCOL_CHANNELS - 1) {
				frameFieldNum++;
			} else {
				protocolFrameParsedCallback(frameFieldArray, frameFieldNum + 1);
				protocol_reset();
				return;
			}
		} else if (frameFieldBufferIndex < sizeof(frameFieldBuffer) - 1) {
			frameFieldBuffer[frameFieldBufferIndex++] = byte;
		} else {
			protocol_reset();
			return;
		}
	} else if (byte == requestPrefix[prefixByteIndex]) {
		requestFrame[frameByteIndex++] = byte;
		prefixByteIndex++;
	} else {
		protocol_reset();
	}
}

char* protocol_generateAnswer(int16_t *fieldArray, uint8_t fieldNum) {
	char fieldBuffer[5];
	
	strcpy(answerFrame, responsePrefix);
	
	for (uint8_t i = 0; i < fieldNum; i++) {
		strcat(answerFrame, itoa(fieldArray[i], fieldBuffer, 10));
		strcat(answerFrame, ",");
	}
	
	uint8_t crc = crc_calc((uint8_t*) answerFrame, strlen(answerFrame));
	
	strcat(answerFrame, itoa(crc, fieldBuffer, 10));
	strcat(answerFrame, "\r\n");
	
	return answerFrame;
}