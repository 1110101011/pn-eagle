#include <stdlib.h>
#include "actuator.h"
#include "timer1.h"
#include "config.h"

static inline uint8_t actuator_isAtTarget(actuator_t* actuator);
static inline uint8_t actuator_isStopped(actuator_t* actuator);
static inline uint8_t actuator_isBlocked(actuator_t* actuator);
static  void actuator_control(actuator_t* actuator, int16_t speed);

void actuator_init(actuator_t* actuator, const gpio_t *dirGpio, uint8_t dirPin, uint8_t pwmChannel, encoder_t *encoder, uint8_t reverse) {
	actuator->dirGpio = dirGpio;
	actuator->dirPin = dirPin;
	actuator->pwmChannel = pwmChannel;
	actuator->encoder = encoder;
	actuator->reverse = reverse;
	
	actuator->state = STATE_NORMAL;
	
	actuator->errorCode = ERROR_OK;
	actuator->lastTime = 0;
	actuator->stopTime = 0;
	actuator->targetPos = 0;
	actuator->speed = 0;
	actuator->lastSpeed = 0;
	actuator->homingTime = 0;
	actuator->attemptCount = 0;

	pid_init(&actuator->pid, ACTUATOR_PID_KP, ACTUATOR_PID_KI, ACTUATOR_PID_KD, -255, 255, ACTUATOR_PID_INTEGRAL_LIMIT, ACTUATOR_PID_RAMP_RATE);
	gpio_pinConfig(actuator->dirGpio, actuator->dirPin, OUTPUT);
}

void actuator_startHoming(actuator_t* actuator) {
	if (actuator->state == STATE_HOMING_RETURN || actuator->state == STATE_HOMING) {
		return;
	}
	
	actuator->state = STATE_HOMING_RETURN;
	actuator->errorCode = ERROR_OK_HOMING;
	actuator->homingTime = 0;
	actuator->stopTime = 0;
	actuator->targetPos = 0;
	
	pid_setSetpoint(&actuator->pid, -1000);
}

void actuator_setTargetPos(actuator_t* actuator, int16_t targetPos) {
	if (actuator->state == STATE_HOMING_RETURN || actuator->state == STATE_HOMING || actuator->state == STATE_ATTEMPTING || actuator->state == STATE_ATTEMPTING_RETURN) {
		return;
	}
	
	if (targetPos > (CONF_ACTUATOR_RANGE * 2)) {
		targetPos = (CONF_ACTUATOR_RANGE * 2);
	}
	
	if (actuator->targetPos == targetPos) {
		return;
	}
	
	actuator->targetPos = targetPos;
	actuator->state = STATE_NORMAL;
	actuator->errorCode = ERROR_OK;
	actuator->attemptCount = 0;
	
	pid_setSetpoint(&actuator->pid, targetPos);
}

int16_t actuator_getTargetPos(actuator_t* actuator) {
	return actuator->targetPos;
}

int16_t actuator_getCurrentPos(actuator_t* actuator) {
	return encoder_getCount(actuator->encoder);
}

actuator_error_t actuator_getErrorCode(actuator_t* actuator) {
	return actuator->errorCode;
}

