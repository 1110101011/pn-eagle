#ifndef PID_H_
#define PID_H_

#define PID_SCALE_FACTOR	1000

#include <stdint.h>

typedef struct {
	int32_t kp;          // Wzmocnienie proporcjonalne
	int32_t ki;          // Wzmocnienie ca³kuj¹ce
	int32_t kd;          // Wzmocnienie ró¿niczkuj¹ce
	int32_t setpoint;    // Wartoœæ zadana
	int32_t prev_error;  // Poprzedni b³¹d
	int32_t integral;    // Suma b³êdów do ca³kowania
	int32_t output_min;  // Minimalne wyjœcie
	int32_t output_max;  // Maksymalne wyjœcie
	int32_t integral_limit; // Limit ca³kowania
} pid_t;

// Inicjalizacja regulatora PID
void pid_init(pid_t* pid, int32_t kp, int32_t ki, int32_t kd, int32_t output_min, int32_t output_max, int32_t integral_limit);

// Aktualizacja wartoœci PID
int16_t pid_update(pid_t* pid, int16_t current_value);

void pid_setSetpoint(pid_t* pid, int32_t setpoint);

#endif /* PID_H_ */