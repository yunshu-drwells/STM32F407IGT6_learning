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
#include "usart.h"

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

/**
  * @}
  */


/** @defgroup USBH_HID_MOUSE_Private_Variables
  * @{
  */
HID_MOUSE_Info_TypeDef    mouse_info;
uint8_t                   mouse_report_data[HID_QUEUE_SIZE];




/**
  * @}
  */


/** @defgroup USBH_HID_MOUSE_Private_Functions
  * @{
  */

/**
 * @brief       USBH 鼠标初始化
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_HID_MouseInit(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *HID_Handle =  (HID_HandleTypeDef *) phost->pActiveClass->pData;  //从USB主机句柄中获取当前活动类的数据，并将其转换为HID句柄
    mouse_info.x = 0;  //将鼠标的x坐标初始化为0
    mouse_info.y = 0;  //将鼠标的y坐标初始化为0
		mouse_info.z = 0;  //将鼠标的y坐标初始化为0
    mouse_info.button = 0;  //将鼠标的按钮状态初始化为0，表示没有任何按钮被按下
    if (HID_Handle->length > sizeof(mouse_report_data))  //检查HID句柄中的长度是否大于鼠标报告数据的大小
    {
        HID_Handle->length = sizeof(mouse_report_data);
    }
		
    HID_Handle->pData = (uint8_t *)mouse_report_data;  //将鼠标报告数据的地址赋值给HID句柄中的数据指针
    USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE);  //使用USB主机句柄中的设备数据和预定义的队列大小初始化HID句柄中的FIFO
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
 * @brief       USBH 鼠标数据解析函数
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_HID_MouseDecode(USBH_HandleTypeDef *phost)
{
    HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;  //从USB主机句柄中获取当前活动类的数据，并将其转换为HID句柄。

    if (HID_Handle->length == 0)return USBH_FAIL;  //如果HID句柄中的长度为0，那么函数立即返回`USBH_FAIL`，表示失败。
		//printf("HID_Handle->length:%d\n", HID_Handle->length);  //8
		//printf("USBH_HID_FifoRead(&HID_Handle->fifo, &mouse_report_data, HID_Handle->length) is %d\n", USBH_HID_FifoRead(&HID_Handle->fifo, &mouse_report_data, HID_Handle->length));  //返回0，说明fifo是空的
		
	  //从HID句柄的FIFO中读取 HID_Handle->length 个字节的数据到 mouse_report_data 中。如果读取的字节数与 HID_Handle->length 相等，那么进入下一步，否则返回 USBH_FAIL。
    if (USBH_HID_FifoRead(&HID_Handle->fifo, &mouse_report_data, HID_Handle->length) == HID_Handle->length) // 读取FIFO
    {
        mouse_info.button = mouse_report_data[0];  //将鼠标报告数据的第一个字节（表示按钮状态）赋值给 mouse_info.button
        mouse_info.x      = mouse_report_data[1];  //将鼠标报告数据的第二个字节（表示x坐标）赋值给 mouse_info.x
        mouse_info.y      = mouse_report_data[2];  //将鼠标报告数据的第三个字节（表示y坐标）赋值给 mouse_info.y
        mouse_info.z      = mouse_report_data[3];  //将鼠标报告数据的第四个字节（可能表示滚轮状态）赋值给 mouse_info.z
        return USBH_OK;
    }
		printf("mouse_info.x:^%d\n", mouse_report_data[1]);
		printf("mouse_info.y:^%d\n", mouse_report_data[2]);
		printf("mouse_info.z:^%d\n", mouse_report_data[3]);

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
