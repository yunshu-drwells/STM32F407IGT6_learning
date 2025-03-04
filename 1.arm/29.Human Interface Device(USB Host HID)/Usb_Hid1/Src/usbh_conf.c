/**
  ******************************************************************************
  * @file           : usbh_conf.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the board support package for the USB host library
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2024 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"

/* USER CODE BEGIN Includes */
#include "delay.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

HCD_HandleTypeDef hhcd_USB_OTG_FS;
void _Error_Handler(char * file, int line);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
USBH_StatusTypeDef USBH_Get_USB_Status(HAL_StatusTypeDef hal_status);
/* USER CODE END PFP */

/* Private functions ---------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/*******************************************************************************
                       LL Driver Callbacks (HCD -> USB Host Library)
*******************************************************************************/
/* MSP Init */

void HAL_HCD_MspInit(HCD_HandleTypeDef* hcdHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(hcdHandle->Instance==USB_OTG_FS)
  {
  /* USER CODE BEGIN USB_OTG_FS_MspInit 0 */

  /* USER CODE END USB_OTG_FS_MspInit 0 */
  
    /**USB_OTG_FS GPIO Configuration    
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
  /* USER CODE BEGIN USB_OTG_FS_MspInit 1 */

  /* USER CODE END USB_OTG_FS_MspInit 1 */
  }
}

void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hcdHandle)
{
  if(hcdHandle->Instance==USB_OTG_FS)
  {
  /* USER CODE BEGIN USB_OTG_FS_MspDeInit 0 */

  /* USER CODE END USB_OTG_FS_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
  
    /**USB_OTG_FS GPIO Configuration    
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

  /* USER CODE BEGIN USB_OTG_FS_MspDeInit 1 */

  /* USER CODE END USB_OTG_FS_MspDeInit 1 */
  }
}

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
  USBH_LL_IncTimer(hhcd->pData);
}

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
	printf("Connected \r\n");
  USBH_LL_Connect(hhcd->pData);
}

/**
  * @brief  SOF callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
	printf("Disconnected \r\n");
  USBH_LL_Disconnect(hhcd->pData);
}

/**
  * @brief  Port Port Enabled callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd)
{
  printf("connect port open \r\n");
  USBH_LL_PortEnabled(hhcd->pData);
} 


/**
  * @brief  Port Port Disabled callback.
  * @param  hhcd: HCD handle
  * @retval None
  */
void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd)
{
  printf("connect port close \r\n");
  USBH_LL_PortDisabled(hhcd->pData);
} 


/**
  * @brief  Notify URB state change callback.
  * @param  hhcd: HCD handle
  * @param  chnum: channel number
  * @param  urb_state: state
  * @retval None
  */
void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
  /* To be used with OS to sync URB state with the global state machine */
#if (USBH_USE_OS == 1)
  USBH_LL_NotifyURBChange(hhcd->pData);
#endif
}

/*******************************************************************************
                       LL Driver Interface (USB Host Library --> HCD)
*******************************************************************************/

/* 以下是: USBH LL HCD 驱动接口函数(HCD->USB Host Library) */

/**
 * @brief       USBH 底层初始化函数
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_Init(USBH_HandleTypeDef *phost)
{
    /* Init USB_IP */
    if (phost->id == HOST_FS)
    {

        /* Link the driver to the stack. */
        hhcd_USB_OTG_FS.Instance = USB_OTG_FS;                /* 使用USB_OTG */
        hhcd_USB_OTG_FS.Init.Host_channels = 11;              /* 主机通道数为11个 */
        hhcd_USB_OTG_FS.Init.speed = HCD_SPEED_FULL;          /* USB全速 12M */
        hhcd_USB_OTG_FS.Init.dma_enable = DISABLE;            /* 不使用DMA */
        hhcd_USB_OTG_FS.Init.phy_itface = HCD_PHY_EMBEDDED;   /* 使能内部PHY */
        hhcd_USB_OTG_FS.Init.Sof_enable = DISABLE;            /* 禁止SOF中断 */
        hhcd_USB_OTG_FS.Init.vbus_sensing_enable = 0;         /* 不使能VBUS检测 */
        hhcd_USB_OTG_FS.Init.lpm_enable = 0;                  /* 使能连接电源管理 */
        hhcd_USB_OTG_FS.Init.low_power_enable = 0;            /* 不使能低功耗模式 */

        hhcd_USB_OTG_FS.pData = phost;
        phost->pData = &hhcd_USB_OTG_FS;
        
        if (HAL_HCD_Init(&hhcd_USB_OTG_FS) != HAL_OK)
        {
            printf("USB Init ERROR \r\n");
        }
        else {
            printf("USB Init OK \r\n");
        }
        USBH_LL_SetTimer(phost, HAL_HCD_GetCurrentFrame(&hhcd_USB_OTG_FS));
    }

    return USBH_OK;
}


