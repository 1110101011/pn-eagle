#include "uart.h"
#include "sys.h"
#include <avr/io.h>

static usart_t usart;

void uart_init(uint16_t baud, void (*receivedByteEvent)(uint8_t)) {
	usart.baud = baud;
	usart.receivedByteEvent = receivedByteEvent;
	
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

void uart_sendByte(uint8_t byte) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = byte;
}

ISR(USART0_RXC_vect) {
	uint8_t byte = UDR0;
	usart.receivedByteEvent(byte);
}