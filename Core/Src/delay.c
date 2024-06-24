/*
 * delay.c
 *
 *  Created on: Apr 11, 2024
 *      Author: Administrator
 */

#include "delay.h"
#include "tim.h"

    // 生成微秒级延时函数
    void Delay_us(uint32_t us)
    {
      __HAL_TIM_SET_COUNTER(&htim6, 0);  // 将定时器计数器清零
      while (__HAL_TIM_GET_COUNTER(&htim6) < us);  // 等待计数器到达指定值
    }
