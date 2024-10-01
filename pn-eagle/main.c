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
	
    while (1) {
		if (circBuffer_elements(&rxBuffer)) {
			uint8_t byte = circBuffer_get(&rxBuffer);
			protocol_newByte(byte);
		}
		
		if (circBuffer_elements(&txBuffer) && uart0_readyToSend()) {
			uint8_t byte = circBuffer_get(&txBuffer);
			uart0_sendByte(byte);
		}
		
		if (tim1ms_nextTime <= sys_time) {  
			timer1msEvent();
			tim1ms_nextTime = sys_time + 1;
		}	
		
		if (tim100ms_nextTime <= sys_time) {
			timer100msEvent();
			tim100ms_nextTime = sys_time + 100;
		}
		
		for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
			encoder_process(&encoder[i], sys_time);
			actuator_process(&actuator[i], sys_time);
		}
    }
}
