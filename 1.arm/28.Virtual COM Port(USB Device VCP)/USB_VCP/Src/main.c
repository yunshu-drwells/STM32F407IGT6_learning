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
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fsmc.h"

/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdio.h"

#include "delay.h"

//usb cdc
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//lcd
uint8_t lcd_id[12];
uint16_t colors[12] = {WHITE, BLACK, BLUE, RED, MAGENTA, GREEN, CYAN, YELLOW, BRRED, GRAY, LGRAY, BROWN};
uint8_t color_index = 0;

//usb cdc
USBD_HandleTypeDef USBD_Device;             /* USB Device����ṹ�� */
extern volatile uint8_t g_device_state;     /* USB���� ��� */
extern uint16_t g_usb_usart_rx_sta;         /* ����״̬��� */
uint16_t len;
uint16_t times = 0;
uint8_t usbstatus = 0;
/* �����ƴ���1�������ݵķ���,������USB���⴮�ڽ��յ������� */
extern uint8_t g_usb_usart_rx_buffer[USB_USART_REC_LEN];       /* ���ջ���,���USART_REC_LEN���ֽ� */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

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
  MX_USB_OTG_FS_PCD_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	//lcd init
	delay_init(168);                    // ��ʱ��ʼ�� 
	lcd_init();                             // ��ʼ��LCD 
  g_point_color = RED;
  sprintf((char *)lcd_id, "LCD ID:%04X", lcddev.id);  // ��LCD ID��ӡ��lcd_id����
	
	lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
	lcd_show_string(30, 70, 200, 16, 16, "USB Virtual USART TEST", RED);
	lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
	lcd_show_string(30, 110, 200, 16, 16, "USB Connecting...", RED);    /* ��ʾUSB��ʼ���� */


	USBD_Init(&USBD_Device, &VCP_Desc, 0);                      /* ��ʼ��USB */
	USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);           /* ����� */
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);   /* ΪMSC����ӻص����� */
	USBD_Start(&USBD_Device);                                  /* ����USB */
	delay_ms(1800);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//lcd test
		/*
		lcd_clear(colors[color_index]);  //����

		lcd_show_string(10, 40, 240, 32, 32, "STM32", RED);
		lcd_show_string(10, 80, 240, 24, 24, "TFTLCD TEST", RED);
		lcd_show_string(10, 110, 240, 16, 16, "ATOM@ALIENTEK", RED);
		lcd_show_string(10, 130, 240, 16, 16, (char *)lcd_id, RED); // ��ʾLCD ID 
		color_index++;

		if (color_index == 12){
				color_index = 0;
		}
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); // LED0��˸ 
		HAL_Delay(1000);
		*/
		delay_ms(1);
		char str[20] = {0};
		printf(str, "g_device_state:%d\n", g_device_state);
		if (usbstatus != g_device_state)    /* USB����״̬�����˸ı� */
		{
				usbstatus = g_device_state;     /* ��¼�µ�״̬ */

				if (usbstatus == 1){
						lcd_show_string(30, 110, 200, 16, 16, "USB Connected    ", RED); /* ��ʾUSB���ӳɹ� */
						HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); /* ��LED�� */
				}
				else{
						lcd_show_string(30, 110, 200, 16, 16, "USB disConnected ", RED); /* ��ʾUSB�Ͽ� */
						HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); /* ��LED�� */
				}
		}

		if (g_usb_usart_rx_sta & 0x8000){
				len = g_usb_usart_rx_sta & 0x3FFF;  /* �õ��˴ν��յ������ݳ��� */
				usb_printf("\r\n�����͵���Ϣ����Ϊ:%d\r\n\r\n", len);
				cdc_vcp_data_tx(g_usb_usart_rx_buffer, len);
				usb_printf("\r\n\r\n");             /* ���뻻�� */
				g_usb_usart_rx_sta = 0;
		}
		else{
				times++;
				if (times % 5000 == 0){
						usb_printf("\r\nSTM32������USB���⴮��ʵ��\r\n");
						usb_printf("����ԭ��@ALIENTEK\r\n\r\n");
				}
				if (times % 200 == 0)usb_printf("����������,�Իس�������\r\n");
				if (times % 30 == 0){
						HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); /* ��˸��LED,��ʾϵͳ�������� */
				}
				delay_ms(10);
		}

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
