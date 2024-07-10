/**
  ******************************************************************************
  * @file    USB_Device/MSC_Standalone/Src/usbd_storage.c
  * @author  MCD Application Team
  * @brief   Memory management layer
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
#include "usbd_storage.h"
//#include "stm324xg_eval_sd.h"
#include "norflash.h"  //W25Q128
#include "usart.h"

/* 文件系统在外部FLASH的起始地址
 * 我们定义SPI FLASH前12M给文件系统用, 所以地址从0开始
 */
#define USB_STORAGE_FLASH_BASE  0

/* 自己定义的一个标记USB状态的寄存器 */
/* bit0:表示电脑正在向SD卡写入数据 */
/* bit1:表示电脑正从SD卡读出数据 */
/* bit2:SD卡写数据错误标志位 */
/* bit3:SD卡读数据错误标志位 */
/* bit4:1,表示电脑有轮询操作(表明连接还保持着) */
volatile uint8_t g_usb_state_reg = 0;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*
#define STORAGE_LUN_NBR                  1  
#define STORAGE_BLK_NBR                  0x10000  
#define STORAGE_BLK_SIZ                  0x200
*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t writestatus, readstatus = 0;
/* USB Mass storage Standard Inquiry Data */
int8_t STORAGE_Inquirydata[] = { /* 36 */
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (STANDARD_INQUIRY_DATA_LEN - 5),
  0x00,
  0x00,	
  0x00,
  'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', /* Manufacturer: 8 bytes  */
  'S', 'P', 'I', 'F', 'l', 'a', 's', 'h', /* Product     : 16 Bytes */
  ' ', 'D', 'i', 's', 'k', ' ', ' ', ' ',
  '0', '.', '0','1',                      /* Version     : 4 Bytes  */
}; 

/* Private function prototypes -----------------------------------------------*/
int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

USBD_StorageTypeDef USBD_DISK_fops = {
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata, 
};
/* Private functions ---------------------------------------------------------*/

/**
 * @brief       初始化存储设备
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SPI FLASH
 * @retval      操作结果
 *   @arg       0    , 成功
 *   @arg       其他 , 错误代码
 */
int8_t STORAGE_Init(uint8_t lun)
{
    uint8_t res;
    switch(lun)
    {
        case 0:     /* SPI FLASH */
            norflash_init();
            res = USBD_OK;
						printf("STORAGE_Init_FS->norflash_init\n");
						//HAL_Delay(10000);
            break;
        default :
            res = USBD_FAIL;
    }
    return res;
}

/**
 * @brief       获取存储设备的容量和块大小
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SPI FLASH
 * @param       block_num  : 块数量(扇区数)
 * @param       block_size : 块大小(扇区大小)
 * @retval      操作结果
 *   @arg       0    , 成功
 *   @arg       其他 , 错误代码
 */
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    uint8_t res;
    switch(lun)
    {
        case 0:     /* SPI FLASH */
            *block_size = 512;
            *block_num = (1024 * 1024 * 12) / 512;    /* SPI FLASH的前面12M字节,文件系统用 */
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }
    
    return res;
}
/**
 * @brief       查看存储设备是否就绪
 * @param       lun        : 逻辑单元编号
 *   @arg                  1, SPI FLASH
 * @retval      就绪状态
 *   @arg       0    , 就绪
 *   @arg       其他 , 未就绪
 */
int8_t STORAGE_IsReady(uint8_t lun)
{
    uint8_t res;

    g_usb_state_reg |= 0x10;    /* 标记轮询 */

    switch(lun)
    {
        case 0:                 /* SPI FLASH */
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }
    
    return res;
}

/**
 * @brief       查看存储设备是否写保护
 * @param       lun        : 逻辑单元编号
 *   @arg                  1, SPI FLASH
 * @retval      读保护状态
 *   @arg       0    , 没有读保护
 *   @arg       其他 , 有读保护
 */
int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
    uint8_t res;
    switch(lun)
    {
        case 0:     /* SPI FLASH */
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }

    return res;
}

/**
  * @brief  Reads data from the medium.
  * @param  lun: Logical unit number
  * @param  blk_addr: Logical block address
  * @param  blk_len: Blocks number
  * @retval Status (0: Ok / -1: Error)
  */
/**
 * @brief       从存储设备读取数据
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SPI FLASH
 * @param       buf        : 数据存储区首地址指针
 * @param       blk_addr   : 要读取的地址(扇区地址)
 * @param       blk_len    : 要读取的块数(扇区数)
 * @retval      操作结果
 *   @arg       0    , 成功
 *   @arg       其他 , 错误代码
 */
int8_t STORAGE_Read(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    uint8_t res;

    g_usb_state_reg |= 0x02;    /* 标记正在读数据 */
    
    switch(lun)
    {
        case 0:                 /* SPI FLASH */
            norflash_read(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }
    
    return res;
}
/**
 * @brief       向存储设备写数据
 * @param       lun        : 逻辑单元编号
 *   @arg                  0, SPI FLASH
 * @param       buf        : 数据存储区首地址指针
 * @param       blk_addr   : 要写入的地址(扇区地址)
 * @param       blk_len    : 要写入的块数(扇区数)
 * @retval      操作结果
 *   @arg       0    , 成功
 *   @arg       其他 , 错误代码
 */
int8_t STORAGE_Write(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
    uint8_t res;
    
    g_usb_state_reg |= 0x01;    /* 标记正在写数据 */

    switch(lun)
    {
        case 0:     /* SPI FLASH */
            norflash_write(buf, USB_STORAGE_FLASH_BASE + blk_addr * 512, blk_len * 512);
            res = USBD_OK;
            break;
        default :
            res = USBD_FAIL;
    }
    
    return res;
}

/**
 * @brief       获取支持的最大逻辑单元个数
 *   @note      注意, 这里返回的逻辑单元个数是减去了1的.
 *              0, 就表示1个; 1, 表示2个; 以此类推
 * @param       无
 * @retval      支持的逻辑单元个数 - 1
 */
int8_t STORAGE_GetMaxLun(void)
{
    return STORAGE_LUN_NBR - 1;
}
 
/**
  * @brief BSP Tx Transfer completed callbacks
  * @param None
  * @retval None
  */
void BSP_SD_WriteCpltCallback(void)
{
  writestatus = 1;
}

/**
  * @brief BSP Rx Transfer completed callbacks
  * @param None
  * @retval None
  */
void BSP_SD_ReadCpltCallback(void)
{
  readstatus = 1;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

