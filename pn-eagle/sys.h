#ifndef SYS_H_
#define SYS_H_

#define F_CPU 16000000UL

#include <stdint.h>

extern volatile uint32_t sys_time;

void sys_init(void);

#endif /* SYS_H_ */