#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"



#include "stm32f4xx_hal_can.h"
#include "stm32f4xx_hal_def.h"

#ifndef __CAN_H
#define __CAN_H


/******************************************************************************************/
/* CAN ���� ���� */

#define CAN_RX_GPIO_PORT                GPIOI
#define CAN_RX_GPIO_PIN                 GPIO_PIN_9
#define CAN_RX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)     /* PI��ʱ��ʹ�� */

#define CAN_TX_GPIO_PORT                GPIOB
#define CAN_TX_GPIO_PIN                 GPIO_PIN_9
#define CAN_TX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* PB��ʱ��ʹ�� */

/******************************************************************************************/

/* CAN����RX0�ж�ʹ�� */
#define CAN_RX0_INT_ENABLE      0                                                       /* 0,��ʹ��; 1,ʹ��; */

/* ����ӿں��� */
uint8_t can_receive_msg(uint32_t id, uint8_t *buf);                                     /* CAN��������, ��ѯ */
uint8_t can_send_msg(uint32_t id, uint8_t *msg, uint8_t len);                           /* CAN�������� */
uint8_t can_init(uint32_t tsjw,uint32_t tbs2,uint32_t tbs1,uint16_t brp,uint32_t mode); /* CAN��ʼ�� */

#endif

















