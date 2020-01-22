/*
 * eeprom.c
 *
 *  Created on: Feb 23, 2018
 *      Author: Swapna
 */

#include "eeprom.h"
#include "hw.h"

typedef enum {
	EEPROM_COMPLETE = 0,
	EEPROM_TIMEOUT  = 1
}EEPROM_Status;

static void EEPROM_Lock(void);
static uint32_t EEPROM_Read(uint32_t address);
static void EEPROM_Unlock(void);
static EEPROM_Status EEPROM_WaitForLastOperation(uint32_t timeout);
static EEPROM_Status EEPROM_Write(uint32_t Address, uint32_t Data);

/*!
 *  Read specified quantity of data from EEPROM to buffer
 *  input:
 *  buffer - pointer to buffer
 *  length - length of buffer in bytes
 *  note: buffer must be 32-bit aligned
 */
void EEPROM_ReadBuffer(uint32_t* buffer, uint32_t length)
{
	uint32_t i;

	for (i = 0; i < length; i += 4) *buffer++ = EEPROM_Read(DATA_EEPROM_START_ADDR + i);
}

/*!
 *  Save specified buffer to EEPROM
 *  input:
 *  buffer - pointer to buffer
 *  length - length of buffer in bytes
 *  note: buffer must be 32-bit aligned
 */
void EEPROM_WriteBuffer(uint32_t* buffer, uint32_t length)
{
	uint32_t i;
	volatile uint32_t data;

	EEPROM_Unlock();

	for (i = 0; i < length; i += 4) {
		data = *buffer++;
		if (data != EEPROM_Read(DATA_EEPROM_START_ADDR + i)) EEPROM_Write(DATA_EEPROM_START_ADDR + i, data);
	}
	EEPROM_Lock();
}

// Lock the data memory and FLASH_PECR register
static void EEPROM_Lock(void)
{
	FLASH->PECR |= FLASH_PECR_PELOCK;
}

/*!
 *  Read 32-bit value from EEPROM data memory
 *  input:
 *  Address - address in data memory
 *  return: value from EEPROM data memory
 *  note: Address must be between DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR
 */
static uint32_t EEPROM_Read(uint32_t address)
{
	return (*(volatile uint32_t*)address);
}

// Unlocks the data memory and FLASH_PECR register
static void EEPROM_Unlock(void)
{
	if (FLASH->PECR & FLASH_PECR_PELOCK)
	{
		// Unlock only if it locked
		FLASH->PEKEYR = FLASH_PEKEY1;
		FLASH->PEKEYR = FLASH_PEKEY2;
	}
}

/*!
 *  Wait for a EEPROM operation complete or a timeout occur
 *  input:
 *  Timeout - timeout interval
 *  return: EEPROM status: EEPROM_COMPLETE or EEPROM_TIMEOUT
 */
static EEPROM_Status EEPROM_WaitForLastOperation(uint32_t timeout)
{
	volatile EEPROM_Status status = EEPROM_COMPLETE;

	// Wait for a EEPROM operation to complete or a timeout occur
	while ((FLASH->SR & FLASH_SR_BSY) && (timeout)) timeout--;
	if (!timeout) status = EEPROM_TIMEOUT;

	return status;
}

 /*!
  *  Write long at specified address in data memory (32-bit)
  *  input:
  *  Address - address of long to be written
  *  Data - data to be written
  *  return: EEPROM status: EEPROM_COMPLETE or EEPROM_TIMEOUT
  */
static EEPROM_Status EEPROM_Write(uint32_t address, uint32_t data)
{
	EEPROM_Status status = EEPROM_COMPLETE;

	// Wait for a EEPROM operation to complete or a timeout occur
	status = EEPROM_WaitForLastOperation(EEPROM_PRG_TIMEOUT);

	if (status == EEPROM_COMPLETE)
	{
	    // Clear the FTDW bit (data will be erased before write if it non zero)
	    FLASH->PECR &= (uint32_t)(~(uint32_t)ERASED_BIT);
        *(volatile uint32_t *)address = data; // Program the new data

	    // Wait for a EEPROM operation to complete or a timeout occur
	    status = EEPROM_WaitForLastOperation(EEPROM_PRG_TIMEOUT);
	}

	return status;
}