/**
 * @brief       USBH 底层取消初始化(恢复默认复位状态)函数
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_DeInit(USBH_HandleTypeDef *phost)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBH_StatusTypeDef usb_status = USBH_OK;

    hal_status = HAL_HCD_DeInit(phost->pData);

    usb_status = USBH_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief       USBH 底层驱动开始工作
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_Start(USBH_HandleTypeDef *phost)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBH_StatusTypeDef usb_status = USBH_OK;

    hal_status = HAL_HCD_Start(phost->pData);

    usb_status = USBH_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief       USBH 底层驱动停止工作
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_Stop(USBH_HandleTypeDef *phost)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBH_StatusTypeDef usb_status = USBH_OK;

    hal_status = HAL_HCD_Stop(phost->pData);

    usb_status = USBH_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief       USBH 获取USB设备的速度
 * @param       phost       : USBH句柄指针
 * @retval      USBH 设备速度
 */
USBH_SpeedTypeDef USBH_LL_GetSpeed(USBH_HandleTypeDef *phost)
{
    USBH_SpeedTypeDef speed = USBH_SPEED_FULL;

    switch (HAL_HCD_GetCurrentSpeed(phost->pData))
    {
        case 0 :
            speed = USBH_SPEED_HIGH;
            printf("USB Host [HS]\r\n");
            break;

        case 1 :
            speed = USBH_SPEED_FULL;
            printf("USB Host [FS]\r\n");
            break;

        case 2 :
            speed = USBH_SPEED_LOW;
            printf("USB Host [LS]\r\n");
            break;

        default:
            printf("USB Host [FS?]\r\n");
            speed = USBH_SPEED_FULL;
            break;
    }

    return  speed;
}

/**
 * @brief       USBH 复位USB HOST端口
 * @param       phost       : USBH句柄指针
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_ResetPort(USBH_HandleTypeDef *phost)
{
    printf("USB reset port \r\n");
    
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBH_StatusTypeDef usb_status = USBH_OK;

    hal_status = HAL_HCD_ResetPort(phost->pData);

    usb_status = USBH_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief       USBH 获取最近一次传输的包大小
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @retval      包大小
 */
uint32_t USBH_LL_GetLastXferSize(USBH_HandleTypeDef *phost, uint8_t pipe)
{
    return HAL_HCD_HC_GetXferCount(phost->pData, pipe);
}

