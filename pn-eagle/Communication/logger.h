#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>

typedef struct {
	void (*stringToSendCallback)(const char *buffer);
} logger_t;

void logger_init(void (*stringToSendCallback)(const char*));
void logger_print(const char *buf);
void logger_println(const char *buf);
void logger_printValue(const char *buf, uint16_t value);

#endif /* LOGGER_H_ */