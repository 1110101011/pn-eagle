#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "sys.h"
#include "core.h"
#include "uart0.h"
#include "timer1.h"
#include "extint.h"
#include "circ_buffer.h"
#include "protocol.h"
#include "logger.h"

static uint32_t tim1ms_nextTime = 0;
static uint32_t tim100ms_nextTime = 0;

int main(void) {
	sys_init();
	
    uart0_init(57600, usartReceivedByteEvent);
	logger_init(loggerStringToSendEvent);
	circBuffer_init(&rxBuffer, rxBufferData, sizeof(rxBufferData));
	circBuffer_init(&txBuffer, txBufferData, sizeof(txBufferData));
	
	timer1_init();
	extint_init(extintEdgeEvent);
	
	coreInit();
	
	sei();
	
	logger_println("Uruchomienie programu\n");
	logger_printValue("baud: ", 57600);
	
    while (1) {
		if (circBuffer_elements(&rxBuffer)) {
			uint8_t byte = circBuffer_get(&rxBuffer);
			protocol_newByte(byte);
		}
		
		if (circBuffer_elements(&txBuffer) && uart0_readyToSend()) {
			uint8_t byte = circBuffer_get(&txBuffer);
			uart0_sendByte(byte);
		}
		
		// timer 1ms
		if (tim1ms_nextTime <= sys_time) {  
				
			tim1ms_nextTime = sys_time + 1;
		}	
		
		// timer 100ms
		if (tim100ms_nextTime <= sys_time) {
			int16_t fieldArray[CONF_ACTUATOR_COUNT * 3];
			
			for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
				fieldArray[i * 2] = actuator_getCurrentPos(&actuator[i]);
				fieldArray[(i * 2) + 1] = actuator_getTargetPos(&actuator[i]);
			}
			
			for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
				fieldArray[(CONF_ACTUATOR_COUNT * 2) + i] = actuator_getErrorCode(&actuator[i]);
			}
			
			char *frameBuffer = protocol_generateAnswer(fieldArray, sizeof(fieldArray) / sizeof(fieldArray[0]));
			
			for (uint8_t i = 0; i < strlen(frameBuffer); i++) {
				circBuffer_put(&txBuffer, (uint8_t) frameBuffer[i]);
			}
	
			tim100ms_nextTime = sys_time + 100;
		}
		
		for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
			encoder_process(&encoder[i], sys_time);
			actuator_process(&actuator[i], sys_time);
		}
    }
}
