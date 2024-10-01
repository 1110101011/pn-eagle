#include "pid.h"

// Inicjalizacja struktury PID
void pid_init(pid_t* pid, int32_t kp, int32_t ki, int32_t kd, int32_t output_min, int32_t output_max, int32_t integral_limit) {
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->output_min = output_min;
	pid->output_max = output_max;
	pid->integral_limit = integral_limit;
	pid->prev_error = 0;
	pid->integral = 0;
	pid->setpoint = 0; // Domy�lna warto�� zadana na pocz�tek
}

// Funkcja aktualizuj�ca PID
int16_t pid_update(pid_t* pid, int16_t current_value) {
	int32_t error = pid->setpoint - current_value;
	int32_t derivative = error - pid->prev_error;

	// Reset cz�onu ca�kuj�cego je�li jeste�my na zadanej pozycji
	if (error == 0) {
		pid->integral = 0;  // Zerowanie cz�onu ca�kuj�cego przy braku b��du
	}

	// Ca�kowanie z ograniczeniem
	pid->integral += error;
	if (pid->integral > pid->integral_limit) {
		pid->integral = pid->integral_limit;
		} else if (pid->integral < -pid->integral_limit) {
		pid->integral = -pid->integral_limit;
	}

	// Wyj�cie PID: Kp*error + Ki*integral + Kd*derivative
	int32_t output = (pid->kp * error + pid->ki * pid->integral + pid->kd * derivative) / PID_SCALE_FACTOR;

	// Ograniczenie wyj�cia
	if (output > pid->output_max) {
		output = pid->output_max;
	} else if (output < pid->output_min) {
		output = pid->output_min;
	}

	pid->prev_error = error;
	return (int16_t)output;
}

// Funkcja ustawiaj�ca warto�� zadan� (setpoint)
void pid_setSetpoint(pid_t* pid, int32_t setpoint) {
	pid->setpoint = setpoint;
	pid->integral = 0;  // Reset cz�onu ca�kuj�cego, gdy ustawiamy now� warto�� zadan�
}