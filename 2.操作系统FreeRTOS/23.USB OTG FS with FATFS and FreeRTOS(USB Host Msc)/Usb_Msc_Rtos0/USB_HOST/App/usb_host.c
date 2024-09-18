/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"

/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "exfuns.h"  //fs[]
#include "usbh_msc.h"  //USBH_msc
#include "delay.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t usb_mount_status = 0;               /* USB 挂载状态 */
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void MX_USB_HOST_Init(void)
{
  /* USER CODE BEGIN USB_HOST_Init_PreTreatment */

  /* USER CODE END USB_HOST_Init_PreTreatment */

  /* Init host Library, add supported class and start the library. */
  if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS) != USBH_OK)
  {
    Error_Handler();
  }
  if (USBH_Start(&hUsbHostFS) != USBH_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */

  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/**
 * @brief       USB枚举状态死机检测,防止USB枚举失败导致的死机
 * @param       phost : USB HOST结构体指针
 * @retval      0, 没有死机
 *              1, 死机了,外部必须重新启动USB连接.
 */
uint8_t usbh_check_enume_dead(USBH_HandleTypeDef *phost)
{
    static uint16_t errcnt = 0;

    /* 这个状态,如果持续存在,则说明USB死机了 */
    if (phost->gState == HOST_ENUMERATION && (phost->EnumState == ENUM_IDLE || phost->EnumState == ENUM_GET_FULL_DEV_DESC))
    {
        errcnt++;

        if (errcnt > 2000)              /* 死机了 */
        {
            errcnt = 0;
            return 1;
        }
    }
    else 
    {
        errcnt = 0;
    }

    return 0;
}

/**
 * @brief       USB HID重新连接
 * @param       无
 * @retval      无
 */
void usbh_hid_reconnect(void)
{
    /* 关闭之前的连接 */
    USBH_msc.DeInit(&hUsbHostFS);    /* 复位HID */
    USBH_DeInit(&hUsbHostFS);         /* 复位USB HOST */
    
    /* 重新复位USB */
    RCC->AHB2RSTR |= 1 << 7;        /* USB OTG FS 复位 */
    delay_ms(5);
    RCC->AHB2RSTR &= ~(1 << 7);     /* 复位结束 */

    memset(&hUsbHostFS, 0, sizeof(hUsbHostFS)); /* 清零数据 */
    
    /* 重新连接USB HID设备 */
    MX_USB_HOST_Init();
}
/* USER CODE END 0 */


/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
	//lcd_show_string(30, 140, 200, 16, 16, "USBH_UserProcess start\n", BLUE);
  /* USER CODE BEGIN CALL_BACK_1 */
	uint32_t total, free;
  uint8_t res = 0;
	switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_DISCONNECTION:
            f_mount(0, "1:", 1);        /* 卸载U盘 */
            lcd_show_string(30, 140, 200, 16, 16, "U Disk connecting...      ", BLUE);
            lcd_fill(30, 160, 239, 220, WHITE);
            break;

        case HOST_USER_CLASS_ACTIVE:
            lcd_show_string(30, 140, 200, 16, 16, "U Disk connect successful!", BLUE);
            res = f_mount(fs[1], "1:", 1);    /* 重新挂载U盘 */
						if(0 == res){
							lcd_show_string(30, 160, 200, 16, 16, "U Disk mount successful!", BLUE);
						}else
							lcd_show_string(30, 160, 200, 16, 16, "U Disk mount failed!", BLUE);
						
            res = exfuns_get_free("1:", &total, &free);

            if (res == 0)
            {
                lcd_show_string(30, 200, 200, 16, 16, "FATFS OK!", BLUE);
                lcd_show_string(30, 220, 200, 16, 16, "U Disk Total Size:     MB", BLUE);
                lcd_show_string(30, 240, 200, 16, 16, "U Disk  Free Size:     MB", BLUE);
                lcd_show_num(174, 220, total >> 10, 5, 16, BLUE);   /* 显示U盘总容量 MB */
                lcd_show_num(174, 240, free >> 10, 5, 16, BLUE);
								usb_mount_status = 1;
            }
            else
            {
                lcd_show_string(30, 200, 200, 16, 16, "FATFS ERROR!", BLUE);
                //printf("U盘存储空间获取失败\r\n");
								usb_mount_status = 0;   /* 标记 usb 挂载失败 */
            }
            break;

        case HOST_USER_CONNECTION:
            break;

        default:
            break;
    }
  /* USER CODE END CALL_BACK_1 */
}

/**
  * @}
  */

/**
  * @}
  */

