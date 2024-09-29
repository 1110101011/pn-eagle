#include "actuator.h"
#include "timer1.h"
#include <stdlib.h>

static void actuator_control(actuator_t* actuator, int16_t speed);

void actuator_init(actuator_t* actuator, const gpio_t *dirGpio, uint8_t dirPin, uint8_t pwmChannel) {
	actuator->dirGpio = dirGpio;
	actuator->dirPin = dirPin;
	actuator->pwmChannel = pwmChannel;
	
	actuator->currentPos = 0;
	actuator->targetPos = 0;
	actuator->speed = 0;
	
	gpio_pinConfig(actuator->dirGpio, actuator->dirPin, OUTPUT);
}

void actuator_setTargetPos(actuator_t* actuator, int16_t targetPos) {
	actuator->targetPos = targetPos;
}

void actuator_setCurrentPos(actuator_t* actuator, int16_t currentPos) {
	actuator->currentPos = currentPos;
}

void actuator_process1ms(actuator_t* actuator) {
	int16_t error = actuator->targetPos - actuator->currentPos;
	
	if (error > 10) {
		actuator_control(actuator, 170);
	} else if (error < -10) {
		actuator_control(actuator, -170);
	} else {
		actuator_control(actuator, 0);
	}
}

static void actuator_control(actuator_t* actuator, int16_t speed) {
	if (actuator->speed == speed) {
		return;
	}
	
	uint8_t absSpeed = abs(speed);
	
	if (speed > 0) {
		gpio_pinSet(actuator->dirGpio, actuator->dirPin, 1);
	} else if (speed < 0) {
		gpio_pinSet(actuator->dirGpio, actuator->dirPin, 0);
	}
	
	actuator->speed = speed;
	timer1_setChannelDuty(actuator->pwmChannel, absSpeed);
}