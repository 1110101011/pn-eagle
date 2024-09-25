#include <string.h>
#include "logger.h"

static logger_t logger;

void logger_init(void (*stringToSendCallback)(const char*)) {
	logger.stringToSendCallback = stringToSendCallback;
}

void logger_print(const char *buf) {
	logger.stringToSendCallback(buf);
}

void logger_println(const char *buf) {
	logger_print(buf);
}

void logger_printValue(const char *buf, uint16_t value) {
	uint8_t digit;
	uint8_t i = 4;
	char strValue[6] = {0};

	logger_print(buf);

	do {
		digit = value % 10;
		strValue[i] = '0' + digit;
		value /= 10;
		i--;
	} while (value);

	logger_print(&strValue[i+1]);
}
