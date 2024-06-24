/*
 * M601.c
 *
 *  Created on: Apr 11, 2024
 *      Author: Administrator
 */
#include "M601.h"
#include "delay.h"
#include <stdint.h>


/*M601 DQ 引脚定义*/
#define M601_DQ_GPIO_PORT      			GPIOA
#define M601_DQ_GPIO_PIN 				GPIO_PIN_8
#define M601_DQ_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()


/*DQ 拉低 拉高函数定义*/
#define DQ_SET()                        HAL_GPIO_WritePin(M601_DQ_GPIO_PORT, M601_DQ_GPIO_PIN,  GPIO_PIN_SET)
#define DQ_RESET()					  HAL_GPIO_WritePin(M601_DQ_GPIO_PORT, M601_DQ_GPIO_PIN,  GPIO_PIN_RESET)



/*DQ 数据输出*/

#define M601_DQ_IN   					  HAL_GPIO_ReadPin(M601_DQ_GPIO_PORT, M601_DQ_GPIO_PIN)





/**
* @brief 初始化 M601 的 IO 口 DQ 同时检测 M601 的存在
* @param 无
* @retval 0, 正常
* 1, 不存在/不正常
*/
void M601_init(void)
{
	 GPIO_InitTypeDef gpio_init_struct;
	 M601_DQ_GPIO_CLK_ENABLE(); /* 开启 DQ 引脚时钟 */
	 gpio_init_struct.Pin = M601_DQ_GPIO_PIN;
	 gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD; /* 开漏输出 */
	 gpio_init_struct.Pull = GPIO_PULLUP; /* 上拉 */
	 gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	 HAL_GPIO_Init(M601_DQ_GPIO_PORT, &gpio_init_struct);


	/* M601_DQ 引脚模式设置,开漏输出,上拉, , 开漏输出的时候(=1), 也可以读取外部信号的高低电平 */
	 M601_reset();
	 return M601_check();
}

/**
* @brief 复位 M601
* @param data: 要写入的数据
* @retval 无
*/
void M601_reset(void)
{

 DQ_RESET(); /* 拉低 DQ,复位 */
 Delay_us(480); /* 拉低 480us */
 DQ_SET(); /* 释放总线 */
 Delay_us(15); /* 延迟 15US */


}

/**
* @brief 等待 M601 的回应
* @param 无
* @retval 0, M601 正常
* 1, DS18B20 异常/不存在
*/
uint8_t M601_check(void)
{
	uint8_t retry = 0;
	uint8_t rval = 0;
	 while (M601_DQ_IN && retry < 200) /* 等待 DQ 变低, 等待 200us */
	 {
	 retry++;
	 Delay_us(1);
	 }
	 if (retry >= 200)
	 {
	 rval = 1;
	 }
	 else
	 {
	 retry = 0;
	 while (!M601_DQ_IN && retry < 240) /* 等待 DQ 变高, 等待 240us */
	 {
	 retry++;
	 Delay_us(1);
	 }
	 if (retry >= 240) rval = 1;
	 }
	 return rval;

}


void M601_write_bit(unsigned char bit)
{
	if (bit)
	{
		// Write '1' to DQ
		DQ_RESET(); /* Write 1 */
		Delay_us(3);
		DQ_SET();
		Delay_us(60);
	}
	else
	{
		// Write '0' to DQ
		DQ_RESET(); /* Write 0 */
		Delay_us(53);
		DQ_SET();
		Delay_us(10);
	}
}


/**
* @brief 写一个字节到 M601
* @param data: 要写入的字节
* @retval 无
*/
void M601_write_byte(uint8_t data)
{
 uint8_t j;
 for (j = 1; j <= 8; j++)
 {
	 if (data & 0x01)
	 {
		 DQ_RESET(); /* Write 1 */
		 Delay_us(2);
		 DQ_SET();
		 Delay_us(60);
	 }
	 	 else
	 	 {
	 		 DQ_RESET(); /* Write 0 */
	 		 Delay_us(60);
	 		 DQ_SET();
	 		 Delay_us(2);
	 	 }
	 	data >>= 1; /* 右移,获取高一位数据 */
  }
 }


/**
* @brief 从 M601 读取一个位
* @param 无
* @retval 读取到的位值: 0 / 1
*/
uint8_t M601_read_bit(void)
{
 uint8_t data = 0;
 DQ_RESET();
 Delay_us(2);
 DQ_SET();
 Delay_us(12);
 	 if (M601_DQ_IN)
 	 {
 		 data = 1;
 	 }
 Delay_us(50);
 return data;
}
/**
* @brief 从 M601 读取一个字节
* @param 无
* @retval 读到的数据
*/
uint8_t M601_read_byte(void)
{
 uint8_t i, b, data = 0;
 for (i = 0; i < 8; i++)
 {
 b = M601_read_bit(); /* M601 先输出低位数据 ,高位数据后输出 */

 data |= b << i; /* 填充 data 的每一位 */
 }
 return data;
}

/**
* @brief 开始温度转换
* @param 无
* @retval 无
*/
void M601_start(void)
{
	M601_reset();
	M601_check();
	M601_write_byte(0xcc); /* skip rom */
	M601_write_byte(0x44); /* convert */
}



/**
  * @brief  测温读温
  * @param  无
  * @retval 状态
*/
float GetTemp(void)
{
	float ftemp;

	 uint8_t TL, TH;
	 short temp;

	 M601_start(); /* M601 start convert */
	 M601_reset();
	 M601_check();

	 M601_write_byte(0xcc); /* skip rom */
	 M601_write_byte(0xbe); /*convert */

	 TL = M601_read_byte(); /* LSB */
	 TH = M601_read_byte(); /* MSB */

	 temp = TH; /* 获得高八位 */
	 temp <<= 8;
	 temp += TL; /* 获得低八位 */


	 ftemp = (float)temp/ 256.0 + 40.0;


	 return ftemp;

}

