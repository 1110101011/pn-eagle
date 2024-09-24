#include <avr/io.h>
#include "peripherals/uart.h"

int main(void) {
    uart_init(9600);
	uart_sendByte('a');
	
    while (1) {
    }
}

