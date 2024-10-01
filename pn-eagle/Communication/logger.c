#include <string.h>
#include "logger.h"

const char* logger_endl = "\n";

static void (*stringToSendCallback)(const char *buffer);

void logger_init(void (*stringToSendEvent)(const char*)) {
	stringToSendCallback = stringToSendEvent;
}

void logger_print(const char *buf) {
	stringToSendCallback(buf);
}

void logger_println(const char *buf) {
	logger_print(buf);
	logger_print(logger_endl);
}

void logger_printValue(const char *buf, int16_t value) {
	char strValue[10]; 
	uint8_t i = 0;

	logger_print(buf);

	if (value < 0) {
		strValue[i++] = '-'; 
		value = -value;     
	}

	if (value == 0) {
		strValue[i++] = '0';
	} else {
		uint16_t tempValue = value;
		uint8_t digits = 0;

		while (tempValue > 0) {
			tempValue /= 10;
			digits++;
		}

		tempValue = value;
		for (int j = digits - 1; j >= 0; j--) {
			strValue[i + j] = '0' + (tempValue % 10);
			tempValue /= 10;
		}
		i += digits; 
	}

	strValue[i++] = '\n';
	strValue[i] = '\0';  

	logger_print(strValue);
}
