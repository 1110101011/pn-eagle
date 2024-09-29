#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>

extern const char* logger_endl;

void logger_init(void (*stringToSendEvent)(const char*));
void logger_print(const char *buf);
void logger_println(const char *buf);
void logger_printValue(const char *buf, int16_t value);

#endif /* LOGGER_H_ */