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
#include "string.h"
#include "tim.h"
#include "string.h"
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
uint8_t u8TaskListBuffer[400];  //保存任务列表的缓冲区
extern uint32_t u32TimeCount;  //定时器统计值
uint8_t u8CmdBuffer[20];  //队列接收缓冲区
/* USER CODE END Variables */
osThreadId UsartTaskHandle;
uint32_t UsartTaskBuffer[ 128 ];
osStaticThreadDef_t UsartTaskControlBlock;
osThreadId Led0TaskHandle;
osThreadId KeyTaskHandle;
osThreadId DelayTaskHandle;
osMessageQId CmdQueueHandle;

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

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
	//1. 开启定时器6的中断——启动定时器
	//2. 定时器统计值，初始化为0
	HAL_TIM_Base_Stop_IT(&htim6);  //参数：TIM_HandleTypeDef* 类型的定时器句柄指针(在tim.c中)
	u32TimeCount = 0ul;
}

__weak unsigned long getRunTimeCounterValue(void)
{
	return u32TimeCount;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
	 printf("%s is Stack OverFlow!\r\n", pcTaskName);
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
//模拟栈溢出
void vStackOverFlowTest(void){
	uint8_t u8TestBuffer[128];
	int16_t i;
	(void)u8TestBuffer;
	for(i = 360; i >= 0; i--){
		u8TestBuffer[i] = 0x88;
	}
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

  /* Create the queue(s) */
  /* definition and creation of CmdQueue */
  osMessageQDef(CmdQueue, 20, uint8_t);
  CmdQueueHandle = osMessageCreate(osMessageQ(CmdQueue), NULL);

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
	uint8_t u8Index;
	uint8_t str_len;
	uint8_t pin_port;
  for(;;)
  {
		u8Index = 0;  //每次读取消息前，索引初始化0
		if(xQueueReceive(CmdQueueHandle, &u8CmdBuffer[u8Index++], portMAX_DELAY) == pdPASS){
			while(xQueueReceive(CmdQueueHandle, &u8CmdBuffer[u8Index++], 50)){}  //115200 8bit 数据包最大值是255Bytes 每秒可传输数据11520字 50ms则可以576字
			u8CmdBuffer[u8Index] = '\0';  //字符串结束标记
			printf("%s\n", u8CmdBuffer);
			//printf("strncmp((char*)u8CmdBuffer, openled, 7) -> %d\n", strncmp((char*)u8CmdBuffer, "openled", 7));
			//数据解析
			str_len = strlen((char*)u8CmdBuffer);  //获取命令长度
			//printf("str_len %d\n", str_len);
			if(strncmp((char*)u8CmdBuffer, "openled", 7) == 0){
				pin_port = u8CmdBuffer[str_len-1] - 48;  //ascii ‘0’- 0 = 48
				printf("pin_port %d\n", pin_port);
				if(pin_port == 0)	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);  // 点亮LED_{pin_port}
				else if(pin_port == 1)	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);  // 点亮LED_{pin_port}
				else printf("cmd is illegal!\n");
			}
			if(strncmp((char*)u8CmdBuffer, "closeled", 8) == 0){
				pin_port = u8CmdBuffer[str_len-1] - 48;  //ascii ‘0’- 0 = 48
				printf("pin_port %d\n", pin_port);
				if(pin_port == 0)	HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);  // 关闭LED_{pin_port}
				else if(pin_port == 1)	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);  // 关闭LED_{pin_port}
				else printf("cmd is illegal!\n");
			}
		}
    osDelay(5000);
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
		osDelay(50000);
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
			//vTaskSuspend(Led0TaskHandle);  //任务句柄在MX_FREERTOS_Init中
			memset(u8TaskListBuffer, 0, 400);
			taskENTER_CRITICAL();
			vTaskGetRunTimeStats((char*)u8TaskListBuffer);
			printf("Task            Abs Time      Time \r\n");
			printf("****************************************\r\n");
			printf("%s", u8TaskListBuffer);
			taskEXIT_CRITICAL();
			KeyStatus = KEY_RESET;
		}
		if(KEY_UP == KeyStatus){
			//恢复LED任务
			//vTaskResume(Led0TaskHandle);
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
    vTaskDelayUntil(&WakeTime, 5000);
  }
  /* USER CODE END Delay_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
