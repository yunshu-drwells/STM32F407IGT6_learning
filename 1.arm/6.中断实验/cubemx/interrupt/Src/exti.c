#include "exti.h"
#include "key.h"
#include "led.h"

/**
 * @brief       KEY0 �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void KEY0_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_PIN);         /* �����жϴ����ú��� ���KEY0�����ж��� ���жϱ�־λ */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_PIN);         /* HAL��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}

/**
 * @brief       KEY1 �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void KEY1_INT_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY1_PIN);         /* �����жϴ����ú��� ���KEY1�����ж��� ���жϱ�־λ���ж��°벿��HAL_GPIO_EXTI_Callbackִ�� */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_PIN);         /* HAL��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}

/**
 * @brief       KEY2 �ⲿ�жϷ������
 * @param       ��
 * @retval      ��
 */
void KEY2_INT_IRQHandler(void)
{ 
    HAL_GPIO_EXTI_IRQHandler(KEY2_PIN);        /* �����жϴ����ú��� ���KEY2�����ж��� ���жϱ�־λ���ж��°벿��HAL_GPIO_EXTI_Callbackִ�� */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY2_PIN);        /* HAL��Ĭ�������ж��ٴ���ص����˳�ʱ����һ���жϣ����ⰴ�������󴥷� */
}

/**
 * @brief       �жϷ����������Ҫ��������
                ��HAL�������е��ⲿ�жϷ�����������ô˺���
 * @param       GPIO_Pin:�ж����ź�
 * @retval      ��
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_Delay(10);      /* ���� */

		switch(GPIO_Pin){
			case KEY0_PIN:
				/*key0*/
				if(0 == KEY0){
					LEDx_TogglePin(GPIO_GROUP, LED0_PIN);
				}
				break;
			case KEY1_PIN:
				/*key1*/
				if(0 == KEY1){
					LEDx_TogglePin(GPIO_GROUP, LED1_PIN);					
				}
				break;
			case KEY2_PIN:
				/*key2*/
				if(0 == KEY2){
					LEDx_TogglePin(GPIO_GROUP, LED0_PIN);
					LEDx_TogglePin(GPIO_GROUP, LED1_PIN);
				}
				break;
		}
}

/**
 * @brief       �ⲿ�жϳ�ʼ������
 * @param       ��
 * @retval      ��
 */
void extix_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
		/*GPIO��ʼ����MX_GPIO_Init�������Ѿ���ɣ�������Ҫ�����жϷ�ʽ*/
    gpio_init_struct.Pin = KEY0_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* �½��ش��� */
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIO_GROUP, &gpio_init_struct);    /* KEY0����Ϊ�½��ش����ж� */

    gpio_init_struct.Pin = KEY1_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* �½��ش��� */
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIO_GROUP, &gpio_init_struct);    /* KEY1����Ϊ�½��ش����ж� */
    
    gpio_init_struct.Pin = KEY2_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* �½��ش��� */
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIO_GROUP, &gpio_init_struct);    /* KEY2����Ϊ�½��ش����ж� */
    

    HAL_NVIC_SetPriority(KEY0_INT_IRQn, 0, 2);               /* ��ռ0�������ȼ�2 */
    HAL_NVIC_EnableIRQ(KEY0_INT_IRQn);                       /* ʹ���ж���1 */

    HAL_NVIC_SetPriority(KEY1_INT_IRQn, 1, 2);               /* ��ռ1�������ȼ�2 */
    HAL_NVIC_EnableIRQ(KEY1_INT_IRQn);                       /* ʹ���ж���15 */
    
    HAL_NVIC_SetPriority(KEY2_INT_IRQn, 2, 2);               /* ��ռ2�������ȼ�2 */
    HAL_NVIC_EnableIRQ(KEY2_INT_IRQn);                       /* ʹ���ж���15 */

}

