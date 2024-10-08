#include <avr/io.h>
#include "gpio.h"

const gpio_t gpio_a = {&DDRA, &PORTA};
const gpio_t gpio_b = {&DDRB, &PORTB};
const gpio_t gpio_c = {&DDRC, &PORTC};
const gpio_t gpio_d = {&DDRD, &PORTD};
const gpio_t gpio_e = {&DDRE, &PORTE};
const gpio_t gpio_f = {&DDRF, &PORTF};
const gpio_t gpio_g = {&DDRG, &PORTG};
	
void gpio_pinConfig(const gpio_t *gpio, uint8_t pin, gpio_mode_t mode) {
	if (mode == OUTPUT) {
		*(gpio->ddr) |= (1 << pin);
	} else {
		*(gpio->ddr) &= ~(1 << pin);
	}
}

void gpio_pinSet(const gpio_t *gpio, uint8_t pin, uint8_t state) {
	if (state > 0) {
		*(gpio->port) |= (1 << pin);
	} else {
		*(gpio->port) &= ~(1 << pin);
	}
}