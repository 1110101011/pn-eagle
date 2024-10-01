#ifndef PID_H_
#define PID_H_

#define PID_SCALE_FACTOR	1000

#include <stdint.h>

typedef struct {
	int32_t kp;       
	int32_t ki;        
	int32_t kd;        
	int32_t setpoint;  
	int32_t prev_error; 
	int32_t integral;  
	int32_t outputMin; 
	int32_t outputMax;
	int32_t integralLimit;
} pid_t;

void pid_init(pid_t* pid, int32_t kp, int32_t ki, int32_t kd, int32_t outputMin, int32_t outputMax, int32_t integralLimit);
int16_t pid_update(pid_t* pid, int16_t currentValue);
void pid_setSetpoint(pid_t* pid, int32_t setpoint);

#endif /* PID_H_ */