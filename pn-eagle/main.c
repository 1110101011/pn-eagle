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

static uint32_t tim200ms_nextTime = 0;

int main(void) {
	wdt_enable(WDTO_120MS);
	sei();
	
	sys_init();
	
    uart0_init(CONF_UART_BAUD);
	logger_init(loggerStringToSendEvent);
	
	timer1_init();
	extint_init(extintEdgeEvent);
	
	logger_print("Start programu\n");
	logger_print("Wersja: ");
	logger_print(VERSION_STRING);
	logger_print(logger_endl);
	logger_printValue("Liczba silownikow: ", CONF_ACTUATOR_COUNT);
	logger_printValue("Zakres pracy: ", CONF_ACTUATOR_RANGE);
	logger_print(logger_endl);
	
	coreInit();
	
    while (1) {
		while (uart0_dataToRead() > 0) {
			uint8_t byte = uart0_readData();
			protocol_newByte(byte);
		}
		
		for (uint8_t i = 0; i < CONF_ACTUATOR_COUNT; i++) {
			encoder_process(&encoder[i], sys_time);
			actuator_process(&actuator[i], sys_time);
		}
		
		if (tim200ms_nextTime <= sys_time) {
			timer200msEvent();
			tim200ms_nextTime = sys_time + 200;
		}
		
		extint_poll();
		
		wdt_reset();
    }
}
