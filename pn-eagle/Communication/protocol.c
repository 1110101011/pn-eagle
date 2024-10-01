#include "protocol.h"
#include "circ_buffer.h"
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"

static const char requestPrefix[] = "MMNR33,";
static const char responsePrefix[] = "MMNT44,";

static void (*protocolFrameParsedCallback)(int16_t *fieldArray, uint8_t fieldCount);

static uint8_t requestBufferData[64];
static circ_buffer_t requestBuffer;
static char requestFrame[64];

static char answerFrame[64];

void protocol_init(void (*protocolFrameParsedEvent)(int16_t*, uint8_t)) {
	protocolFrameParsedCallback = protocolFrameParsedEvent;
	circBuffer_init(&requestBuffer, requestBufferData, sizeof(requestBufferData));
}

void protocol_newByte(uint8_t byte) {
	if (byte == '\r' || byte == '\n') {
	    if (circBuffer_elements(&requestBuffer) <= strlen(requestPrefix)) {
	        circBuffer_clear(&requestBuffer);
	        return;
	    }
	    
		uint8_t prefixIndex = 0;
		uint8_t frameIndex = 0;
		
		while (circBuffer_elements(&requestBuffer)) {
			uint8_t byte = circBuffer_get(&requestBuffer);
			
			if (prefixIndex >= strlen(requestPrefix)) {
				requestFrame[frameIndex++] = byte;
			} else if (byte == requestPrefix[prefixIndex]) {
				prefixIndex++;
			} else {
			    circBuffer_clear(&requestBuffer);
			    return;
			}
		}
		requestFrame[frameIndex] = '\0';
		
		int16_t frameFieldArray[16];
		uint8_t frameFieldNum;
		protocol_parseFrame(requestFrame, frameFieldArray, &frameFieldNum);
		
		if (frameFieldArray[frameFieldNum - 1] > 0) {
			// TODO: obliczanie crc
			circBuffer_clear(&requestBuffer);
			return;
		}
		
		protocolFrameParsedCallback(frameFieldArray, frameFieldNum - 1);
		circBuffer_clear(&requestBuffer);
	} else {
		circBuffer_put(&requestBuffer, byte);
	}
}

void protocol_parseFrame(char *frame, int16_t *fieldArray, uint8_t *fieldNum) {
	char temp[64];
	strcpy(temp, frame);

	char *token = strtok(temp, ",");
	uint8_t index = 0;

	while (token != NULL) {
		fieldArray[index++] = atoi(token);  
		token = strtok(NULL, ",");
	}

	*fieldNum = index;
}

char* protocol_generateAnswer(int16_t *fieldArray, uint8_t fieldNum) {
	char fieldBuffer[5];
	uint16_t crc = 0; // TODO: obliczanie crc
	
	strcpy(answerFrame, responsePrefix);
	
	for (uint8_t i = 0; i < fieldNum; i++) {
		strcat(answerFrame, itoa(fieldArray[i], fieldBuffer, 10));
		strcat(answerFrame, ",");
	}
	
	strcat(answerFrame, itoa(crc, fieldBuffer, 10));
	strcat(answerFrame, "\r\n");
	
	return answerFrame;
}