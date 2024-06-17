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
#include "event_groups.h"
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
EventGroupHandle_t KeyEventGroupHandle_t; //事件标志组
/* USER CODE END Variables */
osThreadId KeyxTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Keyx_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

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

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	//创建事件标志组
	KeyEventGroupHandle_t = xEventGroupCreate();
	/* Was the event group created successfully? */
	if( KeyEventGroupHandle_t == NULL )
	{
		/* The event group was not created because there was insufficient
		FreeRTOS heap available. */
		printf("xEventGroupCreate Failed !\n");
	}
	else
	{
		/* The event group was created. */
	}
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
  /* definition and creation of KeyxTask */
  osThreadDef(KeyxTask, Keyx_Task, osPriorityHigh, 0, 128);
  KeyxTaskHandle = osThreadCreate(osThread(KeyxTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Keyx_Task */
/**
  * @brief  Function implementing the KeyxTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Keyx_Task */
void Keyx_Task(void const * argument)
{
  /* USER CODE BEGIN Keyx_Task */
  /* Infinite loop */
	void* HeapBuf = NULL;
  for(;;)
  {
    //等待事件标志组置位
		EventBits_t x = xEventGroupWaitBits(
			KeyEventGroupHandle_t,  //句柄
			KEY0_EVENT_BIT|KEY1_EVENT_BIT|KEY2_EVENT_BIT,  //检测的位
			pdTRUE,  //退出是否清除标志
			pdFALSE,  //是否等待所有标志位
			portMAX_DELAY  //最大等待时间
		);
		//printf("KEYx is down!KeyEventGroupBits:%x\n", x);
		switch(x){
			case KEY0_EVENT_BIT:
				if(NULL == HeapBuf){
					HeapBuf = pvPortMalloc(100);
					printf("Malloc successed!\n");
				}else{
					printf("Press KEY1 vPortFree!\n");
				}
				break;
			case KEY1_EVENT_BIT:
				if(NULL != HeapBuf){
					vPortFree(HeapBuf);
					HeapBuf = NULL;
					printf("Free successed!\n");
				}else{
					printf("Already empty, Please press KEY0 to pvPortMalloc\n");
				}
				break;
			case KEY2_EVENT_BIT:
				printf("Heap free size is %d\n", xPortGetFreeHeapSize());
				break;
		}
    osDelay(100);
  }
  /* USER CODE END Keyx_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
