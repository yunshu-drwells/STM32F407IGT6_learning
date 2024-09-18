/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY0_Pin GPIO_PIN_2
#define KEY0_GPIO_Port GPIOE
#define KEY0_EXTI_IRQn EXTI2_IRQn
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOE
#define KEY1_EXTI_IRQn EXTI3_IRQn
#define KEY2_Pin GPIO_PIN_4
#define KEY2_GPIO_Port GPIOE
#define KEY2_EXTI_IRQn EXTI4_IRQn
#define ETH_RESET_Pin GPIO_PIN_8
#define ETH_RESET_GPIO_Port GPIOI
#define LCD_RS_Pin GPIO_PIN_0
#define LCD_RS_GPIO_Port GPIOG
#define GT9XXX_RST_Pin GPIO_PIN_1
#define GT9XXX_RST_GPIO_Port GPIOG
#define IIC_SCL_Pin GPIO_PIN_6
#define IIC_SCL_GPIO_Port GPIOH
#define GT9XXX_INT_Pin GPIO_PIN_7
#define GT9XXX_INT_GPIO_Port GPIOH
#define IIC_SDA_Pin GPIO_PIN_8
#define IIC_SDA_GPIO_Port GPIOH
#define LCD_BL_Pin GPIO_PIN_9
#define LCD_BL_GPIO_Port GPIOH
#define Spi2_cs_Pin GPIO_PIN_0
#define Spi2_cs_GPIO_Port GPIOI
#define Spi2_sck_Pin GPIO_PIN_1
#define Spi2_sck_GPIO_Port GPIOI
#define Spi2_miso_Pin GPIO_PIN_2
#define Spi2_miso_GPIO_Port GPIOI
#define Spi2_mosi_Pin GPIO_PIN_3
#define Spi2_mosi_GPIO_Port GPIOI
#define LCD_RD_Pin GPIO_PIN_4
#define LCD_RD_GPIO_Port GPIOD
#define CD_WR_Pin GPIO_PIN_5
#define CD_WR_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOG
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOE
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
