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
#include "string.h"
#include "rtc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct tmrTimerControl
{
	const char				*pcTimerName;		/*<< Text name.  This is not used by the kernel, it is included simply to make debugging easier. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	ListItem_t				xTimerListItem;		/*<< Standard linked list item as used by all kernel features for event management. */
	TickType_t				xTimerPeriodInTicks;/*<< How quickly and often the timer expires. */
	UBaseType_t				uxAutoReload;		/*<< Set to pdTRUE if the timer should be automatically restarted once expired.  Set to pdFALSE if the timer is, in effect, a one-shot timer. */
	void 					*pvTimerID;			/*<< An ID to identify the timer.  This allows the timer to be identified when the same callback is used for multiple timers. */
	TimerCallbackFunction_t	pxCallbackFunction;	/*<< The function that will be called when the timer expires. */
	#if( configUSE_TRACE_FACILITY == 1 )
		UBaseType_t			uxTimerNumber;		/*<< An ID assigned by trace tools such as FreeRTOS+Trace */
	#endif

	#if( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
		uint8_t 			ucStaticallyAllocated; /*<< Set to pdTRUE if the timer was created statically so no attempt is made to free the memory again if the timer is later deleted. */
	#endif
} xTIMER;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_NUM  2  //LED数量

//串口接收buff
#define MESSAGE_BUFF_SIZE 50
//led接收buff
#define LED_MESSAGE_BUFF_SIZE 20
//实时时钟字符串头
#define REALTIME "realtime"
//闹钟字符串头
#define ALARMTIME "alarmtime"
//ms转换
#define ST0MS 1000ul
#define MT0MS (ST0MS*60)
#define HT0MS (MT0MS*60)
#define DT0MS (HT0MS*24)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern teKeyStatus KeyStatus;
uint32_t u32TimeCount;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint8_t u8CmdBuff[MESSAGE_BUFF_SIZE];
static uint8_t u8LedMessageBuff[LED_MESSAGE_BUFF_SIZE];


//LED消息队列，字符串集
uint8_t *LedCmdString[LED_NUM*2] = {
	"openled0",
	"openled1",
	"closeled0",
	"closeled1"
};

uint8_t *OpenString[LED_NUM] = {
	"openled0",
	"openled1"
};

uint8_t *CloseString[LED_NUM] = {
	"closeled0",
	"closeled1"
};

GPIO_TypeDef * LedPort[LED_NUM] = {
	LED0_GPIO_Port,
	LED1_GPIO_Port
};

uint16_t LedPin[LED_NUM] ={
	LED0_Pin,
	LED1_Pin
};

