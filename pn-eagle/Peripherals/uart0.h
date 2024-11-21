#ifndef UART0_H_
#define UART0_H_

#define UART_TX_BUFFER_SIZE		128
#define UART_RX_BUFFER_SIZE		128

#include <stdint.h>

void uart0_init(uint16_t baud);
void uart0_sendData(uint8_t* data, uint8_t dataLen);
uint8_t uart0_dataToRead(void);
uint8_t uart0_readData(void);

#endif