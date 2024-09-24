#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_init(uint16_t baud);
void uart_sendByte(uint8_t byte);

#endif
