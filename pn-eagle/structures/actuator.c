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
	actuator->lastSpeed = 0;
	actuator->homingStopTime = 0;

	pid_init(&actuator->pid, ACTUATOR_PID_KP, ACTUATOR_PID_KI, ACTUATOR_PID_KD, -255, 255, ACTUATOR_INTEGRAL_LIMIT);
	gpio_pinConfig(actuator->dirGpio, actuator->dirPin, OUTPUT);
}

void actuator_startHoming(actuator_t* actuator) {
	if (actuator->state == HOMING_COARSE || actuator->state == HOMING_RETURN || actuator->state == HOMING_FINE) {
		return;
	}
	
	actuator->speed = -255;
	
	actuator->state = HOMING_COARSE;
	actuator->homingStopTime = 0;
	actuator->homingDone = 0;
	actuator->targetPos = 0;
	pid_setSetpoint(&actuator->pid, 0);
}

void actuator_setTargetPos(actuator_t* actuator, int16_t targetPos) {
	if (actuator->state == HOMING_COARSE || actuator->state == HOMING_RETURN || actuator->state == HOMING_FINE) {
		return;
	}
	
	actuator->targetPos = targetPos;
	
	pid_setSetpoint(&actuator->pid, targetPos);
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
		actuator->speed = pid_update(&actuator->pid, actuator->encoder->count);		
		break;
		
	case HOMING_COARSE:
		if (abs(actuator->encoder->speed) > 0 ) {
			break;
		} else {
			actuator->homingStopTime += timeDelta;
			
			if (actuator->homingStopTime >= 2000) {
				actuator->encoder->count = 0;
				actuator->homingStopTime = 0;
				actuator->speed = 150;
				actuator->state = HOMING_RETURN;
			}	
		}	
		break;
	
	case HOMING_RETURN:
		if (actuator->encoder->count >= 30) {
			actuator->speed = -120;
			actuator->state = HOMING_FINE;
		}
		break;
	
	case HOMING_FINE:
		if (abs(actuator->encoder->speed) > 0 ) {
			break;
		} else {
			actuator->homingStopTime += timeDelta;
			if (actuator->homingStopTime >= 2000) {
				actuator->speed = 0;
				actuator->encoder->count = 0;
				actuator->homingStopTime = 0;
				actuator->homingDone = 1;
				actuator->state = NORMAL;
			}	
		}
	case ERROR:
	
		break;
	}
	
	actuator_control(actuator, actuator->speed);
}

static void actuator_control(actuator_t* actuator, int16_t speed) {
	if (speed > 0) {
		gpio_pinSet(actuator->dirGpio, actuator->dirPin, 0);
	} else if (speed < 0) {
		gpio_pinSet(actuator->dirGpio, actuator->dirPin, 1);
	}
	
	if (actuator->speed == actuator->lastSpeed) {
		return;
	}
	
	timer1_setChannelDuty(actuator->pwmChannel, abs(speed));
	actuator->lastSpeed = actuator->speed;
}
