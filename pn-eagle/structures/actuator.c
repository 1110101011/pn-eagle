#include "actuator.h"
#include "timer1.h"
#include <stdlib.h>

static void actuator_control(actuator_t* actuator, int16_t speed);

void actuator_init(actuator_t* actuator, const gpio_t *dirGpio, uint8_t dirPin, uint8_t pwmChannel, encoder_t *encoder) {
	actuator->dirGpio = dirGpio;
	actuator->dirPin = dirPin;
	actuator->pwmChannel = pwmChannel;
	actuator->encoder = encoder;
	
	actuator->state = NORMAL;
	actuator->homingDone = 0;
	actuator->lastTime = 0;
	
	actuator->targetPos = 0;
	actuator->speed = 0;
	actuator->homingStopTime = 0;

	gpio_pinConfig(actuator->dirGpio, actuator->dirPin, OUTPUT);
}

void actuator_startHoming(actuator_t* actuator) {
	if (actuator->state == HOMING) {
		return;
	}
	
	actuator->state = HOMING;
	actuator->homingStopTime = 0;
}

void actuator_setTargetPos(actuator_t* actuator, int16_t targetPos) {
	actuator->targetPos = targetPos;
}

int16_t actuator_getTargetPos(actuator_t* actuator) {
	return actuator->targetPos;
}

int16_t actuator_getCurrentPos(actuator_t* actuator) {
	return actuator->encoder->count;
}

void actuator_process(actuator_t* actuator, uint32_t currentTime) {
	int16_t error;
	uint32_t timeDelta = currentTime - actuator->lastTime;
	
	if (timeDelta < ACTUATOR_PROCESS_PERIOD) {
		return;
	}
	
	actuator->lastTime = currentTime;;
	
	switch (actuator->state) {
	case NORMAL:
		error = actuator->targetPos - actuator->encoder->count;
	
		if (error > 0) {
			actuator_control(actuator, 255);
		} else if (error < 0) {
			actuator_control(actuator, -255);
		} else {
			actuator_control(actuator, 0);
		}
		
		break;
	case HOMING:
		actuator_control(actuator, -ACTUATOR_HOMING_DUTY);
		
		if (abs(actuator->encoder->speed) > 0 ) {
			break;
		} else {
			actuator->homingStopTime += timeDelta;
			
			if (actuator->homingStopTime >= ACTUATOR_HOMING_STOP_TIME) {
				actuator_control(actuator, 0);
				actuator->homingDone = 1;
				actuator->encoder->count = 0;
				actuator->state = NORMAL;
			}	
		}	
		break;
	}
	
}

static void actuator_control(actuator_t* actuator, int16_t speed) {
	if (actuator->speed == speed) {
		return;
	}
	
	uint8_t absSpeed = abs(speed);
	
	if (speed > 0) {
		gpio_pinSet(actuator->dirGpio, actuator->dirPin, 0);
	} else if (speed < 0) {
		gpio_pinSet(actuator->dirGpio, actuator->dirPin, 1);
	}
	
	actuator->speed = speed;
	timer1_setChannelDuty(actuator->pwmChannel, absSpeed);
}