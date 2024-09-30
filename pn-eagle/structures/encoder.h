#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>

typedef struct {
	int16_t count;
	int16_t speed;
	int16_t lastCount;
	uint32_t lastTime;
} encoder_t;

void encoder_init(encoder_t *encoder);
void encoder_update(encoder_t *encoder, int8_t direction);
void encoder_process(encoder_t *encoder, uint32_t currentTime);

#endif /* ENCODER_H_ */