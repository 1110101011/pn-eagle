#include "protocol.h"

static protocol_frame_t frame;

void protocol_init(void) {
	frame.dataIndex = 0;
	frame.prefixIndex = 0;
}

void protocol_newByte(uint8_t byte) {
	if (byte == 'T') {
		frame.dataIndex = 0;
		frame.prefixIndex = 1;
	} else if (byte == 'M' && frame.prefixIndex == 1) {
		frame.prefixIndex++;
	} else if (byte == 'R' && frame.prefixIndex == 2) {
		frame.prefixIndex++;
	} else if (byte == 'A' && frame.prefixIndex == 3) {
		frame.prefixIndex = 4;
		return;
	}
	
	if ((byte == '\r' || byte == 'n') && frame.prefixIndex == 4) {
		protocol_parseFrame();
		frame.prefixIndex = 0;
	} else if (frame.dataIndex < 48-1){
		frame.data[frame.dataIndex++] = byte;
		return;
	} 
	
	frame.dataIndex = 0;
	frame.prefixIndex = 0;
}

void protocol_parseFrame(void) {
	
}