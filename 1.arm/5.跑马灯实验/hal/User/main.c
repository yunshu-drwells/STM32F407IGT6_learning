#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"


int main(void)
{
    HAL_Init();                                 /* ��ʼ��HAL�� */
    sys_stm32_clock_init(336, 8, 2, 7);         /* ����ʱ��,168Mhz */
    delay_init(168);                            /* ��ʱ��ʼ�� */
    led_init();                                 /* ��ʼ��LED */
    
    while(1)
    {
        LED0(0);                                /* LED0 �� */
        LED1(1);                                /* LED1 �� */
        delay_ms(500);
        LED0(1);                                /* LED0 �� */
        LED1(0);                                /* LED1 �� */
        delay_ms(500);
    }
}

