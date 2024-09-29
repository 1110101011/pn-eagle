#ifndef TIMER1_H_
#define TIMER1_H_

#define TIMER1_TOP			666
#define TIMER1_CHANNEL_NUM	3

#include <avr/io.h>

void timer1_init(void);
void timer1_setChannelDuty(uint8_t channel, uint8_t duty);

#endif /* TIMER1_H_ */