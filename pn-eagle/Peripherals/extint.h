#ifndef EXTINT_H_
#define EXTINT_H_

#include <stdint.h>

void extint_init(void (*edgeEvent)(uint8_t, int8_t));
void extint_poll(void);

#endif /* EXTINT_H_ */