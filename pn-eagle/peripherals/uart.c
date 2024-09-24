#include "uart.h"
#include "../sys.h"
#include <avr/io.h>

void uart_init(uint16_t baud) {
	unsigned int ubrr = F_CPU/16/baud-1; 
	UBRR0H = (unsigned char)(ubrr>>8);   
	UBRR0L = (unsigned char)ubrr;        

	UCSR0B = (1<<RXEN0) | (1<<TXEN0);

	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);  
}

void uart_sendByte(uint8_t byte) {
	while (!(UCSR0A & (1<<UDRE0)));

	UDR0 = byte;
}