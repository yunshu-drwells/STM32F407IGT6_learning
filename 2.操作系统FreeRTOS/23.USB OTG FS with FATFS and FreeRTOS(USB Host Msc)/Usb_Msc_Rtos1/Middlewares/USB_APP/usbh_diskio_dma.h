/**
  ******************************************************************************
  * @file    usbh_diskio_dma.h
  * @author  MCD Application Team
  * @brief   Header for usbh_diskio_dma.c module.
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
#ifndef __USBH_DISKIO_H
#define __USBH_DISKIO_H

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usbh_msc.h"
#include "ff.h"  //BYTE
#include "diskio.h"  //DRESULT
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
//extern const Diskio_drvTypeDef  USBH_Driver;

typedef BYTE	DSTATUS;
#define USB_DEFAULT_BLOCK_SIZE      512

#define _USE_WRITE 1  //开启USBH_write函数
#define _USE_IOCTL 1  //开启USBH_ioctl函数

DSTATUS USBH_initialize(void);
DSTATUS USBH_status(void);
DRESULT USBH_read(BYTE *buff, DWORD sector, UINT count);
DRESULT USBH_write(const BYTE *buff,DWORD sector,UINT count);
DRESULT USBH_ioctl(BYTE cmd,void *buff);

#endif /* __USBH_DISKIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

