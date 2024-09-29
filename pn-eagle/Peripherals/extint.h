#ifndef EXTINT_H_
#define EXTINT_H_

#include <stdint.h>

void extint_init(void (*edgeEvent)(uint8_t, uint8_t));

#endif /* EXTINT_H_ */