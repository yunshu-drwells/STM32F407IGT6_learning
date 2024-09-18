/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "stm32f4xx_hal.h"
#include "spi.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"
#include "fsmc.h"

/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "delay.h"

#include "usbh_def.h"  //USBH_HandleTypeDef

#include "norflash.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "exfuns.h"

#include "btim.h"  //btim_timx_int_init
#include "fonts.h"  //
#include "text.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//lcd
uint8_t lcd_id[12];
uint16_t colors[12] = {WHITE, BLACK, BLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, BRRED, GRAY, LGRAY, BROWN};
uint8_t color_index = 0;

extern USBH_HandleTypeDef hUsbHostFS;    /* USB Host处理结构体 在usb_host.c中定义*/
   
uint8_t res = 0, t = 0;

uint8_t usb_mount_status = 0;               /* USB 挂载状态 */

uint8_t i, j;
uint8_t fontx[2];   /* GBK码 */
uint32_t fontcnt = 0;

extern TIM_HandleTypeDef timx_handler;      /* 定时器参数句柄 */
#define FONTSECSIZE         791
#define FONTINFOADDR        12 * 1024 * 1024
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
/**
 * @brief       USB HID重新连接
 * @param       无
 * @retval      无
 */
void usbh_hid_reconnect(void)
{
    /* 关闭之前的连接 */
    USBH_DeInit(&hUsbHostFS);       /* 复位USB HOST */
    
    /* 重新复位USB */
    RCC->AHB2RSTR |= 1 << 7;        /* USB OTG FS 复位 */
    delay_ms(5);
    RCC->AHB2RSTR &= ~(1 << 7);     /* 复位结束 */

    memset(&hUsbHostFS, 0, sizeof(hUsbHostFS)); /* 清零数据 */
    MX_USB_HOST_Init();
		//循环操作直到挂载u盘成功
		while(usb_mount_status==0)          /* 等待 USB 设备挂载完成 */
		{
				lcd_clear(WHITE);               /* 清屏 */
				lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
				lcd_show_string(30, 50, 200, 16, 16, "USB Connecting...", RED);
				USBH_Process(&hUsbHostFS);
				delay_ms(20);
				//delay_ms(200);
		}
}
void erase_gbk_sector(){
	uint32_t *buf = mymalloc(SRAMIN, 4096);                           /* 申请4K字节内存 */
	uint16_t i, j;
	for (i = 0; i < FONTSECSIZE; i++)           // 先擦除字库区域,提高写入速度 
	{					
			norflash_read((uint8_t *)buf, ((FONTINFOADDR / 4096) + i) * 4096, 4096);    // 读出整个扇区的内容 

			for (j = 0; j < 1024; j++)              // 校验这个扇区的数据
			{
					if (buf[j] != 0xFFFFFFFF) break;    // 发现数据不为`0xFFFFFFFF`，则需要擦除该扇区 
			}

			if (j != 1024)
			{
					norflash_erase_sector((FONTINFOADDR / 4096) + i); // 需要擦除的扇区 
					//printf("norflash_erase_sector:%d\n", (FONTINFOADDR / 4096) + i);
			}
	}
	myfree(SRAMIN, buf);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USB_HOST_Init();    //完成了：USBH_Init、USBH_RegisterClass、USBH_Start
  MX_FSMC_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	//lcd init
	delay_init(168);                    // 延时初始化 
	lcd_init();                             // 初始化LCD 
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  // 将LCD ID打印到lcd_id数组
	
	my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
	my_mem_init(SRAMCCM);               /* 初始化内部SRAMCCM内存池 */
	norflash_init();
	exfuns_init();                      /* 为fatfs相关变量申请内存 */
	

	lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	lcd_show_string(30, 70, 200, 16, 16, "USB U Disk TEST", RED);
	lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	
	lcd_show_string(30, 110, 200, 16, 16, "USB Connecting...", RED);    // 提示USB开始连接
	
	//USBH_Init(&hUSBHost, USBH_UserProcess, HOST_FS);
	//USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
	//USBH_Start(&hUSBHost);
	
	
	delay_ms(1800);
  //btim_timx_int_init(1000, 840);      // 84 000 000 / 840 = 100 000 100Khz的计数频率，计数1000次为10ms
	//USBH_Process(&hUsbHostFS);
	//delay_ms(2800);
	btim_timx_int_init(1000, 840);  // 84 000 000 / 840 = 100 000 100Khz的计数频率，计数1000次为10ms
	//HAL_TIM_PeriodElapsedCallback中调用USBH_Process(&hUsbHostFS);
	
	//循环操作直到挂载u盘成功
	while(usb_mount_status==0)          // 等待 USB 设备挂载完成 
	{
			lcd_clear(WHITE);               // 清屏 
			lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
			lcd_show_string(30, 50, 200, 16, 16, "USB Connecting...", RED);
			//USBH_Process(&hUsbHostFS);
			delay_ms(20);
			//delay_ms(200);
	}

	//erase_gbk_sector();  //清空norflash已有的字库
	printf("erased norflash charactor lib\n");
	while (fonts_init())                // 检查字库 
	{
			printf("update font\r\n");
			usb_mount_status=0;
			lcd_clear(WHITE);               // 清屏 
		
			MX_USB_HOST_Init();
			delay_ms(1800);
			btim_timx_int_init(1000, 840);  /* 84 000 000 / 840 = 100 000 100Khz的计数频率，计数1000次为10ms */
		
			//循环操作直到挂载u盘成功
			while(usb_mount_status==0)          // 等待 USB 设备挂载完成 
			{
					lcd_clear(WHITE);               // 清屏 
					lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
					lcd_show_string(30, 50, 200, 16, 16, "USB Connecting...", RED);
					//USBH_Process(&hUsbHostFS);
					delay_ms(20);
					//delay_ms(200);
			}

			lcd_clear(WHITE);               // 清屏 
			lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
			lcd_show_string(30, 50, 200, 16, 16, "Font Updating...", RED);
			//usbh_hid_reconnect();

			res = fonts_update_font(20, 90, 16, (uint8_t *)"1:", RED);  // 更新字库
			while (res)         // 更新失败 
			{
					lcd_show_string(30, 90, 200, 16, 16, "Font Update Failed!", RED);
					delay_ms(200);
					lcd_fill(20, 90, 200 + 20, 90 + 16, WHITE);
					delay_ms(200);
			}
			TIM3->CR1&=~(1<<0);     // 关闭定时器3 
			lcd_show_string(30, 90, 200, 16, 16, "Font Update Success!   ", RED);
			delay_ms(1500);
			lcd_clear(WHITE);   // 清屏 
	}
	TIM3->CR1&=~(1<<0);     // 关闭定时器3 

	lcd_clear(WHITE);       // 清屏 
	text_show_string(30, 30, 200, 16, "正点原子STM32开发板", 16, 0, RED);
	text_show_string(30, 50, 200, 16, "GBK字库测试程序", 16, 0, RED);
	text_show_string(30, 70, 200, 16, "正点原子@ALIENTEK", 16, 0, RED);
	text_show_string(30, 90, 200, 16, "按KEY0,更新字库", 16, 0, RED);

	text_show_string(30, 110, 200, 16, "内码高字节:", 16, 0, BLUE);
	text_show_string(30, 130, 200, 16, "内码低字节:", 16, 0, BLUE);
	text_show_string(30, 150, 200, 16, "汉字计数器:", 16, 0, BLUE);

	text_show_string(30, 180, 200, 24, "对应汉字为:", 24, 0, BLUE);
	text_show_string(30, 204, 200, 16, "对应汉字(16*16)为:", 16, 0, BLUE);
	text_show_string(30, 220, 200, 16, "对应汉字(12*12)为:", 12, 0, BLUE);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

		for (i = 0x81; i < 0xff; i++)               // GBK内码高字节范围为0x81~0xFE
		{
				fontx[0] = i;
				lcd_show_num(118, 110, i, 3, 16, BLUE); // 显示内码高字节

				for (j = 0x40; j < 0xfe; j++)           // GBK内码低字节范围为 0x40~0x7E, 0x80~0xFE)
				{
						if (j == 0x7f)
								continue;

						fontcnt++;
						lcd_show_num(118, 130, j, 3, 16, BLUE);       // 显示内码低字节 
						lcd_show_num(118, 150, fontcnt, 5, 16, BLUE); // 汉字计数显示 
						fontx[1] = j;
						text_show_font(30 + 132, 180, fontx, 24, 0, BLUE);
						text_show_font(30 + 144, 204, fontx, 16, 0, BLUE);
						text_show_font(30 + 108, 220, fontx, 12, 0, BLUE);
						t = 200;
						
						HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
						delay_ms(20);
				}
		}

		/*
		USBH_Process(&hUsbHostFS);
		delay_ms(10);
		t++;
		
		if (t == 50)
		{
				t = 0;
				HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);

		}
		*/
		//lcd test
		/*
		lcd_clear(colors[color_index]);  //清屏

		lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
		lcd_show_string(10, 80, 240, 24, 24, "TFTLCD TEST", RED);
		lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
		lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); // 显示LCD ID 
		color_index++;

		if (color_index == 12){
				color_index = 0;
		}
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); // LED0闪烁 
		HAL_Delay(1000);
		*/


  /* USER CODE END WHILE */
    MX_USB_HOST_Process();

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
