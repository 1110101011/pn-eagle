#ifndef INC_ACTUATOR_H_
#define INC_ACTUATOR_H_

#define ACTUATOR_PROCESS_PERIOD 100  

#include "gpio.h"
#include <stdint.h>

typedef struct {
	const gpio_t *dirGpio;
	uint8_t dirPin;
	uint8_t pwmChannel;
	
	int16_t currentPos;
	int16_t targetPos;
	int16_t speed;
} actuator_t;

void actuator_init(actuator_t* actuator, const gpio_t *dirGpio, uint8_t dirPin, uint8_t pwmChannel);
void actuator_setTargetPos(actuator_t* actuator, int16_t targetPos);
void actuator_setCurrentPos(actuator_t* actuator, int16_t currentPos);

void actuator_process1ms(actuator_t* actuator);

#endif