/**
 * @brief       USBH 打开一个管道(通道)
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @param       epnum       : 端点号
 * @param       dev_address : 设备地址
 * @param       speed       : 设备速度
 * @param       ep_type     : 端点类型
 * @param       mps         : 端点最大包大小
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_OpenPipe(USBH_HandleTypeDef *phost, uint8_t pipe_num, uint8_t epnum,
                                    uint8_t dev_address, uint8_t speed, uint8_t ep_type, uint16_t mps)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBH_StatusTypeDef usb_status = USBH_OK;

    hal_status = HAL_HCD_HC_Init(phost->pData, pipe_num, epnum,
                                 dev_address, speed, ep_type, mps);

    usb_status = USBH_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief       USBH 关闭一个管道(通道)
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_ClosePipe(USBH_HandleTypeDef *phost, uint8_t pipe)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBH_StatusTypeDef usb_status = USBH_OK;

    hal_status = HAL_HCD_HC_Halt(phost->pData, pipe);

    usb_status = USBH_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief       USBH 提交一个新的URB
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @param       direction   : 方向设置
 * @param       ep_type     : 端点类型
 * @param       token       : 端点标志
 * @param       pbuff       : URB数据区首地址
 * @param       length      : URB数据长度
 * @param       do_ping     : 激活do ping protocol,USB HS才用到
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_SubmitURB(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t direction,
                                     uint8_t ep_type, uint8_t token, uint8_t *pbuff, uint16_t length,
                                     uint8_t do_ping)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBH_StatusTypeDef usb_status = USBH_OK;

    hal_status = HAL_HCD_HC_SubmitRequest(phost->pData, pipe, direction,
                                          ep_type, token, pbuff, length,
                                          do_ping);
    usb_status =  USBH_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief       USBH 获取URB状态
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @retval      URB状态
  *     @arg URB_IDLE
  *     @arg URB_DONE
  *     @arg URB_NOTREADY
  *     @arg URB_NYET
  *     @arg URB_ERROR
  *     @arg URB_STALL
  */
USBH_URBStateTypeDef USBH_LL_GetURBState(USBH_HandleTypeDef *phost, uint8_t pipe)
{
    return (USBH_URBStateTypeDef)HAL_HCD_HC_GetURBState (phost->pData, pipe);
}

/**
 * @brief       USBH 控制VBUS状态
 * @param       phost       : USBH句柄指针
 * @param       state       : vbus状态. 0,激活; 1,不激活
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_DriverVBUS(USBH_HandleTypeDef *phost, uint8_t state)
{
    if (phost->id == HOST_FS)
    {
        if (state == 0)
        {
            
        }
        else
        {
            
        }
    }

    USBH_Delay(200);
    return USBH_OK;
}

/**
 * @brief       USBH 设置管道的翻转
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @param       toggle      : 翻转状态
 * @retval      USB状态
 *   @arg       USBH_OK(0)   , 正常;
 *   @arg       USBH_BUSY(1) , 忙;
 *   @arg       其他         , 失败;
 */
USBH_StatusTypeDef USBH_LL_SetToggle(USBH_HandleTypeDef *phost, uint8_t pipe, uint8_t toggle)
{
    HCD_HandleTypeDef *pHandle;
    pHandle = phost->pData;

    if (pHandle->hc[pipe].ep_is_in)
    {
        pHandle->hc[pipe].toggle_in = toggle;
    }
    else
    {
        pHandle->hc[pipe].toggle_out = toggle;
    }

    return USBH_OK;
}

/**
 * @brief       USBH 获取管道翻转状态
 * @param       phost       : USBH句柄指针
 * @param       pipe        : 管道编号
 * @retval      翻转状态
 */
uint8_t USBH_LL_GetToggle(USBH_HandleTypeDef *phost, uint8_t pipe)
{
    uint8_t toggle = 0;
    HCD_HandleTypeDef *pHandle;
    pHandle = phost->pData;

    if (pHandle->hc[pipe].ep_is_in)
    {
        toggle = pHandle->hc[pipe].toggle_in;
    }
    else
    {
        toggle = pHandle->hc[pipe].toggle_out;
    }

    return toggle;
}

/**
 * @brief       USBH 延时函数(以ms为单位)
 * @param       Delay       : 延时的ms数
 * @retval      翻转状态
 */
void USBH_Delay(uint32_t Delay)
{
    HAL_Delay(Delay);
}

/**
  * @brief  Retuns the USB status depending on the HAL status:
  * @param  hal_status: HAL status
  * @retval USB status
  */
USBH_StatusTypeDef USBH_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
    USBH_StatusTypeDef usb_status = USBH_OK;

    switch (hal_status)
    {
        case HAL_OK :
            usb_status = USBH_OK;
            break;

        case HAL_ERROR :
            usb_status = USBH_FAIL;
            break;

        case HAL_BUSY :
            usb_status = USBH_BUSY;
            break;

        case HAL_TIMEOUT :
            usb_status = USBH_FAIL;
            break;

        default :
            usb_status = USBH_FAIL;
            break;
    }

    return usb_status;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
