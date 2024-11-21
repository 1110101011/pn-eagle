#include <avr/io.h>
#include <string.h>
#include "core.h"
#include "logger.h"
#include "uart0.h"
#include "protocol.h"
#include "config.h"

actuator_t actuator[CONF_ACTUATOR_COUNT];
encoder_t encoder[CONF_ACTUATOR_COUNT];

void coreInit(void) {
	protocol_init(protocolFrameParsedEvent);
	
	encoder_init(&encoder[0], 1);
	encoder_init(&encoder[1], 0);
	encoder_init(&encoder[2], 0);
	
	actuator_init(&actuator[0], &gpio_d, 4, 0, &encoder[0], 1);
	actuator_init(&actuator[1], &gpio_d, 5, 1, &encoder[1], 0);
	actuator_init(&actuator[2], &gpio_d, 6, 2, &encoder[2], 0);
	
	for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
		actuator_startHoming(&actuator[i]);
	}
}

void extintEdgeEvent(uint8_t channel, int8_t dir) {
	encoder_update(&encoder[channel], dir);
}

void loggerStringToSendEvent(const char *buffer) {
	uart0_sendData((uint8_t*) buffer, strlen(buffer));
}

void protocolFrameParsedEvent(int16_t *fieldArray, uint8_t fieldCount) {
	uint8_t positionNumber = fieldCount - 1;
	uint8_t homingFlag = fieldArray[fieldCount - 1];
	
	if (positionNumber != CONF_ACTUATOR_COUNT) {
		return;
	}
	
	if (homingFlag == 1) {
		for (uint8_t i = 0; i < positionNumber; i++) {
			actuator_startHoming(&actuator[i]);
		}
		return;
	}
	
	for (uint8_t i = 0; i < positionNumber; i++) {
		actuator_setTargetPos(&actuator[i], fieldArray[i] * 2);
	}
}

void timer200msEvent(void) {
	int16_t fieldArray[CONF_ACTUATOR_COUNT * 3];
	
	for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
		fieldArray[i * 2] = actuator_getCurrentPos(&actuator[i]) / 2;
		fieldArray[(i * 2) + 1] = actuator_getTargetPos(&actuator[i]) / 2;
	}
	
	for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
		fieldArray[(CONF_ACTUATOR_COUNT * 2) + i] = actuator_getErrorCode(&actuator[i]);
	}
	
	char *frameBuffer = protocol_generateAnswer(fieldArray, sizeof(fieldArray) / sizeof(fieldArray[0]));
	
	uart0_sendData((uint8_t*) frameBuffer, strlen(frameBuffer));
}