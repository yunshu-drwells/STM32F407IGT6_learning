/**
  ******************************************************************************
  * @file    USB_Device/MSC_Standalone/Src/usbd_conf.c
  * @author  MCD Application Team
  * @brief   This file implements the USB Device library callbacks and MSP
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
  *//* Includes ------------------------------------------------------------------*/

/**
 ****************************************************************************************************
 * @file        usbd_conf.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-11-24
 * @brief       usbd_conf 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：正点原子 F407电机开发板
 * 在线视频：www.yuanzige.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 *
 * 修改说明
 * V1.0 20211124
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "usbd_conf.h"
#include "usbd_core.h"
#include "usbd_def.h"

#include "stm32f4xx_hal.h"

#include "usart.h"

//是否打开调试信息
#if 0
#define openDebugMess
#endif

/* PCD_USB_OTG_FS 定义 */
PCD_HandleTypeDef g_pcd_usb_otg_fs;

/* USB连接状态
 * 0,没有连接;
 * 1,已经连接;
 */
volatile uint8_t g_device_state = 0;    /* 默认没有连接 */

USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status);   /* 返回USB的状态 */

/**
 * @brief       初始化PCD MSP
 * @param       hpcd : PCD结构体指针
 * @retval      无
 */
 /*
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
    GPIO_InitTypeDef gpio_init_struct = {0};

    if (hpcd->Instance == USB_OTG_FS)
    {

        __HAL_RCC_GPIOA_CLK_ENABLE();                       // 使能GPIOA时钟 
        gpio_init_struct.Pin = GPIO_PIN_11 | GPIO_PIN_12;   // 初始化GPIO口 
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;            // 复用 
        gpio_init_struct.Pull = GPIO_NOPULL;                // 浮空 
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 高速 
        gpio_init_struct.Alternate = GPIO_AF10_OTG_FS;      // 复用为OTG_FS 
        HAL_GPIO_Init(GPIOA, &gpio_init_struct);            // 初始化PA11和PA12引脚 

        __HAL_RCC_USB_OTG_FS_CLK_ENABLE();                  // 使能OTG FS时钟 

        HAL_NVIC_SetPriority(OTG_FS_IRQn, 1, 0);            // 优先级设置为抢占1,子优先级0 
        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);                    // 使能OTG FS中断 
    }
    else if (hpcd->Instance == USB_OTG_HS)
    {
        // USB OTG HS本例程没用到,故不做处理 
    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle)
{
    if (pcdHandle->Instance == USB_OTG_FS)
    {
        // USER CODE BEGIN USB_OTG_FS_MspDeInit 0 

        // USER CODE END USB_OTG_FS_MspDeInit 0 
        // Peripheral clock disable 
        __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

        //USB_OTG_FS GPIO Configuration
        //PA11     ------> USB_OTG_FS_DM
        //PA12     ------> USB_OTG_FS_DP
        
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11 | GPIO_PIN_12);

        HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
    }
}
*/

/**
 * @brief       USB OTG 中断服务函数
 *   @note      处理所有USB中断
 * @param       无
 * @retval      无
 */
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&g_pcd_usb_otg_fs);
}

/**
 * @brief       USBD 配置阶段回调函数
 * @param       hpcd    : PCD结构体指针
 * @retval      无
 */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{		
#ifdef openDebugMess
		printf("usbd_conf.c->HAL_PCD_SetupStageCallback\n");
#endif	
    USBD_LL_SetupStage((USBD_HandleTypeDef*)hpcd->pData, (uint8_t *) hpcd->Setup);
}

/**
 * @brief       USBD OUT 阶段回调函数
 * @param       hpcd    : PCD结构体指针
 * @param       epnum   : 端点号
 * @retval      无
 */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
#ifdef openDebugMess
		printf("usbd_conf.c->HAL_PCD_DataOutStageCallback\n");
#endif	
    USBD_LL_DataOutStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
 * @brief       USBD IN 阶段回调函数
 * @param       hpcd    : PCD结构体指针
 * @param       epnum   : 端点号
 * @retval      无
 */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
#ifdef openDebugMess
		printf("usbd_conf.c->HAL_PCD_DataInStageCallback\n");
#endif
    USBD_LL_DataInStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
 * @brief       USBD SOF 回调函数
 * @param       hpcd    : PCD结构体指针
 * @retval      无
 */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
#ifdef openDebugMess
		printf("usbd_conf.c->HAL_PCD_SOFCallback\n");
#endif
    USBD_LL_SOF((USBD_HandleTypeDef*)hpcd->pData);
}

