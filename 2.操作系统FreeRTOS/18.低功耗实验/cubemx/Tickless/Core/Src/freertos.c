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
#define LED_NUM  2  //LED����

//���ڽ���buff
#define MESSAGE_BUFF_SIZE 50
//led����buff
#define LED_MESSAGE_BUFF_SIZE 20
//ʵʱʱ���ַ���ͷ
#define REALTIME "realtime"
//�����ַ���ͷ
#define ALARMTIME "alarmtime"
//msת��
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


//LED��Ϣ���У��ַ�����
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

//ʵʱʱ���ַ�����ֽ����ṹ��
struct sRealTimeString{
 uint8_t Hours[10];
 uint8_t Minutes[10]; 
 uint8_t Seconds[10]; 
 uint8_t Month[10]; 
 uint8_t Date[10];  
 uint8_t Year[10]; 

}RealTimeString;
//�����ַ�����ֽ����ṹ��
struct sAlarmTimeString{
 uint8_t Hours[10];
 uint8_t Minutes[10]; 
 uint8_t Seconds[10]; 
 uint8_t Mode[10]; 
 uint8_t Action[10];  

}AlarmTimeString;
//���Ӳ����ṹ��
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
//ģ��ջ���
void vStackOverFlowTest(void){
	uint8_t u8TestBuffer[128];
	int16_t i;
	(void)u8TestBuffer;
	for(i = 360; i >= 0; i--){
		u8TestBuffer[i] = 0x88;
	}
}


//���Ӵ����������ʱ�����ص�����
void vTimerCallback(xTimerHandle pxTimer){
  uint32_t ulTimerID;
  uint8_t i;
  //��ȡ��ǰ��ʱ��ID
  ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);
  //�ж϶�ʱ������ģʽ������Զ����أ�����������ʱ������
  if(((xTIMER*)pxTimer)->uxAutoReload){
      xTimerChangePeriodFromISR(pxTimer,DT0MS,NULL);
      printf("���������������������\r\n");
  }
  printf("ulTimerID = %d\r\n",ulTimerID);
  //���������ʱ��ID�ţ� ���͵�led��Ϣ������
  for(i=0;i<strlen((char const*)LedCmdString[ulTimerID]);i++){
    xQueueSend(LedQueueHandle,&LedCmdString[ulTimerID][i],1);
  }
}




