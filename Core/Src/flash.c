/*
 * flash.c
 *
 *  Created on: May 15, 2024
 *      Author: wangqiang
 */

#include "flash.h"
#include <stdint.h>
#include "stm32f0xx_hal.h"
void Flash_Write_Temperature(uint32_t temperature_1)
{
	uint32_t currentTemperature = Flash_Read_Temperature();
	if (currentTemperature = temperature_1) {
		HAL_FLASH_Unlock();

		// 擦除Flash页
		FLASH_EraseInitTypeDef EraseInitStruct;
		uint32_t PageError = 0;

		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;
		EraseInitStruct.NbPages = 1;

		// 擦除Flash页
		FLASH_PageErase(FLASH_USER_START_ADDR);

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
			// 错误处理
			HAL_FLASH_Lock();
			return;
		}

		// 写入温度值
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, TEMP_ADDRESS,
				temperature_1) != HAL_OK) {
			// 错误处理
		}

		HAL_FLASH_Lock();
	}
}

uint32_t Flash_Read_Temperature(void)
{
    uint32_t temp = *(__IO uint32_t*)TEMP_ADDRESS;
    if (temp == 0xFFFFFFFF) // 初始状态，未写入数据
    {
        temp = TEMP_DEFAULT;
    }
    return temp;
}
