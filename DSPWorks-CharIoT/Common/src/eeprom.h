/*
 * eeprom.h
 *
 *  Created on: Feb 23, 2018
 *      Author: Swapna
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <stdint.h>

void EEPROM_ReadBuffer(uint32_t* buffer, uint32_t length);
void EEPROM_WriteBuffer(uint32_t* buffer, uint32_t length);

#endif /* _EEPROM_H_ */
