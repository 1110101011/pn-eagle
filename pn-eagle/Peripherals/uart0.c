#include "uart0.h"
#include "sys.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static void (*receivedByteCallback)(uint8_t byte);

ISR(USART0_RX_vect) {
	uint8_t byte = UDR0;
	receivedByteCallback(byte);
}

void uart0_init(uint16_t baud, void (*receivedByteEvent)(uint8_t)) {
	receivedByteCallback = receivedByteEvent;
	
	uint16_t ubrr = (F_CPU / 16 / baud) - 1;

	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) ubrr;
	
	UCSR0B = 
		(1 << RXEN0) |
		(1 << TXEN0) | 
		(1 << RXCIE0);
	
	UCSR0C = 
		(1 << UCSZ01) | 
		(1 << UCSZ00);
}

uint8_t uart0_isReady(void) {
	return UCSR0A & (1 << UDRE0);
}

void uart0_sendByte(uint8_t byte) {
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = byte;
}