//实时时钟字符串拆分解析结构体
struct sRealTimeString{
 uint8_t Hours[10];
 uint8_t Minutes[10]; 
 uint8_t Seconds[10]; 
 uint8_t Month[10]; 
 uint8_t Date[10];  
 uint8_t Year[10]; 

}RealTimeString;
//闹钟字符串拆分解析结构体
struct sAlarmTimeString{
 uint8_t Hours[10];
 uint8_t Minutes[10]; 
 uint8_t Seconds[10]; 
 uint8_t Mode[10]; 
 uint8_t Action[10];  

}AlarmTimeString;
//闹钟参数结构体
typedef struct{
 uint8_t Hours;
 uint8_t Minutes; 
 uint8_t Seconds; 
 uint8_t Mode; 
 uint8_t Action;  

} sAlarmTime;
/* USER CODE END Variables */
osThreadId UsartTaskHandle;
uint32_t UsartTaskBuffer[ 128 ];
osStaticThreadDef_t UsartTaskControlBlock;
osThreadId LedxTaskHandle;
osThreadId Key0TaskHandle;
osThreadId DelayTaskHandle;
osMessageQId CmdQueueHandle;
osMessageQId LedQueueHandle;
osTimerId RtcTimerHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Usart_Task(void const * argument);
void Ledx_Task(void const * argument);
void Key0_Task(void const * argument);
void Delay_Task(void const * argument);
void Rtc_Timer(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

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


//闹钟触发的软件定时器，回调函数
void vTimerCallback(xTimerHandle pxTimer){
  uint32_t ulTimerID;
  uint8_t i;
  //获取当前定时器ID
  ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
  //判断定时器工作模式，如果自动重载，则更新软件定时器周期
  if(((xTIMER*)pxTimer)->uxAutoReload){
      xTimerChangePeriodFromISR(pxTimer,DT0MS,NULL);
      printf("明天继续触发动作！！！\r\n");
  }
  printf("ulTimerID = %d\r\n",ulTimerID);
  //根据软件定时器ID号， 发送到led消息队列中
  for(i=0;i<strlen((char const*)LedCmdString[ulTimerID]);i++){
    xQueueSend(LedQueueHandle,&LedCmdString[ulTimerID][i],1);
  }
}




//计算闹钟与实时时钟之前的间隔时间，返回ms
uint32_t CountAlarmInterval(sAlarmTime AlarmTime){
  int32_t AlarmTimeTick,RealTimeTick;
  RTCTimeDates RTCTimeDate;
  //获取实时时钟
  RTCTimeDate = GetRTC();
  //计算闹钟ms计数
  AlarmTimeTick = AlarmTime.Hours*HT0MS+AlarmTime.Minutes*MT0MS+AlarmTime.Seconds*ST0MS;
  //计算实时时钟ms计数
  RealTimeTick = RTCTimeDate.RtcTime.Hours*HT0MS+RTCTimeDate.RtcTime.Minutes*MT0MS+RTCTimeDate.RtcTime.Seconds*ST0MS;
  printf("AlarmTimeTick = %lu\r\n",(unsigned long)AlarmTimeTick);
  printf("RealTimeTick = %lu\r\n",(unsigned long)RealTimeTick);
  //判断闹钟是否大于等于当前实时时钟
  //大于->返回闹钟-实时时钟
  if((AlarmTimeTick-RealTimeTick) >= 0){
  
      return AlarmTimeTick-RealTimeTick;
  }else{
    //小于->一天的ms值+实时时钟-返回闹钟
    return DT0MS+RealTimeTick-AlarmTimeTick;
  }
}


void ParseAlarmTimeString(uint8_t *buff){
	char *pbufftime;
	char *pbufftimeindex;
	char *pbuffparm;
	char *pbuffparmindex;
	uint32_t AlarmTick;
	TimerHandle_t xTimer;
	sAlarmTime AlarmTime;
	void SetRTC(RTCTimeDates *pRTCTimeDate);
    //获取闹钟时间字符串指针
    pbufftime = strstr((char const *)buff, ":");
    //获取闹钟参数字符串指针
    pbuffparm = strstr((char const *)buff, ",");
    if (pbufftime != NULL)
    {
        //指针加1 取出正确的头指针
        pbufftime++;
        //取出正确的尾指针
        pbufftime = strtok(pbufftime, ",");
        //取出小时
        pbufftimeindex = strtok(pbufftime, ":");
        memcpy(AlarmTimeString.Hours, pbufftimeindex, strlen(pbufftimeindex));
        //取出分钟
        pbufftimeindex = strtok(NULL, ":");
        memcpy(AlarmTimeString.Minutes, pbufftimeindex, strlen(pbufftimeindex));
        //取出秒
        pbufftimeindex = strtok(NULL, ":");
        memcpy(AlarmTimeString.Seconds, pbufftimeindex, strlen(pbufftimeindex));
    }
    if (pbuffparm != NULL)
    {
        //指针加1 取出正确的头指针
        pbuffparm++;
        //取出工作模式
        pbuffparmindex = strtok(pbuffparm, ",");
        memcpy(AlarmTimeString.Mode, pbuffparmindex, strlen(pbuffparmindex));
        //取出执行动作
        pbuffparmindex = strtok(NULL, ",");
        memcpy(AlarmTimeString.Action, pbuffparmindex, strlen(pbuffparmindex));
    }
    printf("设置闹钟系统时间为：%s:%s:%s\r\n",
           AlarmTimeString.Hours,
           AlarmTimeString.Minutes,
           AlarmTimeString.Seconds);    
    printf("设置闹钟工作模式为：%s\r\n",
           AlarmTimeString.Mode);   
    printf("设置闹钟执行动作为：%s\r\n",
           AlarmTimeString.Action); 
    //转换字符串格式的闹钟参数为整型值
    AlarmTime.Hours = atoi((char const *)AlarmTimeString.Hours);
    AlarmTime.Minutes = atoi((char const *)AlarmTimeString.Minutes);
    AlarmTime.Seconds = atoi((char const *)AlarmTimeString.Seconds);
    AlarmTime.Mode = atoi((char const *)AlarmTimeString.Mode);
    AlarmTime.Action = atoi((char const *)AlarmTimeString.Action);
    //计数周期间隔
    AlarmTick = CountAlarmInterval(AlarmTime);
    printf("当前闹钟间隔为：%lu\r\n",(unsigned long)AlarmTick);
    //创建定时器，传入间隔、工作模式、触发动作
    xTimer = xTimerCreate("timer",AlarmTick,AlarmTime.Mode,(void*)AlarmTime.Action,vTimerCallback);
    //判断定时器是否创建成功
    if(xTimer != NULL){
        //启动定时器
        xTimerStart(xTimer,0);
        printf("启动定时器成功！\r\n");
    }
}

//解析实时时钟字符串
void ParseRealTimeString(uint8_t *buff){
    char *pbuffdate;
    char *pbuffdateindex;
    char *pbufftime;
    char *pbufftimeindex;
	RTCTimeDates RTCTimeDate;
    //获取日期字符串指针
    pbuffdate = strstr((char const *)buff, ":");
    //获取时间字符串指针
    pbufftime = strstr((char const *)buff, ",");
    if (pbuffdate != NULL)
    {
        //指针加1 取出正确的头指针
        pbuffdate++;
        //取出正确的尾指针
        pbuffdate = strtok(pbuffdate, ",");
        //取出年
        pbuffdateindex = strtok(pbuffdate, "-");
        memcpy(RealTimeString.Year, pbuffdateindex, strlen(pbuffdateindex));
        //取出月
        pbuffdateindex = strtok(NULL, "-");
        memcpy(RealTimeString.Month, pbuffdateindex, strlen(pbuffdateindex));
        //取出天
        pbuffdateindex = strtok(NULL, "-");
        memcpy(RealTimeString.Date, pbuffdateindex, strlen(pbuffdateindex));
    }
    if (pbufftime != NULL)
    {
        //指针加1 取出正确的头指针
        pbufftime++;
        //取出小时
        pbufftimeindex = strtok(pbufftime, ":");
        memcpy(RealTimeString.Hours, pbufftimeindex, strlen(pbufftimeindex));
        //取出分钟
        pbufftimeindex = strtok(NULL, ":");
        memcpy(RealTimeString.Minutes, pbufftimeindex, strlen(pbufftimeindex));
        //取出秒
        pbufftimeindex = strtok(NULL, ":");
        memcpy(RealTimeString.Seconds, pbufftimeindex, strlen(pbufftimeindex));
    }
    printf("设置当前系统时间为：%s-%s-%s,%s:%s:%s\r\n",
           RealTimeString.Year,
           RealTimeString.Month,
           RealTimeString.Date,
           RealTimeString.Hours,
           RealTimeString.Minutes,
           RealTimeString.Seconds);
    //字符串转换为实时时钟值
    RTCTimeDate.RtcDate.Year = atoi((char const *)RealTimeString.Year) - 2000;
    RTCTimeDate.RtcDate.Month = atoi((char const *)RealTimeString.Month);
    RTCTimeDate.RtcDate.Date = atoi((char const *)RealTimeString.Date);
    RTCTimeDate.RtcTime.Hours = atoi((char const *)RealTimeString.Hours);
    RTCTimeDate.RtcTime.Minutes = atoi((char const *)RealTimeString.Minutes);
    RTCTimeDate.RtcTime.Seconds = atoi((char const *)RealTimeString.Seconds);
    //设置当前实时时钟
    SetRTC(&RTCTimeDate);
}


//解析串口命令字符串
void vCmdParseString(uint8_t *buff){
    //判断是否为实时时钟设置
    if(strncmp((char const*)buff,REALTIME,strlen(REALTIME)) == 0){
      ParseRealTimeString(buff);
    }
    //判断是否为闹钟设置
    else if(strncmp((char const*)buff,ALARMTIME,strlen(ALARMTIME)) == 0){  
      ParseAlarmTimeString(buff);
    }
}


void vLedParseString(uint8_t *buff){
    uint8_t i;
    for(i=0;i<LED_NUM;i++){
      if(strcmp((char const*)buff,(char const*)OpenString[i]) == 0){
        HAL_GPIO_WritePin(LedPort[i], LedPin[i], GPIO_PIN_RESET);
        printf("Cmd is %s\n",OpenString[i]);
				return;      
      }  
	}
    for(i=0;i<LED_NUM;i++){
      if(strcmp((char const*)buff,(char const*)CloseString[i]) == 0){
				HAL_GPIO_WritePin(LedPort[i], LedPin[i], GPIO_PIN_SET);
				printf("Cmd is %s\n",CloseString[i]);
				return;
      }	    
    }
}
/* USER CODE END 4 */

/* USER CODE BEGIN PREPOSTSLEEP */
__weak void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
/* place for user code */
	//休眠前
	printf("Before enter sleep mode! ulExpectedIdleTime = %u\n", ulExpectedIdleTime);
	HAL_SuspendTick();  //挂起设置为SysTick的计时器，就是关闭计时器中断
}

