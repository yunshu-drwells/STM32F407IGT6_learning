/**
  ******************************************************************************
  * @file    USB_Device/MSC_Standalone/Inc/usbd_conf.h
  * @author  MCD Application Team
  * @brief   General low level driver configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF_H
#define __USBD_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "delay.h"
#include "malloc.h"
#include "stm32f4xx_hal_pcd.h"

#define HAL_Delay delay_ms                          /* ����HAL_Delayʹ��delay_msʵ�� */
#define HAL_PCD_MODULE_ENABLED                      /* ʹ��PCDģ��,��������,���򽫲������pcd��ش��� */

#define USBD_SUPPORT_USER_STRING        0 

/* �����豸���� */
#define DEVICE_FS       0
#define DEVICE_HS       1

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Common Config */
#define USBD_MAX_NUM_INTERFACES               1
#define USBD_MAX_NUM_CONFIGURATION            1
#define USBD_MAX_STR_DESC_SIZ                 0x100
#define USBD_SELF_POWERED                     1
#define USBD_DEBUG_LEVEL                      0

/* MSC Class Config */
#define MSC_MEDIA_PACKET                      32*1024//8192

/* ���֧�ֵ��豸�� */
#define STORAGE_LUN_NBR                 1           /* ���֧�ֵ��豸��, ��� STORAGE_LUN_NBR +1 �� */

/* Exported macro ------------------------------------------------------------*/
/* Memory management macros */
//#define USBD_malloc               malloc
//#define USBD_free                 free
#define USBD_malloc(x)          mymalloc(SRAMIN,x)
/** Alias for memory release. */
#define USBD_free(x)           myfree(SRAMIN,x)

#define USBD_memset               memset
#define USBD_memcpy               memcpy
#define USBD_Delay                HAL_Delay

/* DEBUG macros */
#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)   printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)

#define  USBD_ErrLog(...)   printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)   
#endif

#if (USBD_DEBUG_LEVEL > 2)                         
#define  USBD_DbgLog(...)   printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)                         
#endif

/* Exported functions ------------------------------------------------------- */

#endif /* __USBD_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/