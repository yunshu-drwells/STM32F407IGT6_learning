/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "malloc.h"
#include "usart.h"
#include "lwip_comm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/*
uint8_t *p = 0;
uint8_t sramx = 0;  // 默认为内部sram 
uint8_t i = 0;
uint8_t paddr[20];  // 存放P Addr:+p地址的ASCII值 
uint16_t memused = 0;  //内存使用率
const char *SRAM_NAME_BUF[SRAMBANK] = {" SRAMIN ", " SRAMCCM "};  //状态
*/
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

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
  /* USER CODE BEGIN 2 */
	my_mem_init(SRAMIN);                /* 初始化内部SRAM内存池 */
  my_mem_init(SRAMCCM);               /* 初始化内部SRAMCCM内存池 */

	printf("lwIP Init !!\n");
	
	while (lwip_comm_init() != 0)
	{
			printf("lwIP Init failed!!\n");
			HAL_Delay(500);
			printf("Retrying...       \n");
			HAL_Delay(500);
	}

	while (!ethernet_read_phy(PHY_SR))  /* 检查MCU与PHY芯片是否通信成功 */
	{
			printf("MCU与PHY芯片通信失败，请检查电路或者源码！！！！\r\n");
	}
	
	
#if LWIP_DHCP
    printf("DHCP IP configing... \n");    /* 开始DHCP */
    
    while ((g_lwipdev.dhcpstatus != 2) && (g_lwipdev.dhcpstatus != 0XFF))   /* 等待DHCP获取成功/超时溢出 */
    {
        lwip_periodic_handle();
    }
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		lwip_periodic_handle();         /* LWIP轮询任务 */
		HAL_Delay(2);
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*
//按键中断函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  //按键中断处理函数
{
    if(GPIO_Pin == KEY0_Pin)
    {
      //KEY0是否按下
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)){
				HAL_Delay(20);  //去抖动
				if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin)){
					printf("KEY0 press,start malloc 2KB mem! \n"); // 申请内存，每次申请2K字节内存
					p = mymalloc(sramx, 2048);  // 申请2K字节,并写入内容
					if (p != NULL){
							sprintf((char *)p, "Memory Malloc Test%03d", i);         // 向p写入一些内容 
							printf("malloced mem is set as:%s\n", (char *)p);  // 显示P的内容 
					}
					if(0 == sramx){  //SRAM
						memused = my_mem_perused(SRAMIN);
						sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
						printf("%s\n", (char *)paddr);   // 显示内部内存使用率 
					}else{  //CCM
						memused = my_mem_perused(SRAMCCM);
						sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
						printf("%s\n",  (char *)paddr);   // 显示TCM内存使用率 
					}
				}
			}
    }
		if(GPIO_Pin == KEY1_Pin)
    {
      //KEY0是否按下
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)){
				HAL_Delay(20);  //去抖动
				if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)){
					printf("KEY1 press,free mem!\n");  // 释放内存
					myfree(sramx, p);
				}
				if(0 == sramx){  //SRAM
					memused = my_mem_perused(SRAMIN);
					sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
					printf("%s\n", (char *)paddr);   // 显示内部内存使用率 
				}else{  //CCM
					memused = my_mem_perused(SRAMCCM);
					sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
					printf("%s\n",  (char *)paddr);   // 显示TCM内存使用率 
				}
			}
    }
		if(GPIO_Pin == KEY2_Pin)
    {
      //KEY0是否按下
			if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)){
				HAL_Delay(20);  //去抖动
				if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)){
					printf("KEY1 press,switch SRAMIN/SRAMCCM!\n");  // 切换操作内存区（ SRAMIN/SRAMCCM，总共管理2个内存块）
					sramx = sramx == 0?1:0;
          printf("sramx:%d %s\n", sramx, (char *)SRAM_NAME_BUF[sramx]);					
				}
			}
    }
		i++;
}
*/
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
