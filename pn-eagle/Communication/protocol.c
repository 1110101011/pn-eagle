#include "protocol.h"
#include "circ_buffer.h"
#include "logger.h"
#include <string.h>

static const char protocolRequestPrefix[] = "MMNR33,";
static const char protocolResponsePrefix[] = "MMNT44,";

static uint8_t protocolRequestBufferData[64];
static circ_buffer_t protocolRequestBuffer;
static char protocolRequestFrame[64];

void protocol_init(void) {
	circBuffer_init(&protocolRequestBuffer, protocolRequestBufferData, sizeof(protocolRequestBufferData));
}

void protocol_newByte(uint8_t byte) {
	if (byte == '\r' || byte == '\n') {
	    if (circBuffer_elements(&protocolRequestBuffer) <= strlen(protocolRequestPrefix)) {
	        circBuffer_clear(&protocolRequestBuffer);
	        return;
	    }
	    
		uint8_t prefixIndex = 0;
		uint8_t frameIndex = 0;
		
		while (circBuffer_elements(&protocolRequestBuffer)) {
			uint8_t byte = circBuffer_get(&protocolRequestBuffer);
			
			if (prefixIndex >= strlen(protocolRequestPrefix)) {
				protocolRequestFrame[frameIndex++] = byte;
			} else if (byte == protocolRequestPrefix[prefixIndex]) {
				prefixIndex++;
			} else {
			    circBuffer_clear(&protocolRequestBuffer);
			    return;
			}
		}
		
		circBuffer_clear(&protocolRequestBuffer);
		
		protocolRequestFrame[frameIndex] = 0;
		protocol_parseFrame(protocolRequestFrame);
	} else {
		circBuffer_put(&protocolRequestBuffer, byte);
	}
}

void protocol_parseFrame(char *frame) {
	logger_println("T");
}