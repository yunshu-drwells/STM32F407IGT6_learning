/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "cmsis_os.h"
teKeyStatus KeyStatus;
#include "event_groups.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
extern osSemaphoreId CpuPrintfBinarySemHandle;
extern osSemaphoreId KeyCountingSemHandle;
extern EventGroupHandle_t KeyEventGroupHandle_t;
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED0_Pin|LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = KEY0_Pin|KEY1_Pin|KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = BEEP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

}

/* USER CODE BEGIN 2 */
//按键检测回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//判断是不是KEY0的中断
	if(GPIO_Pin == KEY0_Pin){
		//KEY0是否按下
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)){
			HAL_Delay(20);  //去抖动
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)){
				//printf("key0 ");
				//设置事件标记组
				xEventGroupSetBitsFromISR(KeyEventGroupHandle_t, KEY0_EVENT_BIT,NULL);
				//printf("is down!\n");
			}
		}
	}
	//判断是不是KEY1的中断
	if(GPIO_Pin == KEY1_Pin){
		//KEY0是否按下
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)){
			HAL_Delay(20);  //去抖动
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)){
				//设置事件标记组
				xEventGroupSetBitsFromISR(KeyEventGroupHandle_t, KEY1_EVENT_BIT,NULL);
			}
		}
	}
	//判断是不是KEY1的中断
	if(GPIO_Pin == KEY2_Pin){
		//KEY0是否按下
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)){
			HAL_Delay(20);  //去抖动
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)){
				//设置事件标记组
				xEventGroupSetBitsFromISR(KeyEventGroupHandle_t, KEY2_EVENT_BIT,NULL);
			}
		}
	}
}
/* USER CODE END 2 */
