/**
  ******************************************************************************
  * @file    usbh_hid_mouse.c
  * @author  MCD Application Team
  * @brief   This file is the application layer for USB Host HID Mouse Handling.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
- "stm32xxxxx_{eval}{discovery}{adafruit}_lcd.c"
- "stm32xxxxx_{eval}{discovery}_sdram.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_mouse.h"
#include "usbh_hid_parser.h"


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */

/** @defgroup USBH_HID_MOUSE
  * @brief    This file includes HID Layer Handlers for USB Host HID class.
  * @{
  */

/** @defgroup USBH_HID_MOUSE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_HID_MOUSE_Private_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_HID_MOUSE_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_HID_MOUSE_Private_FunctionPrototypes
  * @{
  */
static USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost);

/**
  * @}
  */


/** @defgroup USBH_HID_MOUSE_Private_Variables
  * @{
  */
HID_MOUSE_Info_TypeDef    mouse_info;
uint8_t                   mouse_report_data[HID_QUEUE_SIZE];
uint32_t                  mouse_rx_report_buf[2];

/* Structures defining how to access items in a HID mouse report */



/**
  * @}
  */


/** @defgroup USBH_HID_MOUSE_Private_Functions
  * @{
  */

/**
  * @brief  USBH_HID_MouseInit
  *         The function init the HID mouse.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_HID_MouseInit(USBH_HandleTypeDef *phost)
{
  uint32_t i;
  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;


	mouse_info.x = 0;  //将鼠标的x坐标初始化为0
  mouse_info.y = 0;  //将鼠标的y坐标初始化为0
  //mouse_info.buttons[0] = 0U;
  //mouse_info.buttons[1] = 0U;
  //mouse_info.buttons[2] = 0U;
	mouse_info.z = 0;  //将鼠标的y坐标初始化为0
  mouse_info.button = 0;  //将鼠标的按钮状态初始化为0，表示没有任何按钮被按下

  for (i = 0U; i < (sizeof(mouse_report_data) / sizeof(uint32_t)); i++)
  {
    mouse_report_data[i] = 0U;
    mouse_rx_report_buf[i] = 0U;
  }

  if (HID_Handle->length > sizeof(mouse_report_data))
  {
    HID_Handle->length = sizeof(mouse_report_data);
  }
  HID_Handle->pData = (uint8_t *)(void *)mouse_rx_report_buf;
  //USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE * sizeof(mouse_report_data));
	USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE);

  return USBH_OK;
}

/**
  * @brief  USBH_HID_GetMouseInfo
  *         The function return mouse information.
  * @param  phost: Host handle
  * @retval mouse information
  */
HID_MOUSE_Info_TypeDef *USBH_HID_GetMouseInfo(USBH_HandleTypeDef *phost)
{
  if (USBH_HID_MouseDecode(phost) == USBH_OK)
  {
    return &mouse_info;
  }
  else
  {
    return NULL;
  }
}

/**
  * @brief  USBH_HID_MouseDecode
  *         The function decode mouse data.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost)
{
  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

  if (HID_Handle->length == 0U)
  {
    return USBH_FAIL;
  }
  /*Fill report */
  if (USBH_HID_FifoRead(&HID_Handle->fifo, &mouse_report_data, HID_Handle->length) ==  HID_Handle->length)
  {
    /*Decode report */
		mouse_info.x      = mouse_report_data[1];  //将鼠标报告数据的第二个字节（表示x坐标）赋值给 mouse_info.x
		mouse_info.y      = mouse_report_data[2];  //将鼠标报告数据的第三个字节（表示y坐标）赋值给 mouse_info.y

    //mouse_info.buttons[0] = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_b1, 0U);
    //mouse_info.buttons[1] = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_b2, 0U);
    //mouse_info.buttons[2] = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &prop_b3, 0U);
		
		mouse_info.button = mouse_report_data[0];  //将鼠标报告数据的第一个字节（表示按钮状态）赋值给 mouse_info.button
		mouse_info.z      = mouse_report_data[3];  //将鼠标报告数据的第四个字节（可能表示滚轮状态）赋值给 mouse_info.z

    return USBH_OK;
  }
  /*
	printf("mouse_info.x:^%d\n", mouse_report_data[1]);
	printf("mouse_info.y:^%d\n", mouse_report_data[2]);
	printf("mouse_info.z:^%d\n", mouse_report_data[3]);
	*/
  return   USBH_FAIL;
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

/**
  * @}
  */


/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
