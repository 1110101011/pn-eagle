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

/*void logger_printValue(const char *buf, uint16_t value) {
    char strValue[8];
    uint8_t i = 0;

    logger_print(buf);

    if (value > 0) {
		uint16_t tempValue = value;
		uint8_t digits = 0;

		while (tempValue > 0) {
			tempValue /= 10;
			digits++;
		}

		tempValue = value;

		while (tempValue > 0) {
			strValue[digits - 1 - i] = '0' + (tempValue % 10);
			tempValue /= 10;
			i++;
		}
	} else {
	    strValue[i++] = '0'; 
    }

    strValue[i] = '\n';  
    strValue[i + 1] = '\0'; 

    logger_print(strValue);
}*/

void logger_printValue(const char *buf, int16_t value) {
	char strValue[10];  // Wystarczaj¹co miejsca dla 5 cyfr, znaku minus, '\n' i '\0'
	uint8_t i = 0;

	// Wydrukuj nag³ówek
	logger_print(buf);

	// Obs³uga wartoœci ujemnych
	if (value < 0) {
		strValue[i++] = '-';  // Dodaj znak minus
		value = -value;       // Konwertuj na wartoœæ dodatni¹
	}

	// Obs³uga wartoœci 0
	if (value == 0) {
		strValue[i++] = '0';  // Dodaj '0' do strValue
		} else {
		// Konwersja liczby na ci¹g znaków
		uint16_t tempValue = value;
		uint8_t digits = 0;

		// Oblicz liczbê cyfr
		while (tempValue > 0) {
			tempValue /= 10;
			digits++;
		}

		// Zapisz cyfry w odwrotnej kolejnoœci
		tempValue = value;
		for (int j = digits - 1; j >= 0; j--) {
			strValue[i + j] = '0' + (tempValue % 10);
			tempValue /= 10;
		}
		i += digits;  // Zwiêksz i o liczbê cyfr
	}

	// Dodaj znak nowej linii i zakoñczenie ci¹gu
	strValue[i++] = '\n';
	strValue[i] = '\0';  // Znak koñca ci¹gu

	// Drukowanie wartoœci
	logger_print(strValue);
}
