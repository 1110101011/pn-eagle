#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
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
static uint32_t tim200ms_nextTime = 0;

int main(void) {
	wdt_enable(WDTO_120MS);
	sys_init();
	
    uart0_init(CONF_UART_BAUD, usartReceivedByteEvent);
	logger_init(loggerStringToSendEvent);
	circBuffer_init(&rxBuffer, rxBufferData, sizeof(rxBufferData));
	circBuffer_init(&txBuffer, txBufferData, sizeof(txBufferData));
	
	timer1_init();
	extint_init(extintEdgeEvent);
	
	logger_print("Start programu\n");
	logger_print("Wersja: ");
	logger_print(VERSION_STRING);
	logger_print("\nKanaly: 3");
	logger_print(logger_endl);
	
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
		
		if (tim200ms_nextTime <= sys_time) {
			timer200msEvent();
			tim200ms_nextTime = sys_time + 200;
		}
		
		for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
			encoder_process(&encoder[i], sys_time);
			actuator_process(&actuator[i], sys_time);
		}
		
		wdt_reset();
    }
}
