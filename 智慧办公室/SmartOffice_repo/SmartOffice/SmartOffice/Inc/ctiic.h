/**
 ****************************************************************************************************
 * @file        CT_IIC.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-25
 * @brief       电容触摸屏 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台：正点原子 F407电机开发板
 * 在线视频：www.yuanzige.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 *
 * 修改说明
 * V1.0 20211025
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __CT_IIC_H
#define __CT_IIC_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"


/******************************************************************************************/
/* CT_IIC 引脚 定义 */

#define CT_IIC_SCL_GPIO_PORT            GPIOH
#define CT_IIC_SCL_GPIO_PIN             GPIO_PIN_6
#define CT_IIC_SCL_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)   /* PH口时钟使能 */

#define CT_IIC_SDA_GPIO_PORT            GPIOH
#define CT_IIC_SDA_GPIO_PIN             GPIO_PIN_8
#define CT_IIC_SDA_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)   /* PH口时钟使能 */

/******************************************************************************************/

/* IO操作 */
#define CT_IIC_SCL(x)     do{ x ? \
                              HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(CT_IIC_SCL_GPIO_PORT, CT_IIC_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SCL */

#define CT_IIC_SDA(x)     do{ x ? \
                              HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SDA */

#define CT_READ_SDA       HAL_GPIO_ReadPin(CT_IIC_SDA_GPIO_PORT, CT_IIC_SDA_GPIO_PIN) /* 读取SDA */


/* IIC所有操作函数 */
void ct_iic_init(void);             /* 初始化IIC的IO口 */
void ct_iic_stop(void);             /* 发送IIC停止信号 */
void ct_iic_start(void);            /* 发送IIC开始信号 */

void ct_iic_ack(void);              /* IIC发送ACK信号 */
void ct_iic_nack(void);             /* IIC不发送ACK信号 */
uint8_t ct_iic_wait_ack(void);      /* IIC等待ACK信号 */

void ct_iic_send_byte(uint8_t txd);         /* IIC发送一个字节 */
uint8_t ct_iic_read_byte(unsigned char ack);/* IIC读取一个字节 */

#endif
