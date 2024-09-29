#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "sys.h"
#include "core.h"
#include "uart0.h"
#include "timer1.h"
#include "extint.h"
#include "circ_buffer.h"
#include "protocol.h"
#include "logger.h"

uint32_t tim1ms_nextTime = 0;
uint32_t tim500ms_nextTime = 0;

int main(void) {
	sys_init();
	
	DDRB |= (1 << PB5) | (1 << PB6) | (1 << PB7);
	
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
		
		if (tim1ms_nextTime <= sys_time) {  // 1ms		
			actuator_setCurrentPos(&actuator[0], encoder[0].count);
			actuator_setCurrentPos(&actuator[1], encoder[1].count);
			actuator_setCurrentPos(&actuator[2], encoder[2].count);
			actuator_process1ms(&actuator[0]);
			actuator_process1ms(&actuator[1]);
			actuator_process1ms(&actuator[2]);
			
			tim1ms_nextTime = sys_time + 1;
		}	
		
		if (tim500ms_nextTime <= sys_time) {
			
			logger_printValue("ec2:", encoder[1].count);
			logger_printValue("ac2:", actuator[1].currentPos);
			logger_printValue("at2:", actuator[1].targetPos);
			logger_printValue("es2:", encoder[1].speed);
			logger_print(logger_endl);
			tim500ms_nextTime = sys_time + 500;
		}
		
		encoder_process(&encoder[0], sys_time);
		encoder_process(&encoder[1], sys_time);
		encoder_process(&encoder[2], sys_time);
    }
}