/**
 * @brief       USBD 复位回调函数
 * @param       hpcd    : PCD结构体指针
 * @retval      无
 */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    /* Set USB Current Speed */
    switch (hpcd->Init.speed)
    {
        case PCD_SPEED_HIGH:
						printf("USB Device Library  [HS]\n");
#ifdef openDebugMess
            printf("usbd_conf.c->HAL_PCD_ResetCallback: USB Device Library  [HS]\n");
#endif				
            speed = USBD_SPEED_HIGH;
            break;

        case PCD_SPEED_FULL:
						printf("USB Device Library  [FS]\n");
#ifdef openDebugMess					
            printf("usbd_conf.c->HAL_PCD_ResetCallback: USB Device Library  [FS]\n");
#endif				
            speed = USBD_SPEED_FULL;
            break;

        default:
						printf("USB Device Library  [FS?]\n");
#ifdef openDebugMess					
            printf("usbd_conf.c->HAL_PCD_ResetCallback: USB Device Library  [FS?]\n");
#endif				
            speed = USBD_SPEED_FULL;
            break;
    }
    
    USBD_LL_SetSpeed((USBD_HandleTypeDef*)hpcd->pData, speed);
    USBD_LL_Reset((USBD_HandleTypeDef*)hpcd->pData); /* 复位设备 */
}

/**
 * @brief       USBD 挂起回调函数
 * @param       hpcd    : PCD结构体指针
 * @retval      无
 */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    g_device_state = 0;
#ifdef openDebugMess	
		printf("usbd_conf.c->HAL_PCD_SuspendCallback: Device In suspend mode\n");
#endif	
    USBD_LL_Suspend((USBD_HandleTypeDef*)hpcd->pData);
    __HAL_PCD_GATE_PHYCLOCK(hpcd);

    if (hpcd->Init.low_power_enable)
    {
        /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
        SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
    }
}

/**
 * @brief       USBD 恢复回调函数
 * @param       hpcd    : PCD结构体指针
 * @retval      无
 */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
#ifdef openDebugMess	
    printf("usbd_conf.c->Device Resumed\n");
#endif	
    USBD_LL_Resume((USBD_HandleTypeDef*)hpcd->pData);
}

/**
 * @brief       USBD ISO OUT 事务完成回调函数
 * @param       hpcd    : PCD结构体指针
 * @param       epnum   : 端点号
 * @retval      无
 */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
#ifdef openDebugMess	
		printf("usbd_conf.c->HAL_PCD_ISOOUTIncompleteCallback\n");
#endif	
    USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

/**
 * @brief       USBD ISO IN 事务完成回调函数
 * @param       hpcd    : PCD结构体指针
 * @param       epnum   : 端点号
 * @retval      无
 */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
#ifdef openDebugMess	
		printf("usbd_conf.c->HAL_PCD_ISOINIncompleteCallback\n");
#endif	
    USBD_LL_IsoINIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

/**
 * @brief       USBD 连接成功回调函数
 * @param       hpcd    : PCD结构体指针
 * @retval      无
 */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
#ifdef openDebugMess	
		printf("usbd_conf.c->HAL_PCD_ConnectCallback\n");
#endif	
    g_device_state = 1;
    USBD_LL_DevConnected((USBD_HandleTypeDef*)hpcd->pData);
}

/**
 * @brief       USBD 断开连接回调函数
 * @param       hpcd    : PCD结构体指针
 * @retval      无
 */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
#ifdef openDebugMess	
		printf("usbd_conf.c->HAL_PCD_DisconnectCallback\n");
#endif	
    g_device_state = 0;
    printf("USB Device Disconnected.\n");
    USBD_LL_DevDisconnected((USBD_HandleTypeDef*)hpcd->pData);
}

/******************************************************************************************/
/* 以下是: USBD LL 驱动接口函数(PCD->USB Device Library) */

