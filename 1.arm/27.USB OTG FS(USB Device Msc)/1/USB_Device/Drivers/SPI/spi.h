/**
 ****************************************************************************************************
 * @file        spi.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-26
 * @brief       SPI 驱动代码
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
 * V1.0 20211026
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __SPI_H
#define __SPI_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
//# include "stm32f4xx_hal_def.h"

/******************************************************************************************/
/* SPI2 引脚 定义 */

#define SPI2_SCK_GPIO_PORT              GPIOI
#define SPI2_SCK_GPIO_PIN               GPIO_PIN_1
#define SPI2_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)   /* PI口时钟使能 */

#define SPI2_MISO_GPIO_PORT             GPIOI
#define SPI2_MISO_GPIO_PIN              GPIO_PIN_2
#define SPI2_MISO_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)   /* PI口时钟使能 */

#define SPI2_MOSI_GPIO_PORT             GPIOI
#define SPI2_MOSI_GPIO_PIN              GPIO_PIN_3
#define SPI2_MOSI_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)   /* PI口时钟使能 */

/* SPI2相关定义 */
#define SPI2_SPI                        SPI2
#define SPI2_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI2_CLK_ENABLE(); }while(0)    /* SPI2时钟使能 */

/******************************************************************************************/


/* SPI总线速度设置 */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7


void spi2_init(void);
void spi2_set_speed(uint8_t speed);
uint8_t spi2_read_write_byte(uint8_t txdata);

#endif
