__weak void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
/* place for user code */
	//休眠后
	printf("Already enter sleep mode! ulExpectedIdleTime = %u\n", ulExpectedIdleTime);
	//HAL_ResumeTick();
}
/* USER CODE END PREPOSTSLEEP */

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

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of RtcTimer */
  osTimerDef(RtcTimer, Rtc_Timer);
  RtcTimerHandle = osTimerCreate(osTimer(RtcTimer), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of CmdQueue */
  osMessageQDef(CmdQueue, 50, uint8_t);
  CmdQueueHandle = osMessageCreate(osMessageQ(CmdQueue), NULL);

  /* definition and creation of LedQueue */
  osMessageQDef(LedQueue, 20, uint8_t);
  LedQueueHandle = osMessageCreate(osMessageQ(LedQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of UsartTask */
  osThreadStaticDef(UsartTask, Usart_Task, osPriorityLow, 0, 128, UsartTaskBuffer, &UsartTaskControlBlock);
  UsartTaskHandle = osThreadCreate(osThread(UsartTask), NULL);

  /* definition and creation of LedxTask */
  osThreadDef(LedxTask, Ledx_Task, osPriorityNormal, 0, 128);
  LedxTaskHandle = osThreadCreate(osThread(LedxTask), NULL);

  /* definition and creation of Key0Task */
  osThreadDef(Key0Task, Key0_Task, osPriorityNormal, 0, 128);
  Key0TaskHandle = osThreadCreate(osThread(Key0Task), NULL);

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
  for(;;)
  {
		u8Index = 0;  //每次读取消息前，索引初始化0
	  //1、一直等待接收消息,第一个消息应该放在消息缓冲区的第一个元素上
		if(xQueueReceive(CmdQueueHandle,&u8CmdBuff[u8Index++],portMAX_DELAY)==pdPASS){
			while(xQueueReceive(CmdQueueHandle,&u8CmdBuff[u8Index++],50)){}
			u8CmdBuff[u8Index] = '\0';//保证一包完整字符串信息
			vCmdParseString(u8CmdBuff);
			//完成解析以后，要清空接收缓冲区，不然会出现问题
		  memset(u8CmdBuff,0,MESSAGE_BUFF_SIZE);
		}
    osDelay(5000);
  }
  /* USER CODE END Usart_Task */
}

/* USER CODE BEGIN Header_Ledx_Task */
/**
* @brief Function implementing the Led0Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Ledx_Task */
void Ledx_Task(void const * argument)
{
  /* USER CODE BEGIN Ledx_Task */
  /* Infinite loop */
	osTimerStart(RtcTimerHandle,1000);
  for(;;)
  {
    //每次读取消息之前，把索引初始化为0
	  uint8_t u8Index = 0;
	  //1、一直等待接收消息,第一个消息应该放在消息缓冲区的第一个元素上
		if(xQueueReceive(LedQueueHandle,&u8LedMessageBuff[u8Index++],portMAX_DELAY)==pdPASS){
			while(xQueueReceive(LedQueueHandle,&u8LedMessageBuff[u8Index++],50)){}
			u8LedMessageBuff[u8Index] = '\0';//保证一包完整字符串信息
			vLedParseString(u8LedMessageBuff);
			//完成解析以后，要清空接收缓冲区，不然会出现问题
		  memset(u8LedMessageBuff,0,MESSAGE_BUFF_SIZE);
		}	
  }
  /* USER CODE END Ledx_Task */
}

/* USER CODE BEGIN Header_Key0_Task */
/**
* @brief Function implementing the Key0Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Key0_Task */
void Key0_Task(void const * argument)
{
  /* USER CODE BEGIN Key0_Task */
  /* Infinite loop */
	KeyStatus = KEY_RESET;
  for(;;)
  {
    if(KeyStatus == KEY_DOWN){
			//关闭LED
			HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);	
		}
		if(KeyStatus == KEY_UP){
			//恢复LED任务
	
			KeyStatus = KEY_RESET;
		}
		//扫描周期10ms
			osDelay(10);
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
  for(;;)
  {
    osDelay(500);
  }
  /* USER CODE END Delay_Task */
}

/* Rtc_Timer function */
void Rtc_Timer(void const * argument)
{
  /* USER CODE BEGIN Rtc_Timer */
	GetRTC();
  /* USER CODE END Rtc_Timer */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