/**
 * @brief       USBD 底层初始化函数
 * @param       pdev    : USBD句柄指针
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
    if (pdev->id == DEVICE_FS)
    {
#ifdef openDebugMess			
				printf("usbd_conf.c->USBD_LL_Init->id is DEVICE_FS\n");
#endif			
        g_pcd_usb_otg_fs.pData = pdev;                          /* g_pcd_usb_otg_fs的pData指向pdev */
        pdev->pData = &g_pcd_usb_otg_fs;                        /* pdev的pData指向g_pcd_usb_otg_fs */

        g_pcd_usb_otg_fs.Instance = USB_OTG_FS;                 /* 使用USB OTG FS*/
        g_pcd_usb_otg_fs.Init.dev_endpoints = 4;                /* 端点数为4 */
        g_pcd_usb_otg_fs.Init.speed = PCD_SPEED_FULL;           /* USB全速(12Mbps) */
        g_pcd_usb_otg_fs.Init.dma_enable = DISABLE;             /* 不使能DMA */
        g_pcd_usb_otg_fs.Init.phy_itface = PCD_PHY_EMBEDDED;    /* 使用内部PHY */
        g_pcd_usb_otg_fs.Init.Sof_enable = DISABLE;             /* 不使能SOF中断 */
        g_pcd_usb_otg_fs.Init.low_power_enable = DISABLE;       /* 不使能低功耗模式 */
        g_pcd_usb_otg_fs.Init.lpm_enable = DISABLE;             /* 不使能连接电源管理 */
        g_pcd_usb_otg_fs.Init.vbus_sensing_enable = DISABLE;    /* 不使能VBUS检测 */
        g_pcd_usb_otg_fs.Init.use_dedicated_ep1 = DISABLE;      /* 禁止EP1 dedicated中断 */

        HAL_PCD_Init(&g_pcd_usb_otg_fs);

        HAL_PCDEx_SetRxFiFo(&g_pcd_usb_otg_fs, 0x80);           /* 设置接收FIFO大小为0x80(128字节) */
        HAL_PCDEx_SetTxFiFo(&g_pcd_usb_otg_fs, 0, 0x40);        /* 设置发送FIFO 0的大小为0x40(64字节) */
        HAL_PCDEx_SetTxFiFo(&g_pcd_usb_otg_fs, 1, 0x80);        /* 设置发送FIFO 1的大小为0x80(128字节) */
    }
#ifdef openDebugMess		
		printf("usbd_conf.c->USBD_LL_Init-> USBD_OK(0)\n");
#endif		
    return USBD_OK;
}

/**
 * @brief       USBD 底层取消初始化(回复默认复位状态)函数
 * @param       pdev    : USBD句柄指针
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_DeInit(pdev->pData);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_DeInit->usb_status:%d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 底层驱动开始工作
 * @param       pdev    : USBD句柄指针
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Start(pdev->pData);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("USBD_LL_Start is USBD_OK(%d)\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 底层驱动停止工作
 * @param       pdev    : USBD句柄指针
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Stop(pdev->pData);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_Stop-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 初始化(打开)某个端点
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @param       ep_type : 端点类型
 * @param       ep_mps  : 端点最大包容量(字节)
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_OpenEP-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 取消初始化(关闭)某个端点
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_CloseEP-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 清空某个端点的数据
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_FlushEP-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 给某个端点设置一个暂停状态
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_StallEP-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 取消某个端点的暂停状态
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_ClearStallEP-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 返回是否处于暂停状态
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @retval      0, 非暂停; 1, 暂停;
 */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *) pdev->pData;

    if ((ep_addr & 0x80) == 0x80)
    {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    }
    else
    {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

/**
 * @brief       USBD 为设备指定新的USB地址
 * @param       pdev    : USBD句柄指针
 * @param       dev_addr: 新的设备地址,USB_OTG_FS/USB_OTG_HS
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
    g_device_state = 1;
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_SetUSBAddress-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 通过端点发送数据
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @param       pbuf    : 数据缓冲区首地址
 * @param       size    : 要发送的数据大小
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_Transmit-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 准备一个端点接收数据
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @param       pbuf    : 数据缓冲区首地址
 * @param       size    : 要接收的数据大小
 * @retval      USB状态
 *   @arg       USBD_OK(0)   , 正常;
 *   @arg       USBD_BUSY(1) , 忙;
 *   @arg       USBD_FAIL(2) , 失败;
 */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint32_t size)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

    usb_status =  USBD_Get_USB_Status(hal_status);
#ifdef openDebugMess	
		printf("usbd_conf.c->USBD_LL_PrepareReceive-> usb_status: %d\n", usb_status);
#endif	
    return usb_status;
}

/**
 * @brief       USBD 获取最后一个传输包的大小
 * @param       pdev    : USBD句柄指针
 * @param       ep_addr : 端点号
 * @retval      包大小
 */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef *) pdev->pData, ep_addr);
}

/**
 * @brief       USBD 延时函数(以ms为单位)
 * @param       Delay   : 延时的ms数
 * @retval      无
 */
void USBD_LL_Delay(uint32_t Delay)
{
    HAL_Delay(Delay);
}


/**
 * @brief       返回USB的状态(HAL状态)
 * @param       hal_status : 当前的状态
 * @retval      无
 */
USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
    USBD_StatusTypeDef usb_status = USBD_OK;

    switch (hal_status)
    {
        case HAL_OK :
            usb_status = USBD_OK;
            break;

        case HAL_ERROR :
            usb_status = USBD_FAIL;
            break;

        case HAL_BUSY :
            usb_status = USBD_BUSY;
            break;

        case HAL_TIMEOUT :
            usb_status = USBD_FAIL;
            break;

        default :
            usb_status = USBD_FAIL;
            break;
    }

    return usb_status;
}
