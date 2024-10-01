#include "pid.h"

void pid_init(pid_t* pid, int32_t kp, int32_t ki, int32_t kd, int32_t outputMin, int32_t outputMax, int32_t integralLimit) {
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->outputMin = outputMin;
	pid->outputMax = outputMax;
	pid->integralLimit = integralLimit;
	pid->prev_error = 0;
	pid->integral = 0;
	pid->setpoint = 0;
}

int16_t pid_update(pid_t* pid, int16_t currentValue) {
	int32_t error = pid->setpoint - currentValue;
	int32_t derivative = error - pid->prev_error;

	if (error == 0) {
		pid->integral = 0;
	}

	pid->integral += error;
	if (pid->integral > pid->integralLimit) {
		pid->integral = pid->integralLimit;
	} else if (pid->integral < -pid->integralLimit) {
		pid->integral = -pid->integralLimit;
	}

	int32_t output = (pid->kp * error + pid->ki * pid->integral + pid->kd * derivative) / PID_SCALE_FACTOR;

	if (output > pid->outputMax) {
		output = pid->outputMax;
	} else if (output < pid->outputMin) {
		output = pid->outputMin;
	}

	pid->prev_error = error;
	return (int16_t) output;
}

void pid_setSetpoint(pid_t* pid, int32_t setpoint) {
	pid->setpoint = setpoint;
	pid->integral = 0; 
}