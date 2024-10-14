#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

static inline int16_t fast_atoin(const char* str, uint8_t n) {
	int16_t val = 0;
	
	for (uint8_t i = 0; i < n; i++){
		val = val*10 + (*str++ - '0');
	}
	
	return val;
}

#endif /* UTIL_H_ */