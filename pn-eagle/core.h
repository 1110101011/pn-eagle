#ifndef CORE_H_
#define CORE_H_

#include <stdint.h>
#include "circ_buffer.h"
#include "config.h"
#include "actuator.h"
#include "encoder.h"

extern uint8_t rxBufferData[128];
extern uint8_t txBufferData[128];
extern circ_buffer_t rxBuffer;
extern circ_buffer_t txBuffer;

extern actuator_t actuator[CONF_ACTUATOR_COUNT];
extern encoder_t encoder[CONF_ACTUATOR_COUNT];

void coreInit(void);

void usartReceivedByteEvent(uint8_t byte);
void extintEdgeEvent(uint8_t channel, int8_t dir);
void loggerStringToSendEvent(const char *buffer);
void protocolFrameParsedEvent(int16_t *fieldArray, uint8_t fieldCount);
void timer200msEvent(void);

#endif /* CORE_H_ */