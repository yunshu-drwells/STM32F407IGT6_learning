/**
 ****************************************************************************************************
 * @file        ethernet.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-12-02
 * @brief       ETHERNET 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 F407电机开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211202
 * 第一次发布
 *
 ****************************************************************************************************
 */
 
#ifndef __ETHERNET_H
#define __ETHERNET_H
//#include "./SYSTEM/sys/sys.h"

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
//不引用上面三个头文件就会报错../Drivers/STM32F4xx_HAL_Driver/Inc/stm32f4xx_hal.h(220): error:  #20: identifier "HAL_StatusTypeDef" is undefined
#include "stm32f4xx_hal_conf.h"


/******************************************************************************************/
/* 引脚 定义 */

#define ETH_CLK_GPIO_PORT               GPIOA
#define ETH_CLK_GPIO_PIN                GPIO_PIN_1
#define ETH_CLK_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOA_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */

#define ETH_MDIO_GPIO_PORT              GPIOA
#define ETH_MDIO_GPIO_PIN               GPIO_PIN_2
#define ETH_MDIO_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOA_CLK_ENABLE();}while(0)                 /* 所在IO口时钟使能 */

#define ETH_CRS_GPIO_PORT               GPIOA
#define ETH_CRS_GPIO_PIN                GPIO_PIN_7
#define ETH_CRS_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOA_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */

#define ETH_MDC_GPIO_PORT               GPIOC
#define ETH_MDC_GPIO_PIN                GPIO_PIN_1
#define ETH_MDC_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */

#define ETH_RXD0_GPIO_PORT              GPIOC
#define ETH_RXD0_GPIO_PIN               GPIO_PIN_4
#define ETH_RXD0_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOC_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */

#define ETH_RXD1_GPIO_PORT              GPIOC
#define ETH_RXD1_GPIO_PIN               GPIO_PIN_5
#define ETH_RXD1_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOC_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */

#define ETH_TX_EN_GPIO_PORT             GPIOG
#define ETH_TX_EN_GPIO_PIN              GPIO_PIN_11
#define ETH_TX_EN_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOG_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */

#define ETH_TXD0_GPIO_PORT              GPIOG
#define ETH_TXD0_GPIO_PIN               GPIO_PIN_13
#define ETH_TXD0_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOG_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */

#define ETH_TXD1_GPIO_PORT              GPIOG
#define ETH_TXD1_GPIO_PIN               GPIO_PIN_14
#define ETH_TXD1_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOG_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */

#define ETH_RESET_GPIO_PORT             GPIOI
#define ETH_RESET_GPIO_PIN              GPIO_PIN_8
#define ETH_RESET_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOI_CLK_ENABLE();}while(0)                  /* 所在IO口时钟使能 */


/******************************************************************************************/

/* ETH端口定义 */
#define ETHERNET_RST(x)  do{ x ? \
                            HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET) : \
                            HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET); \
                        }while(0)



extern ETH_HandleTypeDef g_eth_handler;                                 /* 以太网句柄 */
extern ETH_DMADescTypeDef *g_eth_dma_rx_dscr_tab;                       /* 以太网DMA接收描述符数据结构体指针 */
extern ETH_DMADescTypeDef *g_eth_dma_tx_dscr_tab;                       /* 以太网DMA发送描述符数据结构体指针 */
extern uint8_t *g_eth_rx_buf;                                           /* 以太网底层驱动接收buffers指针 */
extern uint8_t *g_eth_tx_buf;                                           /* 以太网底层驱动发送buffers指针 */

uint8_t ethernet_init(void);                                            /* 以太网芯片初始化 */
uint32_t ethernet_read_phy(uint16_t reg);                               /* 读取以太网芯片寄存器值 */
void ethernet_write_phy(uint16_t reg, uint16_t value);                  /* 向以太网芯片指定地址写入寄存器值 */
uint8_t ethernet_chip_get_speed(void);                                  /* 获得以太网芯片的速度模式 */
uint32_t  ethernet_get_eth_rx_size(ETH_DMADescTypeDef *dma_rx_desc);    /* 获取接收到的帧长度 */

uint8_t ethernet_mem_malloc(void);                                      /* 为ETH底层驱动申请内存 */
void ethernet_mem_free(void);                                           /* 释放ETH 底层驱动申请的内存 */
#endif

