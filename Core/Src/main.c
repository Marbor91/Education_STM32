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
#include "perif.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
extern uint8_t IsButPressed(uint32_t pressDelay);
extern void Blink (uint32_t *qtty);
extern void Blinky(uint32_t duration);
void Marbor_task (void);
void Kris_task (void);


	uint32_t RecordDelay = 2000;


int main(void)
{

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	HAL_GPIO_WritePin(LED_PORT, LED_PIN, LED_OFF_STATE);



	while (1)		// в бесконечном цикле и не блокирующем режиме
	{
	  Marbor_task();
	  //Kris_task();
	}
}

void Kris_task (void)
{
	enum ButState
	{
		NONE,
		LONG_PRESS,
		SHORT_PRESS,

	};
	enum ProgState
	{
		OFF,
		LONG_BLINK,
		SHORT_BLINK
	};

	static enum ProgState ProgStatus = OFF;	// переменная для машины состояний
	static enum ButState ButStatus = NONE;		// это для кнопки
	static uint32_t counter = 0;
	const uint32_t long_duration = 1500;
	const uint32_t long_blink_duration = 500;
	const uint32_t short_blink_duration = 100;
	

	if(IsButPressed(100))	// bounce avoid
		{
			if(counter - HAL_GetTick() > long_duration)
			{
				ButStatus = LONG_PRESS;

			}
			else
			{
				ButStatus = SHORT_PRESS;
			}
		}
	switch(ProgStatus)	 
	  {
		case OFF:		// ждем хоть одного нажатия
		{
			if (ButStatus==LONG_PRESS) {ProgStatus = LONG_BLINK;ButStatus = NONE; }
			else 					   {ProgStatus = SHORT_BLINK;}
		} break;
		case LONG_BLINK:
		{
			if (ButStatus==LONG_PRESS) {ProgStatus = OFF;ButStatus = NONE;}
			else 					   {Blinky(long_blink_duration);}
		} break;
		case SHORT_BLINK:
		{
			if (ButStatus==SHORT_PRESS) {ProgStatus = OFF;ButStatus = NONE; }
			else 					   {Blinky(short_blink_duration);}
		} break;

	  }



}


void Marbor_task (void)
{
	enum State
	{
		NONE,		// нихуя
		RECORD,		//  кнопку нажали, идет стадия записи кол-ва нажатий
		PLAY		//  проигрывание кол-ва нажатий
	};

	static enum State ProgStatus = NONE;	// переменная для машины состояний
	uint32_t PressCounter = 0;
	
	  switch(ProgStatus)	 
	  {
		  static uint32_t counter = 0;
		  case NONE:		// ждем хоть одного нажатия
		  {
			  if(IsButPressed(100))
			  {
				  ProgStatus = RECORD;
				  PressCounter++;
				  counter = HAL_GetTick();	// запоминаем момент перехода
			  }
		  } break;
		  case RECORD:		// считаем нажатия и ждем когда не будет нажатия N сек
		  {
			  
			  if(IsButPressed(100))
			  {
				  counter = HAL_GetTick();
				  PressCounter++;
				  DEBUG_PIN_ON(DEBUG_PIN_5);
				  DEBUG_PIN_OFF(DEBUG_PIN_5);
			  }
			  if (HAL_GetTick()> counter+RecordDelay)	// если с последнего нажатия прошло N секунд, то идем в проигрывание
			  {
				  ProgStatus=PLAY;
				  counter = HAL_GetTick();	// запоминаем момент перехода
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

	GPIO_InitStruct.Pin = DEBUG_PIN_2|DEBUG_PIN_3|DEBUG_PIN_4|DEBUG_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DEBUG_PORT, &GPIO_InitStruct);

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
