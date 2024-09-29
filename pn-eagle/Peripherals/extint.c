#include "extint.h"
#include "encoder.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static void (*edgeCallback)(uint8_t channel, uint8_t dir);

void extint_init(void (*edgeEvent)(uint8_t, uint8_t)) {
	edgeCallback = edgeEvent;
	
	PORTD |= (1 << PD0) | (1 << PD1) | (1 << PD2);
	PORTB |= (1 << PB0) | (1 << PB2) | (1 << PB3);
	
	EICRA |= (1 << ISC01) | (1 << ISC00);	// INT0 (PD0) rising edge 
	EICRA |= (1 << ISC11) | (1 << ISC10);	// INT1 (PD1) rising edge 
	EICRA |= (1 << ISC21) | (1 << ISC20);	// INT2 (PD2) rising edge 

	EIMSK |= (1 << INT0) | (1 << INT1) | (1 << INT2);
}

ISR(INT0_vect) {
	int8_t dir = (PINB & (1 << PB0)) ? 1 : -1;
	edgeCallback(0, dir);
}

ISR(INT1_vect) {
	int8_t dir = (PINB & (1 << PB2)) ? 1 : -1;
	edgeCallback(1, dir);
}

ISR(INT2_vect) {
	int8_t dir = (PINB & (1 << PB3)) ? 1 : -1;
	edgeCallback(2, dir);
}