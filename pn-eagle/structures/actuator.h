#ifndef INC_ACTUATOR_H_
#define INC_ACTUATOR_H_

#define ACTUATOR_PROCESS_PERIOD		1		// ms
#define ACTUATOR_HOMING_STOP_TIME	2000	// ms
#define ACTUATOR_HOMING_DUTY		150

#define ACTUATOR_PID_KP				40000
#define ACTUATOR_PID_KI				500
#define ACTUATOR_PID_KD				1000
#define ACTUATOR_INTEGRAL_LIMIT		100

// PD 30000, 1000

#include <stdint.h>
#include "gpio.h"
#include "encoder.h"
#include "pid.h"

typedef enum {
	NORMAL,
	HOMING_COARSE,
	HOMING_RETURN,
	HOMING_FINE,
	ERROR
} actuator_state_t;

typedef struct {
	const gpio_t *dirGpio;
	uint8_t dirPin;
	uint8_t pwmChannel;
	encoder_t *encoder;
	pid_t pid;
	
	actuator_state_t state;
	uint32_t lastTime;
	
	int16_t targetPos;
	int16_t speed;
	int16_t lastSpeed;
	uint8_t homingDone;
	uint16_t homingStopTime;
} actuator_t;

void actuator_init(actuator_t* actuator, const gpio_t *dirGpio, uint8_t dirPin, uint8_t pwmChannel, encoder_t *encoder);
void actuator_startHoming(actuator_t* actuator);
void actuator_setTargetPos(actuator_t* actuator, int16_t targetPos);
int16_t actuator_getTargetPos(actuator_t* actuator);
int16_t actuator_getCurrentPos(actuator_t* actuator);

void actuator_process(actuator_t* actuator, uint32_t currentTime);

#endif