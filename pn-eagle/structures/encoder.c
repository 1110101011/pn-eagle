#include "encoder.h"

void encoder_init(encoder_t *encoder) {
	encoder->count = 0;
	encoder->speed = 0;
	encoder->lastCount = 0;
	encoder->lastTime = 0;
}

void encoder_update(encoder_t *encoder, int8_t direction) {
	encoder->count += direction;
}

void encoder_process(encoder_t *encoder, uint32_t current_time) {
	uint32_t timeDelta = current_time - encoder->lastTime;

	if (timeDelta >= 100) {  // 100ms
		encoder->speed = (encoder->count - encoder->lastCount) * (1000 / timeDelta);
		encoder->lastCount = encoder->count;
		
		encoder->lastTime = current_time;
	}
}