//����������ʵʱʱ��֮ǰ�ļ��ʱ�䣬����ms
uint32_t CountAlarmInterval(sAlarmTime AlarmTime){
  int32_t AlarmTimeTick,RealTimeTick;
  RTCTimeDates RTCTimeDate;
  //��ȡʵʱʱ��
  RTCTimeDate = GetRTC();
  //��������ms����
  AlarmTimeTick = AlarmTime.Hours*HT0MS+AlarmTime.Minutes*MT0MS+AlarmTime.Seconds*ST0MS;
  //����ʵʱʱ��ms����
  RealTimeTick = RTCTimeDate.RtcTime.Hours*HT0MS+RTCTimeDate.RtcTime.Minutes*MT0MS+RTCTimeDate.RtcTime.Seconds*ST0MS;
  printf("AlarmTimeTick = %lu\r\n",(unsigned long)AlarmTimeTick);
  printf("RealTimeTick = %lu\r\n",(unsigned long)RealTimeTick);
  //�ж������Ƿ���ڵ��ڵ�ǰʵʱʱ��
  //����->��������-ʵʱʱ��
  if((AlarmTimeTick-RealTimeTick) >= 0){
  
      return AlarmTimeTick-RealTimeTick;
  }else{
    //С��->һ���msֵ+ʵʱʱ��-��������
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
    //��ȡ����ʱ���ַ���ָ��
    pbufftime = strstr((char const *)buff, ":");
    //��ȡ���Ӳ����ַ���ָ��
    pbuffparm = strstr((char const *)buff, ",");
    if (pbufftime != NULL)
    {
        //ָ���1 ȡ����ȷ��ͷָ��
        pbufftime++;
        //ȡ����ȷ��βָ��
        pbufftime = strtok(pbufftime, ",");
        //ȡ��Сʱ
        pbufftimeindex = strtok(pbufftime, ":");
        memcpy(AlarmTimeString.Hours, pbufftimeindex, strlen(pbufftimeindex));
        //ȡ������
        pbufftimeindex = strtok(NULL, ":");
        memcpy(AlarmTimeString.Minutes, pbufftimeindex, strlen(pbufftimeindex));
        //ȡ����
        pbufftimeindex = strtok(NULL, ":");
        memcpy(AlarmTimeString.Seconds, pbufftimeindex, strlen(pbufftimeindex));
    }
    if (pbuffparm != NULL)
    {
        //ָ���1 ȡ����ȷ��ͷָ��
        pbuffparm++;
        //ȡ������ģʽ
        pbuffparmindex = strtok(pbuffparm, ",");
        memcpy(AlarmTimeString.Mode, pbuffparmindex, strlen(pbuffparmindex));
        //ȡ��ִ�ж���
        pbuffparmindex = strtok(NULL, ",");
        memcpy(AlarmTimeString.Action, pbuffparmindex, strlen(pbuffparmindex));
    }
    printf("��������ϵͳʱ��Ϊ��%s:%s:%s\r\n",
           AlarmTimeString.Hours,
           AlarmTimeString.Minutes,
           AlarmTimeString.Seconds);    
    printf("�������ӹ���ģʽΪ��%s\r\n",
           AlarmTimeString.Mode);   
    printf("��������ִ�ж���Ϊ��%s\r\n",
           AlarmTimeString.Action); 
    //ת���ַ�����ʽ�����Ӳ���Ϊ����ֵ
    AlarmTime.Hours = atoi((char const *)AlarmTimeString.Hours);
    AlarmTime.Minutes = atoi((char const *)AlarmTimeString.Minutes);
    AlarmTime.Seconds = atoi((char const *)AlarmTimeString.Seconds);
    AlarmTime.Mode = atoi((char const *)AlarmTimeString.Mode);
    AlarmTime.Action = atoi((char const *)AlarmTimeString.Action);
    //�������ڼ��
    AlarmTick = CountAlarmInterval(AlarmTime);
    printf("��ǰ���Ӽ��Ϊ��%lu\r\n",(unsigned long)AlarmTick);
    //������ʱ����������������ģʽ����������
    xTimer = xTimerCreate("timer",AlarmTick,AlarmTime.Mode,(void*)AlarmTime.Action,vTimerCallback);
    //�ж϶�ʱ���Ƿ񴴽��ɹ�
    if(xTimer != NULL){
        //������ʱ��
        xTimerStart(xTimer,0);
        printf("������ʱ���ɹ���\r\n");
    }
}

//����ʵʱʱ���ַ���
void ParseRealTimeString(uint8_t *buff){
    char *pbuffdate;
    char *pbuffdateindex;
    char *pbufftime;
    char *pbufftimeindex;
	RTCTimeDates RTCTimeDate;
    //��ȡ�����ַ���ָ��
    pbuffdate = strstr((char const *)buff, ":");
    //��ȡʱ���ַ���ָ��
    pbufftime = strstr((char const *)buff, ",");
    if (pbuffdate != NULL)
    {
        //ָ���1 ȡ����ȷ��ͷָ��
        pbuffdate++;
        //ȡ����ȷ��βָ��
        pbuffdate = strtok(pbuffdate, ",");
        //ȡ����
        pbuffdateindex = strtok(pbuffdate, "-");
        memcpy(RealTimeString.Year, pbuffdateindex, strlen(pbuffdateindex));
        //ȡ����
        pbuffdateindex = strtok(NULL, "-");
        memcpy(RealTimeString.Month, pbuffdateindex, strlen(pbuffdateindex));
        //ȡ����
        pbuffdateindex = strtok(NULL, "-");
        memcpy(RealTimeString.Date, pbuffdateindex, strlen(pbuffdateindex));
    }
    if (pbufftime != NULL)
    {
        //ָ���1 ȡ����ȷ��ͷָ��
        pbufftime++;
        //ȡ��Сʱ
        pbufftimeindex = strtok(pbufftime, ":");
        memcpy(RealTimeString.Hours, pbufftimeindex, strlen(pbufftimeindex));
        //ȡ������
        pbufftimeindex = strtok(NULL, ":");
        memcpy(RealTimeString.Minutes, pbufftimeindex, strlen(pbufftimeindex));
        //ȡ����
        pbufftimeindex = strtok(NULL, ":");
        memcpy(RealTimeString.Seconds, pbufftimeindex, strlen(pbufftimeindex));
    }
    printf("���õ�ǰϵͳʱ��Ϊ��%s-%s-%s,%s:%s:%s\r\n",
           RealTimeString.Year,
           RealTimeString.Month,
           RealTimeString.Date,
           RealTimeString.Hours,
           RealTimeString.Minutes,
           RealTimeString.Seconds);
    //�ַ���ת��Ϊʵʱʱ��ֵ
    RTCTimeDate.RtcDate.Year = atoi((char const *)RealTimeString.Year) - 2000;
    RTCTimeDate.RtcDate.Month = atoi((char const *)RealTimeString.Month);
    RTCTimeDate.RtcDate.Date = atoi((char const *)RealTimeString.Date);
    RTCTimeDate.RtcTime.Hours = atoi((char const *)RealTimeString.Hours);
    RTCTimeDate.RtcTime.Minutes = atoi((char const *)RealTimeString.Minutes);
    RTCTimeDate.RtcTime.Seconds = atoi((char const *)RealTimeString.Seconds);
    //���õ�ǰʵʱʱ��
    SetRTC(&RTCTimeDate);
}


//�������������ַ���
void vCmdParseString(uint8_t *buff){
    //�ж��Ƿ�Ϊʵʱʱ������
    if(strncmp((char const*)buff,REALTIME,strlen(REALTIME)) == 0){
      ParseRealTimeString(buff);
    }
    //�ж��Ƿ�Ϊ��������
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
	//����ǰ
	printf("Before enter sleep mode! ulExpectedIdleTime = %u\n", ulExpectedIdleTime);
	HAL_SuspendTick();  //��������ΪSysTick�ļ�ʱ�������ǹرռ�ʱ���ж�
}

__weak void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
/* place for user code */
	//���ߺ�
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
		u8Index = 0;  //ÿ�ζ�ȡ��Ϣǰ��������ʼ��0
	  //1��һֱ�ȴ�������Ϣ,��һ����ϢӦ�÷�����Ϣ�������ĵ�һ��Ԫ����
		if(xQueueReceive(CmdQueueHandle,&u8CmdBuff[u8Index++],portMAX_DELAY)==pdPASS){
			while(xQueueReceive(CmdQueueHandle,&u8CmdBuff[u8Index++],50)){}
			u8CmdBuff[u8Index] = '\0';//��֤һ�������ַ�����Ϣ
			vCmdParseString(u8CmdBuff);
			//��ɽ����Ժ�Ҫ��ս��ջ���������Ȼ���������
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
    //ÿ�ζ�ȡ��Ϣ֮ǰ����������ʼ��Ϊ0
	  uint8_t u8Index = 0;
	  //1��һֱ�ȴ�������Ϣ,��һ����ϢӦ�÷�����Ϣ�������ĵ�һ��Ԫ����
		if(xQueueReceive(LedQueueHandle,&u8LedMessageBuff[u8Index++],portMAX_DELAY)==pdPASS){
			while(xQueueReceive(LedQueueHandle,&u8LedMessageBuff[u8Index++],50)){}
			u8LedMessageBuff[u8Index] = '\0';//��֤һ�������ַ�����Ϣ
			vLedParseString(u8LedMessageBuff);
			//��ɽ����Ժ�Ҫ��ս��ջ���������Ȼ���������
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
			//�ر�LED
			HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);	
		}
		if(KeyStatus == KEY_UP){
			//�ָ�LED����
	
			KeyStatus = KEY_RESET;
		}
		//ɨ������10ms
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
