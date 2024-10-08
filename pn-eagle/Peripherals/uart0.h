#ifndef UART0_H_
#define UART0_H_

#include <stdint.h>

void uart0_init(uint16_t baud, void (*receivedByteEvent)(uint8_t));
uint8_t uart0_isReady(void);
void uart0_sendByte(uint8_t byte);

#endif
