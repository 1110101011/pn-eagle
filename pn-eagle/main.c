#include <avr/io.h>
#include <avr/interrupt.h>
#include "sys.h"
#include "peripherals/uart.h"
#include "structures/circ_buffer.h"
#include "protocol.h"
#include "logger.h"

void usartReceivedByteEvent(uint8_t byte);
void loggerStringToSendEvent(const char *buffer);

circ_buffer_t rxBuffer;
circ_buffer_t txBuffer;

int main(void) {
    uart_init(9600, usartReceivedByteEvent);
	
	circBuffer_init(&rxBuffer);
	circBuffer_init(&txBuffer);
	logger_init(loggerStringToSendEvent);
	
	sei();
	
	logger_println("Uruchomienie programu\n");
	logger_printValue("baud: ", 9600);
	logger_println("\n");
	
    while (1) {
		if (circBuffer_elements(&rxBuffer)) {
			uint8_t byte = circBuffer_get(&rxBuffer);
			protocol_newByte(byte);
		}
		
		if (circBuffer_elements(&txBuffer)) {
			uint8_t byte = circBuffer_get(&txBuffer);
			uart_sendByte(byte);
		}
    }
}

void usartReceivedByteEvent(uint8_t byte) {
	circBuffer_put(&rxBuffer, byte);
}

void loggerStringToSendEvent(const char *buffer) {
	while (*buffer) {
		circBuffer_put(&txBuffer, (uint8_t) *buffer++);
	}
}
