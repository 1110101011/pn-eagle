#include "crc.h"

uint8_t crc_calc(uint8_t* data, uint8_t dataLen) {
	uint8_t crc = CRC_INIT;
	
	for (uint8_t i = 0; i < dataLen; i++) {
		crc ^= data[i];
		
		for (uint8_t j = 0; j < 8; j++) {
			if (crc & 0x80) {
				crc = (crc << 1) ^ CRC_POLY;
			} else {
				crc <<= 1;
			}
		}
	}
	
	return crc;
}