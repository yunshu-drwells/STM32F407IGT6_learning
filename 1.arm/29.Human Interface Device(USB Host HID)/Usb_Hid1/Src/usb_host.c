/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
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

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_hid.h"

/* USER CODE BEGIN Includes */
#include "delay.h"
#include "lcd.h"

#include "usart.h"
#include "usbh_core.h"
#include "usbh_hid.h"
#include "usbh_hid_parser.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

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

extern HID_MOUSE_Info_TypeDef mouse_info;
ApplicationTypeDef App_State = APPLICATION_IDLE;

uint8_t g_usb_first_plugin_flag = 0;    /* USB第一次插入标志,如果为1,说明是第一次插入 */

/**
 * @brief       USB信息显示
 * @param       msg   : 信息类型
 *   @arg               0, USB无连接
 *   @arg               1, USB键盘
 *   @arg               2, USB鼠标
 *   @arg               3, 不支持的USB设备
 * @retval      无
 */
void usbh_msg_show(uint8_t msgx)
{
    switch (msgx)
    {
        case 0: /* USB无连接 */
            lcd_show_string(30, 130, 200, 16, 16, "USB Connecting...", RED);
            lcd_fill(0, 150, lcddev.width, lcddev.height, WHITE);
            break;

        case 1: /* USB键盘 */
            lcd_show_string(30, 130, 200, 16, 16, "USB Connected    ", RED);
            lcd_show_string(30, 150, 200, 16, 16, "USB KeyBoard", RED);
            lcd_show_string(30, 180, 210, 16, 16, "KEYVAL:", RED);
            lcd_show_string(30, 200, 210, 16, 16, "INPUT STRING:", RED);
            break;

        case 2: /* USB鼠标 */
            lcd_show_string(30, 130, 200, 16, 16, "USB Connected    ", RED);
            lcd_show_string(30, 150, 200, 16, 16, "USB Mouse", RED);
            lcd_show_string(30, 180, 210, 16, 16, "BUTTON:", RED);
            lcd_show_string(30, 200, 210, 16, 16, "X POS:", RED);
            lcd_show_string(30, 220, 210, 16, 16, "Y POS:", RED);
            lcd_show_string(30, 240, 210, 16, 16, "Z POS:", RED);
            break;

        case 3: /* 不支持的USB设备 */
            lcd_show_string(30, 130, 200, 16, 16, "USB Connected    ", RED);
            lcd_show_string(30, 150, 200, 16, 16, "Unknow Device", RED);
            break;
    }
}


/* 临时数组,用于存放鼠标坐标/键盘输入内容(4.3屏,最大可以输入2016字节) */
#if !(__ARMCC_VERSION >= 6010050)   /* 不是AC6编译器，即使用AC5编译器时 */
__align(4) uint8_t g_temp_buffer[2017];
#else      /* 使用AC6编译器时 */
__ALIGNED(4) uint8_t g_temp_buffer[2017];
#endif

/**
 * @brief       USB鼠标数据处理
 * @param       data  : USB鼠标数据结构体指针
 * @retval      无
 */
void mouse_data_process(HID_MOUSE_Info_TypeDef *data)
{
    static signed short x, y, z;

    if (g_usb_first_plugin_flag)    /* 第一次插入,将数据清零 */
    {
        g_usb_first_plugin_flag = 0;
        x = y = z = 0;
    }

    x += (signed char)data->x;

    if (x > 9999)x = 9999;

    if (x < -9999)x = -9999;

    y += (signed char)data->y;

    if (y > 9999)y = 9999;

    if (y < -9999)y = -9999;

    z += (signed char)data->z;

    if (z > 9999)z = 9999;

    if (z < -9999)z = -9999;

    sprintf((char *)g_temp_buffer, "BUTTON:");

    if (data->button & 0X01)strcat((char *)g_temp_buffer, "LEFT ");

    if ((data->button & 0X03) == 0X02)
    {
        strcat((char *)g_temp_buffer, "RIGHT");
    }
    else
    {
        if ((data->button & 0X03) == 0X03)strcat((char *)g_temp_buffer, "+RIGHT");
    }
    
    if ((data->button & 0X07) == 0X04)
    {
        strcat((char *)g_temp_buffer, "MID  ");
    }
    else
    {
        if ((data->button & 0X07) > 0X04)strcat((char *)g_temp_buffer, "+MID");
    }
    
    lcd_fill(30 + 56, 180, lcddev.width - 1, 180 + 16, WHITE);
    lcd_show_string(30, 180, 210, 16, 16, (char *)g_temp_buffer, BLUE);
    sprintf((char *)g_temp_buffer, "X POS:%05d", x);
    lcd_show_string(30, 200, 200, 16, 16, (char *)g_temp_buffer, BLUE);
    sprintf((char *)g_temp_buffer, "Y POS:%05d", y);
    lcd_show_string(30, 220, 200, 16, 16, (char *)g_temp_buffer, BLUE);
    sprintf((char *)g_temp_buffer, "Z POS:%05d", z);
    lcd_show_string(30, 240, 200, 16, 16, (char *)g_temp_buffer, BLUE);
    //printf("btn,X,Y,Z:0x%x,%d,%d,%d\r\n",data->button,(signed char)data->x,(signed char)data->y,(signed char)data->z);
}

/**
 * @brief       USB键盘数据处理
 * @param       data  : USB键盘键值
 * @retval      无
 */
