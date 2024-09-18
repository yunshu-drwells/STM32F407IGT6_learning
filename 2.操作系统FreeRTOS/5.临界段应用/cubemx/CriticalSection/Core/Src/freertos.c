/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
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
/* USER CODE BEGIN Variables */
extern teKeyStatus KeyStatus;
/* USER CODE END Variables */
osThreadId UsartTaskHandle;
uint32_t UsartTaskBuffer[ 128 ];
osStaticThreadDef_t UsartTaskControlBlock;
osThreadId Led0TaskHandle;
osThreadId KeyTaskHandle;
osThreadId DelayTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Usart_Task(void const * argument);
void Led0_Task(void const * argument);
void Key0_Task(void const * argument);
void Delay_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

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
  /* definition and creation of UsartTask */
  osThreadStaticDef(UsartTask, Usart_Task, osPriorityLow, 0, 128, UsartTaskBuffer, &UsartTaskControlBlock);
  UsartTaskHandle = osThreadCreate(osThread(UsartTask), NULL);

  /* definition and creation of Led0Task */
  osThreadDef(Led0Task, Led0_Task, osPriorityNormal, 0, 128);
  Led0TaskHandle = osThreadCreate(osThread(Led0Task), NULL);

  /* definition and creation of KeyTask */
  osThreadDef(KeyTask, Key0_Task, osPriorityNormal, 0, 128);
  KeyTaskHandle = osThreadCreate(osThread(KeyTask), NULL);

  /* definition and creation of DelayTask */
  osThreadDef(DelayTask, Delay_Task, osPriorityLow, 0, 128);
  DelayTaskHandle = osThreadCreate(osThread(DelayTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Usart_Task */
/**
  * @brief  Function implementing the UsartTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Usart_Task */
void Usart_Task(void const * argument)
{
  /* USER CODE BEGIN Usart_Task */
  /* Infinite loop */
  for(;;)
  {
		taskENTER_CRITICAL();
		printf("Usart test is running!\r\n");
		taskEXIT_CRITICAL();
    osDelay(50);
  }
  /* USER CODE END Usart_Task */
}

/* USER CODE BEGIN Header_Led0_Task */
/**
* @brief Function implementing the Led0Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Led0_Task */
void Led0_Task(void const * argument)
{
  /* USER CODE BEGIN Led0_Task */
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);  // 点亮LED0
		osDelay(500);
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);  // 熄灭LED0
		osDelay(500);
  }
  /* USER CODE END Led0_Task */
}

/* USER CODE BEGIN Header_Key0_Task */
/**
* @brief Function implementing the KeyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Key0_Task */
void Key0_Task(void const * argument)
{
  /* USER CODE BEGIN Key0_Task */
  /* Infinite loop */
	KeyStatus = KEY_RESET;
  /* Infinite loop */
  for(;;)
  {
		if(KEY_DOWN == KeyStatus){
			//挂起LED任务
			vTaskSuspend(Led0TaskHandle);  //任务句柄在MX_FREERTOS_Init中
			KeyStatus = KEY_RESET;
		}
		if(KEY_UP == KeyStatus){
			//恢复LED任务
			vTaskResume(Led0TaskHandle);
			KeyStatus = KEY_RESET;
		}
    osDelay(10); //扫描周期:10ms
  }
  /* USER CODE END Key0_Task */
}

/* USER CODE BEGIN Header_Delay_Task */
/**
* @brief Function implementing the DelayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Delay_Task */
void Delay_Task(void const * argument)
{
  /* USER CODE BEGIN Delay_Task */
  /* Infinite loop */
	TickType_t WakeTime = xTaskGetTickCount();  //系统上一次节拍值 
  for(;;)
  {
		taskENTER_CRITICAL();
		printf("Delay task is running!\r\n");
		taskEXIT_CRITICAL();
    vTaskDelayUntil(&WakeTime, 50);
  }
  /* USER CODE END Delay_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
