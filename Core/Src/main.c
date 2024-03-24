/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
uint8_t IsButPressed(uint32_t pressDelay);
void Blink (uint32_t *qtty);


enum State
	{
		NONE,		// нихуя
		RECORD,		//  кнопку нажали, идет стадия записи кол-ва нажатий
		PLAY		//  проигрывание кол-ва нажатий
	};

	enum State ProgStatus = NONE;	// переменная для машины состояний
	uint32_t PressCounter = 0;
	uint32_t RecordDelay = 2000;
	uint32_t BlinkOn = 100;
	uint32_t BlinkOff = 600;

int main(void)
{

	#define LED_PIN		GPIO_PIN_1
	#define LED_PORT	GPIOA

	#define BUT_PIN		GPIO_PIN_0
	#define BUT_PORT	GPIOA
	#define LED_ON()		HAL_GPIO_WritePin(LED_PORT,LED_PIN,LED_ON_STATE)
	#define LED_OFF()		HAL_GPIO_WritePin(LED_PORT,LED_PIN,LED_OFF_STATE)
	#define LED_ON_STATE	0
	#define LED_OFF_STATE 1

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	HAL_GPIO_WritePin(LED_PORT, LED_PIN, LED_OFF_STATE);



  while (1)		// в бесконечном цикле и не блокирующем режиме
  {
	  switch(ProgStatus)	 
	  {
		  static uint32_t counter = 0;
		  case NONE:		// ждем хоть одного нажатия
		  {
			  if(IsButPressed(10))
			  {
				  ProgStatus = RECORD;
				  PressCounter++;
				  break;
				  counter = HAL_GetTick();	// запоминаем момент перехода
			  }
		  } break;
		  case RECORD:		// считаем нажатия и ждем когда не будет нажатия N сек
		  {
			  
			  if(IsButPressed(200))
			  {
				  counter = HAL_GetTick();
				  PressCounter++;
			  }
			  if (HAL_GetTick()> counter+RecordDelay)	// если с последнего нажатия прошло N секунд, то идем в проигрывание
			  {
				  ProgStatus=PLAY;
				  counter = HAL_GetTick();	// запоминаем момент перехода
				  break;
			  }
			  
		  }break;
		  case PLAY:
		  {
			  Blink(&PressCounter);
			  if(PressCounter==0)
			  {
				  counter = HAL_GetTick();	// запоминаем момент перехода
				  ProgStatus = NONE;
			  }
			  
		  }break;

	  }
  }
  /* USER CODE END 3 */
}

uint8_t IsButPressed(uint32_t pressDelay)
{
	enum State
	{
		NONE,
		PRESSED,
		RELIASED
	};
	static enum State ButState= NONE;
	
	static uint8_t result = 0;
	static uint32_t counter = 0;
	switch (ButState)
	{
		case NONE:
		{
			if(HAL_GPIO_ReadPin(BUT_PORT,BUT_PIN)==0) 	{ButState = PRESSED;}		// если было касание, то переходим в нажатие
			else										{counter = HAL_GetTick(); result = 0;}
		}break;
		case PRESSED:
		{
			if(HAL_GPIO_ReadPin(BUT_PORT,BUT_PIN)==0)
			{
				if(HAL_GetTick()>counter+pressDelay)	// если кнопка нажата более N милисекунд, то она нажата
				{
					counter = HAL_GetTick();
					ButState = RELIASED;
				}
			}
			else										//если дребезг, то падаем в начало
			{
				counter = HAL_GetTick();
				ButState = NONE;
				result = 0;
			}
		}break;
		case RELIASED:
		{
			if(HAL_GPIO_ReadPin(BUT_PORT,BUT_PIN)==1)	// теперь ждем пока оптустит
			{
				ButState = NONE;
				result = 1;
			}
		}break;
	}
	return result;
}
void Blink (uint32_t *qtty)
{
		enum State
	{
		NONE,		// нихуя 
		ON,		//  горение
		OFF		//  не горение
	};
	static enum State LedStatus = NONE;	// переменная для машины состояний
	static uint32_t counter = 0;
	
	switch(LedStatus)	 
	{
		case NONE:
		{
			counter = HAL_GetTick();
			LedStatus = ON;
		}break;
		case ON:
		{
			LED_ON();
			if (HAL_GetTick()> counter+BlinkOn)
			{
				counter = HAL_GetTick();
				LedStatus = OFF;
			}
		}break;
		case OFF:
		{
			LED_OFF();
			if (HAL_GetTick()> counter+BlinkOff)
			{
				uint32_t var;
				var = *qtty;
				var= var-1;
				*qtty = var;	// декрементируем переменную по указателю.

				counter = HAL_GetTick();
				LedStatus = NONE;
			}
		}break;
	}
	
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO, RCC_MCO1SOURCE_SYSCLK, RCC_MCODIV_1);
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = BUT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUT_PORT, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
