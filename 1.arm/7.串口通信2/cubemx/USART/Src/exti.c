#include "exti.h"
int fputc(int ch, FILE *p)
{
		while(!(USART1->SR & (1<<7)));  //�ȴ����ͼĴ�����
		USART1->DR = ch;
		return ch;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
		if(huart->Instance == USART1)
		{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);/*����LED0*/
				HAL_Delay(50);  /*SysTick�ж����ȼ�����UART*/
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);/*Ϩ��LED0*/
				printf("send finish\n");
				
		}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		if(huart->Instance == USART1)
		{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);/*����LED1*/
				HAL_Delay(50);  /*SysTick�ж����ȼ�����UART*/
				printf("data: st[0]:%x st[1]:%x\n", st[0], st[1]);
				HAL_UART_Receive_IT(huart, st, 2);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);/*Ϩ��LED1*/
				printf("receive finish\n");
		}
}
