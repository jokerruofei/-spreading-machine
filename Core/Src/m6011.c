/*
 * m6011.c
 *
 *  Created on: May 31, 2024
 *      Author: Administrator
 */
#include "M6011.h"
#include "delay.h"
#include <stdint.h>


/*M601 DQ 引脚定义*/
#define M6011_DQ_GPIO_PORT      			GPIOA
#define M6011_DQ_GPIO_PIN 					GPIO_PIN_9
#define M6011_DQ_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()


/*DQ 拉低 拉高函数定义*/
#define DQ1_SET()                        HAL_GPIO_WritePin(M6011_DQ_GPIO_PORT, M6011_DQ_GPIO_PIN,  GPIO_PIN_SET)
#define DQ1_RESET()					  HAL_GPIO_WritePin(M6011_DQ_GPIO_PORT, M6011_DQ_GPIO_PIN,  GPIO_PIN_RESET)



/*DQ 数据输出*/

#define M6011_DQ_IN   					  HAL_GPIO_ReadPin(M6011_DQ_GPIO_PORT, M6011_DQ_GPIO_PIN)





/**
* @brief 初始化 M601 的 IO 口 DQ 同时检测 M601 的存在
* @param 无
* @retval 0, 正常
* 1, 不存在/不正常
*/
void M6011_init(void)
{
	 GPIO_InitTypeDef gpio_init_struct1;
	 M6011_DQ_GPIO_CLK_ENABLE(); /* 开启 DQ 引脚时钟 */
	 gpio_init_struct1.Pin = M6011_DQ_GPIO_PIN;
	 gpio_init_struct1.Mode = GPIO_MODE_OUTPUT_OD; /* 开漏输出 */
	 gpio_init_struct1.Pull = GPIO_PULLUP; /* 上拉 */
	 gpio_init_struct1.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速 */
	 HAL_GPIO_Init(M6011_DQ_GPIO_PORT, &gpio_init_struct1);


	/* M601_DQ 引脚模式设置,开漏输出,上拉, , 开漏输出的时候(=1), 也可以读取外部信号的高低电平 */
	 M6011_reset();
	 return M6011_check();
}

/**
* @brief 复位 M601
* @param data: 要写入的数据
* @retval 无
*/
void M6011_reset(void)
{

 DQ1_RESET(); /* 拉低 DQ,复位 */
 Delay_us(480); /* 拉低 480us */
 DQ1_SET(); /* 释放总线 */
 Delay_us(15); /* 延迟 15US */


}

/**
* @brief 等待 M601 的回应
* @param 无
* @retval 0, M601 正常
* 1, DS18B20 异常/不存在
*/
uint8_t M6011_check(void)
{
	uint8_t retry1 = 0;
	uint8_t rval1 = 0;
	 while (M6011_DQ_IN && retry1 < 200) /* 等待 DQ 变低, 等待 200us */
	 {
	 retry1++;
	 Delay_us(1);
	 }
	 if (retry1 >= 200)
	 {
	 rval1 = 1;
	 }
	 else
	 {
	 retry1 = 0;
	 while (!M6011_DQ_IN && retry1 < 240) /* 等待 DQ 变高, 等待 240us */
	 {
	 retry1++;
	 Delay_us(1);
	 }
	 if (retry1 >= 240) rval1 = 1;
	 }
	 return rval1;

}


void M6011_write_bit(unsigned char bit)
{
	if (bit)
	{
		// Write '1' to DQ
		DQ1_RESET(); /* Write 1 */
		Delay_us(3);
		DQ1_SET();
		Delay_us(60);
	}
	else
	{
		// Write '0' to DQ
		DQ1_RESET(); /* Write 0 */
		Delay_us(53);
		DQ1_SET();
		Delay_us(10);
	}
}


/**
* @brief 写一个字节到 M601
* @param data: 要写入的字节
* @retval 无
*/
void M6011_write_byte(uint8_t data)
{
 uint8_t j;
 for (j = 1; j <= 8; j++)
 {
	 if (data & 0x01)
	 {
		 DQ1_RESET(); /* Write 1 */
		 Delay_us(2);
		 DQ1_SET();
		 Delay_us(60);
	 }
	 	 else
	 	 {
	 		 DQ1_RESET(); /* Write 0 */
	 		 Delay_us(60);
	 		 DQ1_SET();
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
uint8_t M6011_read_bit(void)
{
 uint8_t data = 0;
 DQ1_RESET();
 Delay_us(2);
 DQ1_SET();
 Delay_us(12);
 	 if (M6011_DQ_IN)
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
uint8_t M6011_read_byte(void)
{
 uint8_t i, b, data = 0;
 for (i = 0; i < 8; i++)
 {
 b = M6011_read_bit(); /* M601 先输出低位数据 ,高位数据后输出 */

 data |= b << i; /* 填充 data 的每一位 */
 }
 return data;
}

/**
* @brief 开始温度转换
* @param 无
* @retval 无
*/
void M6011_start(void)
{
	M6011_reset();
	M6011_check();
	M6011_write_byte(0xcc); /* skip rom */
	M6011_write_byte(0x44); /* convert */
}



/**
  * @brief  测温读温
  * @param  无
  * @retval 状态
*/
float GetTempone(void)
{
	float ftemp1;

	 uint8_t TL1, TH1;
	 short temp1;

	 M6011_start(); /* M601 start convert */
	 M6011_reset();
	 M6011_check();

	 M6011_write_byte(0xcc); /* skip rom */
	 M6011_write_byte(0xbe); /*convert */

	 TL1 = M6011_read_byte(); /* LSB */
	 TH1 = M6011_read_byte(); /* MSB */

	 temp1 = TH1; /* 获得高八位 */
	 temp1 <<= 8;
	 temp1 += TL1; /* 获得低八位 */


	 ftemp1 = (float)temp1/ 256.0 + 40.0;


	 return ftemp1;

}

