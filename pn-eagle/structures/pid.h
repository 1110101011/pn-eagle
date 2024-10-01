#ifndef PID_H_
#define PID_H_

#define PID_SCALE_FACTOR	1000

#include <stdint.h>

typedef struct {
	int32_t kp;          // Wzmocnienie proporcjonalne
	int32_t ki;          // Wzmocnienie ca�kuj�ce
	int32_t kd;          // Wzmocnienie r�niczkuj�ce
	int32_t setpoint;    // Warto�� zadana
	int32_t prev_error;  // Poprzedni b��d
	int32_t integral;    // Suma b��d�w do ca�kowania
	int32_t output_min;  // Minimalne wyj�cie
	int32_t output_max;  // Maksymalne wyj�cie
	int32_t integral_limit; // Limit ca�kowania
} pid_t;

// Inicjalizacja regulatora PID
void pid_init(pid_t* pid, int32_t kp, int32_t ki, int32_t kd, int32_t output_min, int32_t output_max, int32_t integral_limit);

// Aktualizacja warto�ci PID
int16_t pid_update(pid_t* pid, int16_t current_value);

void pid_setSetpoint(pid_t* pid, int32_t setpoint);

#endif /* PID_H_ */