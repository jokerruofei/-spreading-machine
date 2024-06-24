/*
 * key.h
 *
 *  Created on: May 14, 2024
 *      Author: Administrator
 */

#ifndef INC_KEY_H_
#define INC_KEY_H_

#define KEY0_GPIO_PORT        GPIOB
#define KEY0_GPIO_PIN         KEY0_Pin

#define KEY1_GPIO_PORT        GPIOB
#define KEY1_GPIO_PIN         KEY1_Pin

#define KEY_lED_GPIO_PORT        GPIOB
#define KEY_LED_GPIO_PIN         KEY_LED_Pin



#define KEY0    HAL_GPIO_ReadPin(KEY0_GPIO_PORT, KEY0_GPIO_PIN)
#define KEY1    HAL_GPIO_ReadPin(KEY1_GPIO_PORT, KEY1_GPIO_PIN)
#define KEY_LED    HAL_GPIO_ReadPin(KEY_LED_GPIO_PORT, KEY_LED_GPIO_PIN)
#endif /* INC_KEY_H_ */
