/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include <string.h>
#include "stm32f0xx_hal.h"
#include "delay.h"
//#include "oled.h"
#include "M601.h"
#include "key.h"
#include "flash.h"
#include "M6011.h"
#include "PID.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BUTTON_HOLD_THRESHOLD 10000

#define PTC_PWM_CHANNEL TIM_CHANNEL_1

PID_TypeDef pidController;
uint32_t target_temperature = TEMP_DEFAULT; // 默认目标温度

uint32_t segment_data[] = {
		0x3F, // 0
		0x06, // 1
		0x5B, // 2
		0x4F, // 3
		0x66, // 4
		0x6D, // 5
		0x7D, // 6
		0x07, // 7
		0x7F, // 8
		0x6F  // 9
		};
uint32_t current_digit = 0;

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_I2C2_Init();
	MX_TIM3_Init();
	MX_TIM6_Init();
	MX_TIM7_Init();
	MX_TIM15_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */
	uint32_t currentTemperature = Flash_Read_Temperature();

	target_temperature = currentTemperature;
	uint8_t buttonState = 0;
	uint32_t buttonPressTime = 0;

	uint8_t buttonState1 = 0;
	uint32_t buttonPressTime1 = 0;
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if (KEY1 == GPIO_PIN_RESET) {

					if (buttonState == 0) {
						buttonState = 1;
						buttonPressTime = HAL_GetTick();
					} else {

						if (HAL_GetTick() - buttonPressTime > BUTTON_HOLD_THRESHOLD) {

							Error_Handler();
						}
					}
				} else {
					if (buttonState == 1) {
						HAL_Delay(200);
						/* 按键被按下，增加目标温度 */
						target_temperature += 1;
						//
						if (target_temperature > 60) {
							target_temperature = 0;
						}
						Flash_Write_Temperature(target_temperature);
						buttonState = 0;
						HAL_Delay(100);
					}
				}

				if (KEY0 == GPIO_PIN_RESET) {
					if (buttonState1 == 0) {
						buttonState1 = 1;
						buttonPressTime1 = HAL_GetTick();
					} else {

						if (HAL_GetTick() - buttonPressTime1 > BUTTON_HOLD_THRESHOLD) {

							Error_Handler();
						}
					}
				} else {
					if (buttonState1 == 1) {
						HAL_Delay(200);

						target_temperature -= 1;

						if (target_temperature < 0) {
							target_temperature = 100;
						}
						Flash_Write_Temperature(target_temperature);
						buttonState1 = 0;
						HAL_Delay(100);
					}
				}

				pidController.setpoint = (float)target_temperature;

				float tempreture = GetTemp();

				float temperture_ch1 = GetTempone();

				float avgTemp = (tempreture + temperture_ch1) / 2.0f;

				// Compute PID output
				float pidOutput = PID_Compute(&pidController, avgTemp);

				// Control PTC heater
				__HAL_TIM_SET_COMPARE(&htim3, PTC_PWM_CHANNEL, pidOutput);

				HAL_Delay(100); // Delay for 100ms

//				char str[10];
//				char str1[10];
//				char str2[10];
//
//				sprintf(str, "%.2f", tempreture);
//				sprintf(str1, "%d", target_temperature);
//				sprintf(str2, "%.2f", temperture_ch1);
//
//				OLED_ShowString(42, 0, str, 16);
//				OLED_ShowString(42, 3, str1, 16);
//				OLED_ShowString(42, 5, str2, 16);

				char buffer[100];
				snprintf(buffer, sizeof(buffer),"Temp1: %.2f, Temp2: %.2f, Avg: %.2f, PID Output: %.2f\r\n",tempreture, temperture_ch1, avgTemp, pidOutput);
				HAL_UART_Transmit(&huart3, (uint8_t*) buffer, strlen(buffer),
						HAL_MAX_DELAY);
				HAL_Delay(300);




	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM7) {

		uint32_t tens = target_temperature / 10;
		uint32_t units = target_temperature % 10;

		if (current_digit == 0) {
			HAL_GPIO_WritePin(GPIOB, LED_DIG1_Pin, GPIO_PIN_SET); // 打开1
			HAL_GPIO_WritePin(GPIOB, LED_DIG2_Pin, GPIO_PIN_RESET); // 关闭2
			HAL_GPIO_WritePin(GPIOA, 0xFF, GPIO_PIN_SET); // 清除
			HAL_GPIO_WritePin(GPIOA, segment_data[tens], GPIO_PIN_RESET); // 显示十位数字
			current_digit = 1;
		} else {
			HAL_GPIO_WritePin(GPIOB, LED_DIG1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, LED_DIG2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, 0xFF, GPIO_PIN_SET); // 清除
			HAL_GPIO_WritePin(GPIOA, segment_data[units], GPIO_PIN_RESET); // 显示个位数字
			current_digit = 0;
		}
	}

}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
		HAL_GPIO_WritePin(GPIOA, BUG_LED_Pin, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOA, BUG_LED_Pin, GPIO_PIN_RESET);
		HAL_Delay(200);
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
