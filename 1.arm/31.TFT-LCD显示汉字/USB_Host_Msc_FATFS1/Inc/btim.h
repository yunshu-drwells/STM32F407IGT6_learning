#ifndef __BTIM_H
#define __BTIM_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"

/******************************************************************************************/
/* ������ʱ�� ���� */

/* TIMX �ж϶��� 
 * Ĭ�������TIM6/TIM7
 * ע��: ͨ���޸���4���궨��,����֧��TIM1~TIM8����һ����ʱ��.
 */
 
#define BTIM_TIMX_INT                       TIM3
#define BTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define BTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define BTIM_TIMX_INT_CLK_ENABLE()          do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM6 ʱ��ʹ�� */

/******************************************************************************************/

void btim_timx_int_init(uint16_t arr, uint16_t psc);    /* ������ʱ�� ��ʱ�жϳ�ʼ������ */

#endif
