/**
  ******************************************************************************
  * @file    usbh_diskio_dma.c
  * @author  MCD Application Team
  * @brief   USB Host Disk I/O driver (with internal DMA).
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

/* Includes ------------------------------------------------------------------*/
//#include "ff_gen_drv.h"
#include "usbh_diskio_dma.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define USB_DEFAULT_BLOCK_SIZE 512

/* Private variables ---------------------------------------------------------*/
//static DWORD scratch[_MAX_SS / 4];
extern USBH_HandleTypeDef  hUSBHost;




/* Private function prototypes -----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/

/**
 * @brief       初始化 USBH
 * @param       无
 * @retval      无
 */
DSTATUS USBH_initialize(void)
{
    return RES_OK;
}

/**
 * @brief       获取U盘状态
 * @param       无
 * @retval      无
 */
DSTATUS USBH_status(void)
{
    DRESULT res = RES_ERROR;
    MSC_HandleTypeDef *MSC_Handle = hUSBHost.pActiveClass->pData;

    if (USBH_MSC_UnitIsReady(&hUSBHost, MSC_Handle->current_lun))
    {
        printf("U盘状态查询成功\r\n");
        res = RES_OK;
    }
    else
    {
        printf("U盘状态查询失败\r\n");
        res = RES_ERROR;
    }

    return res;
}

/**
 * @brief       U盘读扇区操作
 * @param       buff   : 数据缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      执行结果(详见DRESULT定义)
 */
DRESULT USBH_read(BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;
    MSC_LUNTypeDef info;
    MSC_HandleTypeDef *MSC_Handle = hUSBHost.pActiveClass->pData;
    
    if (USBH_MSC_Read(&hUSBHost, MSC_Handle->current_lun, sector, buff, count) == USBH_OK)
    {
        res = RES_OK;
    }
    else
    {
        printf("U盘读取失败\r\n");
        USBH_MSC_GetLUNInfo(&hUSBHost, MSC_Handle->current_lun, &info);
        switch (info.sense.asc)
        {
            case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
            case SCSI_ASC_MEDIUM_NOT_PRESENT:
            case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
                USBH_ErrLog("USB Disk is not ready!");
                res = RES_NOTRDY;
                break;
            default:
                res = RES_ERROR;
                break;
        }
    }
    return res;
}

/**
 * @brief       U盘写扇区操作
 * @param       buff   : 数据缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要写入的扇区数
 * @retval      执行结果(详见DRESULT定义)
 */
#if _USE_WRITE == 1
DRESULT USBH_write(const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT res = RES_ERROR;
    MSC_LUNTypeDef info;
    MSC_HandleTypeDef *MSC_Handle = hUSBHost.pActiveClass->pData;
    if (USBH_MSC_Write(&hUSBHost, MSC_Handle->current_lun, sector, (BYTE *)buff, count) == USBH_OK)
    {
        res = RES_OK;
    }
    else
    {
        printf("U盘写入失败\r\n");
        USBH_MSC_GetLUNInfo(&hUSBHost, MSC_Handle->current_lun, &info);
        switch (info.sense.asc)
        {
            case SCSI_ASC_WRITE_PROTECTED:
                USBH_ErrLog("USB Disk is Write protected!");
                res = RES_WRPRT;
                break;
            case SCSI_ASC_LOGICAL_UNIT_NOT_READY:
            case SCSI_ASC_MEDIUM_NOT_PRESENT:
            case SCSI_ASC_NOT_READY_TO_READY_CHANGE:
                USBH_ErrLog("USB Disk is not ready!");
                res = RES_NOTRDY;
                break;
            default:
                res = RES_ERROR;
                break;
        }
    }
    return res;
}
#endif /* _USE_WRITE == 1 */

/**
 * @brief       U盘IO控制操作
 * @param       cmd    : 控制命令
 * @param       buff   : 控制数据
 * @retval      执行结果(详见DRESULT定义)
 */
#if _USE_IOCTL == 1
DRESULT USBH_ioctl(BYTE cmd,void *buff)
{
    DRESULT res = RES_ERROR;
    MSC_LUNTypeDef info;
    MSC_HandleTypeDef *MSC_Handle = hUSBHost.pActiveClass->pData;
    switch(cmd)
    {
        case CTRL_SYNC:
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT : /* 获取扇区数量 */
            if (USBH_MSC_GetLUNInfo(&hUSBHost, MSC_Handle->current_lun, &info) == USBH_OK)
            {
                *(DWORD*)buff = info.capacity.block_nbr;
                res = RES_OK;
                printf("扇区数量:%d\r\n", info.capacity.block_nbr);
            }
            else
            {
                res = RES_ERROR;
            }
            break;
        case GET_SECTOR_SIZE : /* 获取扇区大小 */
            if (USBH_MSC_GetLUNInfo(&hUSBHost, MSC_Handle->current_lun, &info) == USBH_OK)
            {
                *(DWORD*)buff = info.capacity.block_size;
                res = RES_OK;
                printf("扇区大小:%d\r\n", info.capacity.block_size);
            }
            else
            {
                res = RES_ERROR;
            }
            break;
        case GET_BLOCK_SIZE :   /* 获取一个扇区里面擦除块的大小 */

            if(USBH_MSC_GetLUNInfo(&hUSBHost, MSC_Handle->current_lun, &info) == USBH_OK)
            {
                *(DWORD*)buff = info.capacity.block_size/USB_DEFAULT_BLOCK_SIZE;
                printf("每个扇区擦除块:%d\r\n", info.capacity.block_size/USB_DEFAULT_BLOCK_SIZE);
                res = RES_OK;
            }
            else
            {
                res = RES_ERROR;
            }
            break;
        default:
            res = RES_PARERR;
            break;
    }
    return res;
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

