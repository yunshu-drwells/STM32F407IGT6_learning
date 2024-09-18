#ifndef __EXTI_H
#define __EXTI_H


/******************************************************************************************/
/* 引脚 和 中断编号 & 中断服务函数 定义 */ 

#define KEY0_INT_IRQn                   EXTI2_IRQn


#define KEY1_INT_IRQn                   EXTI3_IRQn


#define KEY2_INT_IRQn                   EXTI4_IRQn


/******************************************************************************************/


void extix_init(void);  /* 外部中断初始化 */

#endif
