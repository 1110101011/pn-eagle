#include "timer1.h"
#include <avr/io.h>

void timer1_init(void) {
	DDRB |= 
		(1 << PB5) | 
		(1 << PB6) | 
		(1 << PB7);
	
	TCCR1A |= 
		(1 << WGM11);  	             
	TCCR1B |= 
		(1 << WGM13) | 
		(1 << WGM12);
		
	TCCR1B |= 
		(1 << CS11);	// clk/8
		
	ICR1 = TIMER1_TOP;
	
	OCR1A = TIMER1_TOP;
	OCR1B = TIMER1_TOP;
	OCR1C = TIMER1_TOP;
}

void timer1_setChannelDuty(uint8_t channel, uint8_t duty) {
	uint16_t value = ((uint32_t) duty * TIMER1_TOP) / 255;
	
	if (value > 0) {
		switch (channel) {
		case 0:
			TCCR1A |= 
				(1 << COM1A1);
			OCR1A = value;
			break;
		case 1:
			TCCR1A |= 
				(1 << COM1B1);
			OCR1B = value;
			break;
		case 2:
			TCCR1A |= 
				(1 << COM1C1);
			OCR1C = value;
			break;
		default:
			return;
		}
	} else {
		switch (channel) {
		case 0:
			TCCR1A &= 
				~(1 << COM1A1);
			break;
		case 1:
			TCCR1A &= 
				~(1 << COM1B1);
			break;
		case 2:
			TCCR1A &= 
				~(1 << COM1C1);
			break;
		default:
			return;
		}
	}
	
}
