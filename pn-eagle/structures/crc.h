#ifndef CRC_H_
#define CRC_H_

#define CRC_POLY	0x07
#define CRC_INIT	0x00

#include <stdint.h>

uint8_t crc_calc(uint8_t* data, uint8_t dataLen);

#endif /* CRC_H_ */