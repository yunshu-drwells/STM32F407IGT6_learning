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
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
extern osSemaphoreId CpuPrintfBinarySemHandle;
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED0_Pin|LED1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEY0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEY0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PEPin PEPin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

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
				printf("KEY0 is Down!\n");
				KeyStatus = KEY_DOWN;
				xSemaphoreGiveFromISR(CpuPrintfBinarySemHandle, NULL);  //释放二值信号量
				//xSemaphoreTakeFromISR(CpuPrintfBinarySemHandle, NULL);  //获取二值信号量
			}
		}else{
			HAL_Delay(20);  //去抖动
			if(GPIO_PIN_SET == HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)){
				printf("KEY0 is Up!\n");
				KeyStatus = KEY_UP;
				
			}
		}
		
	}
	
}
/* USER CODE END 2 */
