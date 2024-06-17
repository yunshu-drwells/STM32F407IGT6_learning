/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "stdio.h"
#include "string.h"
#include "gpio.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern teKeyStatus KStatus;
extern uint32_t timecount;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId USARTHandle;
uint32_t USARTBuffer[ 128 ];
osStaticThreadDef_t USARTControlBlock;
osThreadId LEDHandle;
osThreadId KEYHandle;
osThreadId DelayHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void USART_Task(void const * argument);
void LED_Task(void const * argument);
void KEY_SusRes(void const * argument);
void DelayTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
	HAL_TIM_Base_Start_IT(&htim6);  //Turn on the interrupt of the timer
	timecount = 0ul;                //Initialized to an unsigned long integer
}

__weak unsigned long getRunTimeCounterValue(void)
{
return timecount;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */

}
/* USER CODE END 4 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of USART */
  osThreadStaticDef(USART, USART_Task, osPriorityLow, 0, 128, USARTBuffer, &USARTControlBlock);
  USARTHandle = osThreadCreate(osThread(USART), NULL);

  /* definition and creation of LED */
  osThreadDef(LED, LED_Task, osPriorityIdle, 0, 128);
  LEDHandle = osThreadCreate(osThread(LED), NULL);

  /* definition and creation of KEY */
  osThreadDef(KEY, KEY_SusRes, osPriorityNormal, 0, 128);
  KEYHandle = osThreadCreate(osThread(KEY), NULL);

  /* definition and creation of Delay */
  osThreadDef(Delay, DelayTask, osPriorityLow, 0, 128);
  DelayHandle = osThreadCreate(osThread(Delay), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_USART_Task */
/**
  * @brief  Function implementing the USART thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_USART_Task */
void USART_Task(void const * argument)
{
    
    
    
    
    

  /* USER CODE BEGIN USART_Task */
  /* Infinite loop */
  for(;;)
  {//Relative time delay
		//printf("USART is running!\n");
		osDelay(1000);
  }
  /* USER CODE END USART_Task */
}

/* USER CODE BEGIN Header_LED_Task */
/**
* @brief Function implementing the LED thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LED_Task */
void LED_Task(void const * argument)
{
  /* USER CODE BEGIN LED_Task */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
    osDelay(500);
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
    osDelay(500);
  }
  /* USER CODE END LED_Task */
}

/* USER CODE BEGIN Header_KEY_SusRes */
uint8_t TlBuff[400];
/**
* @brief Function implementing the KEY thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KEY_SusRes */
void KEY_SusRes(void const * argument)
{
  /* USER CODE BEGIN KEY_SusRes */
	KStatus = KEY_RESET;
	//printf("fuck\n");
  /* Infinite loop */
  for(;;)
  {//The suspend and resume of the LED is realized according to the key flag bit
    if(KStatus == KEY_DOWN)
		{
			memset(TlBuff, 0, 400);
			vTaskGetRunTimeStats((char *)TlBuff);
			printf("name,   abs time   time\n");
			printf("-------------------------------------------------------\n");
			printf("%s\n",TlBuff);
			printf("timecount%d\n", timecount);
			KStatus = KEY_RESET;//Flag bit reset
		}
		if(KStatus == KEY_UP)
		{
			KStatus = KEY_RESET;
		}
		osDelay(1000);
  }
  /* USER CODE END KEY_SusRes */
}

/* USER CODE BEGIN Header_DelayTask */
/**
* @brief Function implementing the Delay thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DelayTask */
void DelayTask(void const * argument)
{
  /* USER CODE BEGIN DelayTask */
	TickType_t WakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
		taskENTER_CRITICAL();
		//printf("DelayTask is running!\n");
		taskEXIT_CRITICAL();
		vTaskDelayUntil(&WakeTime, 1000);//Absolute time delay
  }
  /* USER CODE END DelayTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
