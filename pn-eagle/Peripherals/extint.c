#include <avr/io.h>
#include <avr/interrupt.h>
#include "extint.h"

static void (*edgeCallback)(uint8_t channel, int8_t dir);

static uint8_t lastStateA[3] = {0, 0, 0};

void extint_init(void (*edgeEvent)(uint8_t, int8_t)) {
	edgeCallback = edgeEvent;
	
	PORTD |= (1 << PD0) | (1 << PD1) | (1 << PD2);
	PORTB |= (1 << PB0) | (1 << PB2) | (1 << PB3);
}

void extint_poll(void) {
	uint8_t currentStateA[] = {
		PIND & (1 << PD0), 
		PIND & (1 << PD1),
		PIND & (1 << PD2)
	};
	
	uint8_t currentStateB[] = {
		PINB & (1 << PB0),
		PINB & (1 << PB2),
		PINB & (1 << PB3)
	};
	
	for (uint8_t i = 0; i < 3; i++) {
		if (currentStateA[i] == 0 && lastStateA[i] > 0) {
			int8_t dir = (currentStateB[i] > 0) ? 1 : -1;
			edgeCallback(i, dir);
		}
		else if (currentStateA[i] > 0 && lastStateA[i] == 0) {
			int8_t dir = (currentStateB[i] > 0) ? -1 : 1;
			edgeCallback(i, dir);
		}
		
		lastStateA[i] = currentStateA[i];
	}
}