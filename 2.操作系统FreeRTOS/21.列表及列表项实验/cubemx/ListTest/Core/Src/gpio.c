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
#include "event_groups.h"
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
extern EventGroupHandle_t KeyEventGroupHandle_t; //�¼���־��
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

  /*Configure GPIO pins : PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = KEY0_Pin|KEY1_Pin|KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
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
//�������ص�����
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//�ж��ǲ���KEY0���ж�
	if(GPIO_Pin == KEY0_Pin){
		//KEY0�Ƿ���
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)){
			HAL_Delay(20);  //ȥ����
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)){
				//printf("KEY0 is down!\n");
				//�����¼������
				xEventGroupSetBitsFromISR(KeyEventGroupHandle_t, KEY0_EVENT_BIT,NULL);
			}
		}
	}
	//�ж��ǲ���KEY1���ж�
	if(GPIO_Pin == KEY1_Pin){
		//KEY0�Ƿ���
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)){
			HAL_Delay(20);  //ȥ����
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)){
				//printf("KEY1 is down!\n");
				//�����¼������
				xEventGroupSetBitsFromISR(KeyEventGroupHandle_t, KEY1_EVENT_BIT,NULL);
			}
		}
	}
	//�ж��ǲ���KEY1���ж�
	if(GPIO_Pin == KEY2_Pin){
		//KEY0�Ƿ���
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)){
			HAL_Delay(20);  //ȥ����
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)){
				//printf("KEY2 is down!\n");
				//�����¼������
				xEventGroupSetBitsFromISR(KeyEventGroupHandle_t, KEY2_EVENT_BIT,NULL);
			}
		}
	}
}
/* USER CODE END 2 */
