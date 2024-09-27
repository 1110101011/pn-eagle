#include "protocol.h"
#include "circ_buffer.h"
#include "logger.h"

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
		uint8_t prefixIndex = 0;
		uint8_t frameIndex = 0;
		logger_println("N");
		while (circBuffer_elements(&protocolRequestBuffer)) {
			uint8_t byte = circBuffer_get(&protocolRequestBuffer);
			
			if (prefixIndex >= sizeof(protocolRequestPrefix)) {
				protocolRequestFrame[frameIndex++] = byte;
			} else if (byte == protocolRequestPrefix[prefixIndex]) {
				prefixIndex++;
			}
		}
		protocolRequestFrame[frameIndex] = 0;
		
		circBuffer_clear(&protocolRequestBuffer);
		protocol_parseFrame(protocolRequestFrame);
	} else {
		circBuffer_put(&protocolRequestBuffer, byte);
	}
}

void protocol_parseFrame(char *frame) {
	logger_println("T");
}