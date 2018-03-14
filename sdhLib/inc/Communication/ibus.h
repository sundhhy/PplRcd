/*
 * ibus.h
 *
 *  Created on: 2017-3-27
 *      Author: Administrator
 */

#ifndef IBUS_H_
#define IBUS_H_
#include <stdint.h>
#include <semaphore.h>
#include "arithmetic/crc.h"
#define CRCLEN 2
uint16_t htbs( uint16_t hostshort);
uint16_t bths( uint16_t busshort);
uint16_t CRC16(uint8_t *puchMsg, uint16_t usDataLen);

#endif /* IBUS_H_ */