void keybrd_data_process(uint8_t data)
{
    static uint16_t pos;
    static uint16_t endx, endy;
    static uint16_t maxinputchar;

    uint8_t buf[4];

    if (g_usb_first_plugin_flag)    /* 第一次插入,将数据清零 */
    {
        g_usb_first_plugin_flag = 0;
        endx = ((lcddev.width - 30) / 8) * 8 + 30;      /* 得到endx值 */
        endy = ((lcddev.height - 220) / 16) * 16 + 220; /* 得到endy值 */
        maxinputchar = ((lcddev.width - 30) / 8);
        maxinputchar *= (lcddev.height - 220) / 16;     /* 当前LCD最大可以显示的字符数 */
        pos = 0;
    }

    sprintf((char *)buf, "%02X", data);
    lcd_show_string(30 + 56, 180, 200, 16, 16, (char *)buf, BLUE);  /* 显示键值 */

    if (data >= ' ' && data <= '~')
    {
        g_temp_buffer[pos++] = data;
        g_temp_buffer[pos] = 0; /* 添加结束符 */

        if (pos > maxinputchar)pos = maxinputchar;  /* 最大输入这么多 */
    }
    else if (data == 0X0D)      /* 退格键 */
    {
        if (pos)pos--;

        g_temp_buffer[pos] = 0; /* 添加结束符 */
    }

    if (pos <= maxinputchar)    /* 没有超过显示区 */
    {
        lcd_fill(30, 220, endx, endy, WHITE);
        lcd_show_string(30, 220, endx - 30, endy - 220, 16, (char *)g_temp_buffer, BLUE);
    }

    //printf("KEY Board Value:%02X\r\n",data);
    //printf("KEY Board Char:%c\r\n",data);
}

/**
 * @brief       USB键盘/鼠标演示demo数据处理
 * @param       phost : USBH句柄指针
 * @retval      无
 */
void usb_demo(USBH_HandleTypeDef *phost)
{
    char c;
    HID_KEYBD_Info_TypeDef *k_pinfo;
    HID_MOUSE_Info_TypeDef *m_pinfo;

    if (App_State == APPLICATION_READY)
    {
        if (USBH_HID_GetDeviceType(phost) == HID_KEYBOARD)  /* 键盘设备 */
        {
            k_pinfo = USBH_HID_GetKeybdInfo(phost);         /* 获取键盘信息 */

            if (k_pinfo != NULL)
            {
                c = USBH_HID_GetASCIICode(k_pinfo);         /* 转换成ASCII码 */
                keybrd_data_process(c);                     /* 在LCD上显示出键盘字符 */
            }
        }
        else if (USBH_HID_GetDeviceType(phost) == HID_MOUSE)/* 鼠标设备 */
        {
						//printf("this is mouse device\n");
            m_pinfo = USBH_HID_GetMouseInfo(phost);         /* 获取鼠标信息 */
						/*
						if(NULL == m_pinfo){
							printf("m_pinfo is empty\n");
						}
						*/
            if (m_pinfo != NULL)
            {
                mouse_data_process(&mouse_info);            /* LCD上显示鼠标信息 */
            }
        }
    }
}


/****************************************************************************************************************************/
/* 以下代码 usbh_check_enume_dead 和 usbh_hid_reconnect 用于HID死机处理,提高代码健壮性 */

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
    HID_Class.DeInit(&hUsbHostFS);    /* 复位HID */
    USBH_DeInit(&hUsbHostFS);         /* 复位USB HOST */
    
    /* 重新复位USB */
    RCC->AHB2RSTR |= 1 << 7;        /* USB OTG FS 复位 */
    delay_ms(5);
    RCC->AHB2RSTR &= ~(1 << 7);     /* 复位结束 */

    memset(&hUsbHostFS, 0, sizeof(hUsbHostFS)); /* 清零数据 */
    
    /* 重新连接USB HID设备 */
    USBH_Init(&hUsbHostFS, USBH_UserProcess, 0);
    USBH_RegisterClass(&hUsbHostFS, USBH_HID_CLASS);
    USBH_Start(&hUsbHostFS);
}

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
  USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS);

  USBH_RegisterClass(&hUsbHostFS, USBH_HID_CLASS);

  USBH_Start(&hUsbHostFS);

  /* USER CODE BEGIN USB_HOST_Init_PostTreatment */
  
  /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * Background task
 */
void MX_USB_HOST_Process(void)
{
  /* USB Host Background task */
  USBH_Process(&hUsbHostFS);
}
/*
 * user callback definition
 */
static void USBH_UserProcess  (USBH_HandleTypeDef *phost, uint8_t id)
{
  /* USER CODE BEGIN CALL_BACK_1 */
uint8_t dev_type = 0XFF;    /* 设备类型,1,键盘;2,鼠标;其他,不支持的设备 */

    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_DISCONNECTION:
            usbh_msg_show(0);   /* 显示已经断开连接,准备重新连接 */
            App_State = APPLICATION_DISCONNECT;
            break;

        case HOST_USER_CLASS_ACTIVE:
            App_State = APPLICATION_READY;
            dev_type = phost->device.CfgDesc.Itf_Desc[phost->device.current_interface].bInterfaceProtocol;

            if (dev_type == HID_KEYBRD_BOOT_CODE)       /* 键盘设备 */
            {
                g_usb_first_plugin_flag = 1;    /* 标记第一次插入 */
                usbh_msg_show(1);   /* 显示键盘界面 */
            }
            else if (dev_type == HID_MOUSE_BOOT_CODE)   /* 鼠标设备 */
            {
                g_usb_first_plugin_flag = 1;    /* 标记第一次插入 */
                usbh_msg_show(2);   /* 显示鼠标界面 */
            }
            else
            {
                usbh_msg_show(3);   /* 显示不支持 */
            }
            
            break;

        case HOST_USER_CONNECTION:
            App_State = APPLICATION_START;
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



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
