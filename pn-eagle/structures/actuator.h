#ifndef INC_ACTUATOR_H_
#define INC_ACTUATOR_H_

#define ACTUATOR_PROCESS_PERIOD			1		// ms

#define ACTUATOR_PID_KP					5000
#define ACTUATOR_PID_KI					500
#define ACTUATOR_PID_KD					2400
#define ACTUATOR_PID_INTEGRAL_LIMIT		200
#define ACTUATOR_PID_RAMP_RATE			5

#include <stdint.h>
#include "gpio.h"
#include "encoder.h"
#include "pid.h"

typedef enum {
	STATE_NORMAL,
	STATE_ATTEMPTING_RETURN,
	STATE_ATTEMPTING,
	STATE_ERROR,
	STATE_HOMING_COARSE,
	STATE_HOMING_RETURN,
	STATE_HOMING_FINE
} actuator_state_t;

typedef enum {
	ERROR_OK = 0x00,
	ERROR_BLOCKED = 0x01,
	ERROR_OK_HOMING = 0x02	// tymczasowe
} actuator_error_t;

typedef struct {
	const gpio_t *dirGpio;
	uint8_t dirPin;
	uint8_t pwmChannel;
	encoder_t *encoder;
	pid_t pid;
	uint8_t reverse;
	
	actuator_state_t state;
	actuator_error_t errorCode;
	uint32_t lastTime;	
	uint16_t stopTime;
	int16_t targetPos;
	int16_t speed;
	int16_t lastSpeed;
	uint16_t homingTime;
	
	uint8_t attemptCount;
	int16_t blockedPos;
} actuator_t;

void actuator_init(actuator_t* actuator, const gpio_t *dirGpio, uint8_t dirPin, uint8_t pwmChannel, encoder_t *encoder, uint8_t reverse);
void actuator_startHoming(actuator_t* actuator);
void actuator_setTargetPos(actuator_t* actuator, int16_t targetPos);
int16_t actuator_getTargetPos(actuator_t* actuator);
int16_t actuator_getCurrentPos(actuator_t* actuator);
actuator_error_t actuator_getErrorCode(actuator_t* actuator);

void actuator_process(actuator_t* actuator, uint32_t currentTime);

#endif