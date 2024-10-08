#ifndef CONFIG_H_
#define CONFIG_H_

#define VERSION_STRING					"v1.05"
#define CONF_UART_BAUD					9600

#define CONF_ACTUATOR_COUNT				3		// liczba silownikow
#define CONF_ACTUATOR_OFFSET			5		// liczba impulsow o ktore ma sie wysunac silownik po bazowaniu
#define CONF_ACTUATOR_RANGE				505		// zakres pracy silownika liczony od punktu offset

#define CONF_ACTUATOR_ATTEMPT_COUNT		3		// maksymalna liczba prob osiagniecia pozycji
#define CONF_ACTUATOR_ATTEMPT_TIME		500		// czas bezruchu po ktorym proba osiagniecia pozycji ma byc przerwana

#endif /* CONFIG_H_ */