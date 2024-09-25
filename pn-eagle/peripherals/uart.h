#ifndef UART_H_
#define UART_H_

#include <stdint.h>

typedef struct {
	uint16_t baud;
	void (*receivedByteEvent)(uint8_t byte);
} usart_t;

void uart_init(uint16_t baud, void (*receivedByteEvent)(uint8_t));
void uart_sendByte(uint8_t byte);

#endif
