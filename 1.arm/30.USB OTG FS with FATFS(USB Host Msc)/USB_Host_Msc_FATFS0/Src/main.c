/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2024 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "spi.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fsmc.h"

/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"
#include "delay.h"

#include "usbh_def.h"  //USBH_HandleTypeDef

#include "norflash.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "exfuns.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//lcd
uint8_t lcd_id[12];
uint16_t colors[12] = {WHITE, BLACK, BLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, BRRED, GRAY, LGRAY, BROWN};
uint8_t color_index = 0;

USBH_HandleTypeDef hUSBHost;    /* USB Host处理结构体 */
uint8_t t = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

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
  MX_USB_OTG_FS_HCD_Init();
  MX_FSMC_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  //lcd init
	delay_init(168);                    // 延时初始化 
	lcd_init();                             // 初始化LCD 
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  // 将LCD ID打印到lcd_id数组
	
	my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
	my_mem_init(SRAMCCM);               /* 初始化内部SRAMCCM内存池 */
	
	exfuns_init();                      /* 为fatfs相关变量申请内存 */
	
	lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	lcd_show_string(30, 70, 200, 16, 16, "USB U Disk TEST", RED);
	lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	
	lcd_show_string(30, 110, 200, 16, 16, "USB Connecting...", RED);    /* 提示USB开始连接 */
	
	/*
	USBH_HandleTypeDef *phost = (USBH_HandleTypeDef *)malloc(sizeof(USBH_HandleTypeDef));
  memset(phost, 0, sizeof(USBH_HandleTypeDef));
  printf("After malloc and memset: phost->device.is_connected: %u\n", phost->device.is_connected);
	
	printf("Before USBH_Init: phost->id: %u\n", phost->id);
	printf("Before USBH_Init: phost->device.is_connected: %u\n", phost->device.is_connected);
	USBH_Init(phost, USBH_UserProcess, HOST_FS);
	printf("After USBH_Init: phost->id: %u\n", phost->id);
	printf("After USBH_Init: phost->device.is_connected: %u\n", phost->device.is_connected);
	
	USBH_RegisterClass(phost, USBH_MSC_CLASS);
	USBH_Start(phost);
	*/
	

	//printf("Before USBH_Init: phost->id: %u\n", hUSBHost.id);
	//printf("Before USBH_Init: phost->device.is_connected: %u\n", hUSBHost.device.is_connected);
	//printf("Before USBH_Init: phost->device.is_disconnected before: %u\n", hUSBHost.device.is_disconnected);
  //printf("Before USBH_Init: phost->device.is_ReEnumerated before: %u\n", hUSBHost.device.is_ReEnumerated);

	USBH_Init(&hUSBHost, USBH_UserProcess, HOST_FS);
	//printf("After USBH_Init: phost->id: %u\n", hUSBHost.id);
	//printf("After USBH_Init: phost->device.is_connected: %u\n", hUSBHost.device.is_connected);
	//printf("After USBH_Init: phost->device.is_disconnected before: %u\n", hUSBHost.device.is_disconnected);
  //printf("After USBH_Init: phost->device.is_ReEnumerated before: %u\n", hUSBHost.device.is_ReEnumerated);
	//printf("main->phost: %p\n", &hUSBHost);

	USBH_RegisterClass(&hUSBHost, USBH_MSC_CLASS);
	USBH_Start(&hUSBHost);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		USBH_Process(&hUSBHost);
		//printf("USB Host state: %d\n", hUSBHost.gState);  //检查 USB 主机状态
		delay_ms(10);
		t++;

		if (t == 50)
		{
				t = 0;
				HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
		}
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

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
		//printf("USBH_UserProcess is called\n");
    uint32_t total, free;
    uint8_t res = 0;

    switch (id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
            break;

        case HOST_USER_DISCONNECTION:
            f_mount(0, "1:", 1);        /* 卸载U盘 */
            lcd_show_string(30, 140, 200, 16, 16, "U Disk connecting...", BLUE);
            lcd_fill(30, 160, 239, 220, WHITE);
						//phost->device.is_connected = 0;
            break;

        case HOST_USER_CLASS_ACTIVE:
            lcd_show_string(30, 140, 200, 16, 16, "U Disk connect successful!", BLUE);
            f_mount(fs[1], "1:", 1);    /* 重新挂载U盘 */
            res = exfuns_get_free("1:", &total, &free);

            if (res == 0)
            {
                lcd_show_string(30, 160, 200, 16, 16, "FATFS OK!", BLUE);
                lcd_show_string(30, 180, 200, 16, 16, "U Disk Total Size:     MB", BLUE);
                lcd_show_string(30, 200, 200, 16, 16, "U Disk  Free Size:     MB", BLUE);
                lcd_show_num(174, 180, total >> 10, 5, 16, BLUE);   /* 显示U盘总容量 MB */
                lcd_show_num(174, 200, free >> 10, 5, 16, BLUE);
            }
            else
            {
                lcd_show_string(30, 160, 200, 16, 16, "FATFS ERROR!", BLUE);
                printf("U盘存储空间获取失败\r\n");
            }
            break;

        case HOST_USER_CONNECTION:
						//phost->device.is_connected = 1;
            break;

        default:
            break;
    }
}
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
