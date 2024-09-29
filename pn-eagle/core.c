#include "core.h"
#include "logger.h"
#include "protocol.h"
#include <avr/io.h>

uint8_t rxBufferData[128];
uint8_t txBufferData[128];
circ_buffer_t rxBuffer;
circ_buffer_t txBuffer;

actuator_t actuator[3];
encoder_t encoder[3];

void coreInit(void) {
	protocol_init(protocolFrameParsedEvent);
	
	actuator_init(&actuator[0], &gpio_d, 4, 0);
	actuator_init(&actuator[1], &gpio_d, 5, 1);
	actuator_init(&actuator[2], &gpio_d, 6, 2);
	
	encoder_init(&encoder[0]);
	encoder_init(&encoder[1]);
	encoder_init(&encoder[2]);
}

void usartReceivedByteEvent(uint8_t byte) {
	circBuffer_put(&rxBuffer, byte);
}

void extintEdgeEvent(uint8_t channel, uint8_t dir) {
	encoder_update(&encoder[channel], dir);
}

void loggerStringToSendEvent(const char *buffer) {
	while (*buffer) {
		circBuffer_put(&txBuffer, (uint8_t) *buffer++);
	}
}

void protocolFrameParsedEvent(uint16_t *fieldArray, uint8_t fieldCount) {
	uint8_t channels = fieldCount - 1;
	
	for (uint8_t i = 0; i < 3; i++) {
		actuator_setTargetPos(&actuator[i], fieldArray[i]);
	}
}