#include "main.h"

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
			result = 0;
			if(HAL_GPIO_ReadPin(BUT_PORT,BUT_PIN)==0) 	{ButState = PRESSED;}		// если было касание, то переходим в нажатие
			else										{counter = HAL_GetTick();}
		}break;
		case PRESSED:
		{
			result = 0;
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
			}
		}break;
		case RELIASED:
		{
			result = 0;
			if(HAL_GPIO_ReadPin(BUT_PORT,BUT_PIN)==1)	// теперь ждем пока оптустит
			{
				ButState = NONE;
				result = 1;
			}
		}break;
	}
	return result;
}
void Blinky(uint32_t duration)
{
	enum State
	{
		NONE,	// нихуя 
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
			if (HAL_GetTick()> counter+duration)
			{
				counter = HAL_GetTick();
				LedStatus = OFF;
			}
		}break;
		case OFF:
		{
			LED_OFF();
			if (HAL_GetTick()> counter+duration)
			{
				counter = HAL_GetTick();
				LedStatus = NONE;
			}
		}break;
	}
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
	const uint32_t BlinkOn = 100;
	const uint32_t BlinkOff = 600;
	
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
