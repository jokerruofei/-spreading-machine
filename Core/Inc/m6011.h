/*
 * m6011.h
 *
 *  Created on: May 31, 2024
 *      Author: Administrator
 */

#ifndef INC_M6011_H_
#define INC_M6011_H_

#include <stdint.h>


/**
* @brief 初始化 M601 的 IO 口 DQ 同时检测 M601 的存在
* @param 无
* @retval 0, 正常
* 1, 不存在/不正常
*/
void M6011_init(void);

/**
* @brief 复位 M601
* @param data: 要写入的数据
* @retval 无
*/
void M6011_reset(void);

/**
* @brief 等待 M601 的回应
* @param 无
* @retval 0, M601 正常
* 1, DS18B20 异常/不存在
*/
uint8_t M6011_check(void);

/**
* @brief 写一个字节到 M601
* @param data: 要写入的字节
* @retval 无
*/
void M6011_write_byte(uint8_t data);

/**
* @brief 从 M601 读取一个位
* @param 无
* @retval 读取到的位值: 0 / 1
*/
uint8_t M6011_read_bit(void);

/**
* @brief 从 M601 读取一个字节
* @param 无
* @retval 读到的数据
*/
uint8_t M6011_read_byte(void);


/**
* @brief 开始温度转换
* @param 无
* @retval 无
*/
void M6011_start(void);

/**
  * @brief  测温读温
  * @param  无
  * @retval 状态
*/

float GetTempone(void);

#endif /* INC_M6011_H_ */
