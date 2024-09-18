#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"


int main(void)
{
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(336, 8, 2, 7);         /* 设置时钟,168Mhz */
    delay_init(168);                            /* 延时初始化 */
    led_init();                                 /* 初始化LED */
    
    while(1)
    {
        LED0(0);                                /* LED0 亮 */
        LED1(1);                                /* LED1 灭 */
        delay_ms(500);
        LED0(1);                                /* LED0 灭 */
        LED1(0);                                /* LED1 亮 */
        delay_ms(500);
    }
}

