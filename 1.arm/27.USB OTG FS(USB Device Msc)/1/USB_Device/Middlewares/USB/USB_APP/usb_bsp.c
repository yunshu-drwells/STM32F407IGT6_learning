/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file is responsible to offer board support package and is
  *          configurable by user.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usb_bsp.h"
#include "usbd_conf.h"

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "delay.h"
#include "stm32f4xx_hal_rcc.h"

#include "stm32f4xx_hal_pcd.h"  //声明HAL_PCD_MspInit，但在usb_otg.c中实现
#include "usb_otg.h"  //hpcd_USB_OTG_FS

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//USB主机电源控制口
#define USB_HOST_PWRCTRL BIT_ADDR(GPIOA_ODR_Addr,15)  //PA15

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USB_BSP
* @brief This file is responsible to offer board support package
* @{
*/ 

/** @defgroup USB_BSP_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USB_BSP_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 





/** @defgroup USB_BSP_Private_Macros
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USBH_BSP_Private_Variables
* @{
*/ 

/**
* @}
*/ 

/** @defgroup USBH_BSP_Private_FunctionPrototypes
* @{
*/ 
/**
* @}
*/ 

/** @defgroup USB_BSP_Private_Functions
* @{
*/ 


/**
* @brief  USB_OTG_BSP_Init
*         Initilizes BSP configurations
* @param  None
* @retval None
*/

#define RCC_AHB2Periph_OTG_FS            ((uint32_t)0x00000080)
/*
#define RCC_AHB1Periph_GPIOA             ((uint32_t)0x00000001)

#define GPIO_PinSource11           ((uint8_t)0x0B)
#define GPIO_PinSource12           ((uint8_t)0x0C)
#define GPIO_AF_OTG_FS         ((uint8_t)0xA)  // OTG_FS Alternate Function mapping 
#define GPIO_AF_OTG_HS         ((uint8_t)0xA)  // OTG_HS Alternate Function mapping 
*/
void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
	HAL_PCD_MspInit(&hpcd_USB_OTG_FS);
	/*
	GPIO_InitTypeDef  GPIO_InitStructure;
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	//RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);//使能USB OTG时钟
	
	GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_12;//PA11/12复用功能输出

	
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;//复用功能
	GPIO_InitStructure.Pull = GPIO_PULLUP;//推挽输出
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;//100MHz
	GPIO_InitStructure.Pull = GPIO_NOPULL; 
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	*/
	
	/*
	GPIO_InitStructure.Pin = GPIO_PIN_15;//PA15推挽输出		
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;//输出功能
	GPIO_InitStructure.Pull = GPIO_NOPULL; 
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	USB_HOST_PWRCTRL;		//开启USB HOST电源供电
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG_FS);//PA11,AF10(USB)
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG_FS);//PA12,AF10(USB)
	*/
}
/**
* @brief  USB_OTG_BSP_EnableInterrupt
*         Enabele USB Global interrupt
* @param  None
* @retval None
*/
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
	/*
	NVIC_InitTypeDef   NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能通道
	NVIC_Init(&NVIC_InitStructure);//配置 
	*/	
	
	HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 3);
  HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}
/**
* @brief  USB_OTG_BSP_uDelay
*         This function provides delay time in micro sec
* @param  usec : Value of delay required in micro sec
* @retval None
*/
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
  delay_us(usec);
}


/**
* @brief  USB_OTG_BSP_mDelay
*          This function provides delay time in milli sec
* @param  msec : Value of delay required in milli sec
* @retval None
*/
void USB_OTG_BSP_mDelay (const uint32_t msec)
{
  delay_ms(msec);  
}
/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
