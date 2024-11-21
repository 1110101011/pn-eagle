#include "uart0.h"
#include "sys.h"
#include "circ_buffer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static uint8_t txBufferData[UART_TX_BUFFER_SIZE];
static uint8_t rxBufferData[UART_RX_BUFFER_SIZE];
static circ_buffer_t txBuffer;
static circ_buffer_t rxBuffer;

ISR(USART0_RX_vect) {
	uint8_t byte = UDR0;
	circBuffer_putOverwrite(&rxBuffer, byte);
}

ISR(USART0_UDRE_vect) {
	if (circBuffer_elements(&txBuffer) > 0) {
		UDR0 = circBuffer_get(&txBuffer);
	}
	
	if (circBuffer_elements(&txBuffer) == 0) {
		UCSR0B &=
			~(1 << UDRIE0);					// Data Register Empty Interrupt Disable
	}
}

void uart0_init(uint16_t baud) {
	uint16_t ubrr = (F_CPU / 16 / baud) - 1;

	UBRR0H = (uint8_t) (ubrr >> 8);			// Baud Rate Register High
	UBRR0L = (uint8_t) ubrr;				// Baud Rate Register Low
	
	UCSR0B =
		(1 << RXEN0) |						// Receiver Enable
		(1 << TXEN0) |						// Transmitter Enable
		(1 << RXCIE0);						// RX Complete Interrupt Enable
	
	UCSR0C =
		(1 << UCSZ01) | (1 << UCSZ00);		// Character Size 8bit
	
	circBuffer_init(&txBuffer, txBufferData, sizeof(txBufferData));
	circBuffer_init(&rxBuffer, rxBufferData, sizeof(rxBufferData));
}

void uart0_sendData(uint8_t* data, uint8_t dataLen) {
	for (uint8_t i = 0; i < dataLen; i++) {
		circBuffer_putBlocking(&txBuffer, data[i]);
		
		UCSR0B |=
			(1 << UDRIE0);					// Data Register Empty Interrupt Enable
	}
}

uint8_t uart0_dataToRead(void) {
	return circBuffer_elements(&rxBuffer);
}

uint8_t uart0_readData(void) {
	return circBuffer_get(&rxBuffer);
}