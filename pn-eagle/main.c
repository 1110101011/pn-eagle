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

uint32_t tim1ms_nextTime = 0;
uint32_t tim1000ms_nextTime = 0;

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
		
		// timer 1000ms
		if (tim1000ms_nextTime <= sys_time) {
			int16_t fields[] = {
				actuator_getCurrentPos(&actuator[0]),
				actuator_getTargetPos(&actuator[0]),
				actuator_getCurrentPos(&actuator[1]),
				actuator_getTargetPos(&actuator[1]),
				actuator_getCurrentPos(&actuator[2]),
				actuator_getTargetPos(&actuator[2]),
				actuator[0].speed,
				actuator[1].speed,
				actuator[2].speed
			};
			
			char *frame1 = protocol_generateAnswer(fields, sizeof(fields) / sizeof(fields[0]));
			
			for (uint8_t i = 0; i < strlen(frame1); i++) {
				circBuffer_put(&txBuffer, (uint8_t) frame1[i]);
			}
	
			tim1000ms_nextTime = sys_time + 1000;
		}
		
		for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
			encoder_process(&encoder[i], sys_time);
			actuator_process(&actuator[i], sys_time);
		}
    }
}