void actuator_process(actuator_t* actuator, uint32_t currentTime) {
	uint16_t timeDelta = currentTime - actuator->lastTime;
	
	if (timeDelta < ACTUATOR_PROCESS_PERIOD) {
		return;
	} else {
		actuator->lastTime = currentTime;
	}
	
	if (actuator->state == STATE_NORMAL || actuator->state == STATE_ATTEMPTING || actuator->state == STATE_ATTEMPTING_RETURN || actuator->state == STATE_HOMING_RETURN) {
		int16_t pos = actuator_getCurrentPos(actuator);
		actuator->speed = pid_update(&actuator->pid, pos);	
	}
	
	switch (actuator->state) {
	case STATE_NORMAL:
		if (actuator_isBlocked(actuator)) {
			actuator->stopTime += timeDelta;
			
			if (actuator->stopTime >= CONF_ACTUATOR_ATTEMPT_TIME) {
				actuator->stopTime = 0;
				actuator->blockedPos = actuator_getCurrentPos(actuator);
				actuator->attemptCount = 0;
				
				if (actuator->blockedPos < actuator->targetPos) {
					pid_setSetpoint(&actuator->pid, actuator->blockedPos - 40);
				} else {
					pid_setSetpoint(&actuator->pid, actuator->blockedPos + 40);
				}
				
				actuator->state = STATE_ATTEMPTING_RETURN;
			}
		} else {
			actuator->stopTime = 0;	
		}		
		break;
		
	case STATE_ATTEMPTING:
		if (actuator_isBlocked(actuator)) {
			actuator->stopTime += timeDelta;
		
			if (actuator->stopTime >= CONF_ACTUATOR_ATTEMPT_TIME) {
				if (actuator->attemptCount++ < CONF_ACTUATOR_ATTEMPT_COUNT - 1) {
					actuator->blockedPos = actuator_getCurrentPos(actuator);
					actuator->stopTime = 0;
					
					if (actuator->blockedPos < actuator->targetPos) {
						pid_setSetpoint(&actuator->pid, actuator->blockedPos - 40);
					} else {
						pid_setSetpoint(&actuator->pid, actuator->blockedPos + 40);
					}
					
					actuator->state = STATE_ATTEMPTING_RETURN; 
				} else {
					actuator->speed = 0;
					actuator->stopTime = 0;
					actuator->errorCode = ERROR_BLOCKED;
					actuator->state = STATE_ERROR; 
				}
			}
		} 
		break;
	
	case STATE_ATTEMPTING_RETURN:
		if (abs(actuator_getCurrentPos(actuator) - actuator->blockedPos) >= 40) {
			pid_setSetpoint(&actuator->pid, actuator->targetPos);
			actuator->state = STATE_ATTEMPTING;
		}
		break;

	case STATE_HOMING_RETURN:
		if (actuator_isStopped(actuator)) {
			actuator->stopTime += timeDelta;
			
			if (actuator->stopTime >= 2000) {
				actuator->homingTime = 0;
				actuator->stopTime = 0;
				encoder_setCount(actuator->encoder, 0);
				actuator->speed = 120;
				actuator->state = STATE_HOMING;
			}
		} else {
			actuator->homingTime += timeDelta;
			actuator->stopTime = 0;
			
			if (actuator->homingTime >= 10000) {
				actuator->speed = 0;
				encoder_setCount(actuator->encoder, 0);
				actuator->errorCode = ERROR_BLOCKED;
				actuator->state = STATE_ERROR;
			}
		}
		break;
	
	case STATE_HOMING:
		if (encoder_getCount(actuator->encoder) > 0) {
			actuator->speed = 0;
			encoder_setCount(actuator->encoder, -CONF_ACTUATOR_OFFSET * 2);
			pid_setSetpoint(&actuator->pid, 0);
			actuator->stopTime = 0;
			actuator->errorCode = ERROR_OK;
			actuator->state = STATE_NORMAL;
		} else {
			actuator->homingTime += timeDelta;
			
			if (actuator->homingTime >= 5000) {
				actuator->speed = 0;
				encoder_setCount(actuator->encoder, 0);
				actuator->errorCode = ERROR_BLOCKED;
				actuator->state = STATE_ERROR;
			}
		}
		break;
		
	case STATE_ERROR:
		actuator->speed = 0;
		break;
	}
	
	actuator_control(actuator, actuator->speed);
}

static inline uint8_t actuator_isAtTarget(actuator_t* actuator) {
	int16_t pos = actuator_getCurrentPos(actuator);
	return (actuator->targetPos == pos);
}

static inline uint8_t actuator_isStopped(actuator_t* actuator) {
	int16_t speed = encoder_getSpeed(actuator->encoder);
	return (speed == 0);
}

static inline uint8_t actuator_isBlocked(actuator_t* actuator) {
	int16_t speed = encoder_getSpeed(actuator->encoder);
	return (abs(actuator->speed) > 0) && (speed == 0);
}

static void actuator_control(actuator_t* actuator, int16_t speed) {
	if ((speed > 0 && !actuator->reverse) || (speed < 0 && actuator->reverse)) {
		gpio_pinSet(actuator->dirGpio, actuator->dirPin, 0);
	} else {
		gpio_pinSet(actuator->dirGpio, actuator->dirPin, 1);
	}
	
	if (actuator->speed == actuator->lastSpeed) {
		return;
	}
	
	timer1_setChannelDuty(actuator->pwmChannel, abs(speed));
	actuator->lastSpeed = actuator->speed;
}
