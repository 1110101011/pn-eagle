#include <avr/interrupt.h>
#include "sys.h"

volatile uint32_t sys_time = 0;

ISR(TIMER2_COMP_vect) {
	sys_time++;
}

void sys_init(void) {
	// System timer 1ms
	TCCR2 |=
		(1 << WGM21) |
		(1 << CS21)  | (1 << CS20);  // clk/64	
		
	OCR2 = 249;
		
	TIMSK |=
		(1 << OCIE2);
}
