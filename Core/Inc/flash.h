/*
 * flash.h
 *
 *  Created on: May 15, 2024
 *      Author: wangqiang
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_
#include <stdint.h>

#define FLASH_USER_START_ADDR   0x0801FC00   /* Flash start address for user data */
#define FLASH_USER_END_ADDR     (FLASH_USER_START_ADDR + FLASH_PAGE_SIZE - 1)

#define TEMP_DEFAULT            25
#define TEMP_ADDRESS            FLASH_USER_START_ADDR



void Flash_Write_Temperature(uint32_t temperature_1);
uint32_t Flash_Read_Temperature(void);
#endif /* INC_FLASH_H_ */
