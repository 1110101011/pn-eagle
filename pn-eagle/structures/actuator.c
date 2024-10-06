#include "actuator.h"
#include "timer1.h"
#include <stdlib.h>

static void actuator_control(actuator_t* actuator, int16_t speed);

void actuator_init(actuator_t* actuator, const gpio_t *dirGpio, uint8_t dirPin, uint8_t pwmChannel, encoder_t *encoder) {
	actuator->dirGpio = dirGpio;
	actuator->dirPin = dirPin;
	actuator->pwmChannel = pwmChannel;
	actuator->encoder = encoder;
	
	actuator->state = STATE_NORMAL;
	
	actuator->errorCode = ERROR_OK;
	actuator->lastTime = 0;
	actuator->stopTime = 0;
	actuator->targetPos = 0;
	actuator->speed = 0;
	actuator->lastSpeed = 0;
	actuator->homingDone = 0;
	actuator->homingTime = 0;

	pid_init(&actuator->pid, ACTUATOR_PID_KP, ACTUATOR_PID_KI, ACTUATOR_PID_KD, -255, 255, ACTUATOR_INTEGRAL_LIMIT);
	gpio_pinConfig(actuator->dirGpio, actuator->dirPin, OUTPUT);
}

void actuator_startHoming(actuator_t* actuator) {
	if (actuator->state == STATE_HOMING_COARSE || actuator->state == STATE_HOMING_RETURN || actuator->state == STATE_HOMING_FINE) {
		return;
	}
	
	actuator->state = STATE_HOMING_COARSE;
	actuator->errorCode = ERROR_OK_HOMING;
	actuator->speed = -255;
	actuator->homingTime = 0;
	actuator->stopTime = 0;
	actuator->homingDone = 0;
	actuator->targetPos = 0;
	pid_setSetpoint(&actuator->pid, 0);
}

void actuator_setTargetPos(actuator_t* actuator, int16_t targetPos) {
	if (actuator->state == STATE_ERROR || actuator->state == STATE_HOMING_COARSE || actuator->state == STATE_HOMING_RETURN || actuator->state == STATE_HOMING_FINE) {
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

actuator_error_t actuator_getErrorCode(actuator_t* actuator) {
	return actuator->errorCode;
}

void actuator_process(actuator_t* actuator, uint32_t currentTime) {
	uint16_t timeDelta = currentTime - actuator->lastTime;
	
	if (timeDelta < ACTUATOR_PROCESS_PERIOD) {
		return;
	}
	
	actuator->lastTime = currentTime;
	
	switch (actuator->state) {
	case STATE_NORMAL:
		actuator->speed = pid_update(&actuator->pid, actuator->encoder->count);	
		
		if ((abs(actuator->speed) > 0) && (abs(actuator->encoder->speed) == 0)) {
			actuator->stopTime += timeDelta;
			
			if (actuator->stopTime >= 2000) {
				actuator->targetPos = actuator->encoder->count;
				actuator->speed = 0;
				actuator->stopTime = 0;	
				actuator->errorCode = ERROR_BLOCKED;
				actuator->state = STATE_ERROR;
			}
		} else {
			actuator->stopTime = 0;	
		}		
		break;
		
	case STATE_HOMING_COARSE:
		if (abs(actuator->encoder->speed) == 0) {
			actuator->stopTime += timeDelta;
			
			if (actuator->stopTime >= 2000) {
				actuator->homingTime = 0;
				actuator->stopTime = 0;
				actuator->encoder->count = 0;
				actuator->speed = 200;
				actuator->state = STATE_HOMING_RETURN;
			}	
		} else {
			actuator->homingTime += timeDelta;
			actuator->stopTime = 0;
			
			if (actuator->homingTime >= 10000) {
				actuator->speed = 0;
				actuator->encoder->count = 0;
				actuator->errorCode = ERROR_BLOCKED;
				actuator->state = STATE_ERROR;
			}
		}
		break;
	
	case STATE_HOMING_RETURN:
		if (actuator->encoder->count >= 30) {
			actuator->speed = -120;
			actuator->homingTime = 0;
			actuator->state = STATE_HOMING_FINE;
		} else {
			actuator->homingTime += timeDelta;
			
			if (actuator->homingTime >= 5000) {
				actuator->speed = 0;
				actuator->encoder->count = 0;
				actuator->errorCode = ERROR_BLOCKED;
				actuator->state = STATE_ERROR;
			}
		}
		break;
	
	case STATE_HOMING_FINE:
		if (abs(actuator->encoder->speed) == 0) {
			actuator->stopTime += timeDelta;
			
			if (actuator->stopTime >= 2000) {
				actuator->speed = 0;
				actuator->encoder->count = -5;
				actuator->stopTime = 0;
				actuator->homingDone = 1;
				actuator->errorCode = ERROR_OK;
				actuator->state = STATE_NORMAL;
			}	
		} else {
			actuator->homingTime += timeDelta;
			actuator->stopTime = 0;
			
			if (actuator->homingTime >= 5000) {
				actuator->speed = 0;
				actuator->encoder->count = 0;
				actuator->errorCode = ERROR_BLOCKED;
				actuator->state = STATE_ERROR;
			}
		}
	
	case STATE_ERROR:
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
