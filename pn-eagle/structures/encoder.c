#include "encoder.h"

void encoder_init(encoder_t *encoder, uint8_t reversed) {
	encoder->reversed = reversed;
	encoder->count = 0;
	encoder->speed = 0;
	encoder->lastCount = 0;
	encoder->lastTime = 0;
}

void encoder_update(encoder_t *encoder, int8_t direction) {
	if (encoder->reversed) {
		encoder->count -= direction;
	} else {
		encoder->count += direction;	
	}
}

void encoder_process(encoder_t *encoder, uint32_t currentTime) {
	uint32_t timeDelta = currentTime - encoder->lastTime;

	if (timeDelta >= 10) {  // 10ms
		encoder->speed = (encoder->count - encoder->lastCount) * (1000 / timeDelta);
		
		if (encoder->reversed) {
			encoder->speed = -(encoder->speed);
		}
		
		encoder->lastCount = encoder->count;
		
		encoder->lastTime = currentTime;
	}
}

void encoder_setCount(encoder_t *encoder, int16_t count) {
	encoder->count = count;
}

int16_t encoder_getCount(encoder_t *encoder) {
	return encoder->count;
}

int16_t encoder_getSpeed(encoder_t *encoder) {
	return encoder->speed;
}