/**
 ****************************************************************************************************
 * @file        GT9xxx.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2023-05-29
 * @brief       4.3寸电容触摸屏-GT9xxx 驱动代码
 *   @note      GT系列电容触摸屏IC通用驱动,本代码支持: GT9147/GT917S/GT968/GT1151/GT9271 等多种
 *              驱动IC, 这些驱动IC仅ID不一样, 具体代码基本不需要做任何修改即可通过本代码直接驱动
 *
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
 * V1.1 20230529
 * 1, 新增对ST7796 3.5寸屏/ILI9806 4.3寸屏GT1151的支持
 * 2, gt9xxx_init函数中增加触摸IC判定，不属于特定触摸IC就返回1表示初始化失败
 ****************************************************************************************************
 */

#ifndef __GT9XXX_H
#define __GT9XXX_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
#include "main.h"  //引脚定义


/******************************************************************************************/
/* GT9XXX INT 和 RST 引脚 定义 */
/*
#define GT9XXX_RST_GPIO_PORT            GPIOG
#define GT9XXX_RST_GPIO_PIN             GPIO_PIN_1
#define GT9XXX_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)   // PG口时钟使能 

#define GT9XXX_INT_GPIO_PORT            GPIOH
#define GT9XXX_INT_GPIO_PIN             GPIO_PIN_7
#define GT9XXX_INT_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)   // PH口时钟使能 
*/

#define GT9XXX_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)   // PG口时钟使能 

#define GT9XXX_INT_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOH_CLK_ENABLE(); }while(0)   // PH口时钟使能 

/******************************************************************************************/

/* 与电容触摸屏连接的芯片引脚(未包含IIC引脚) 
 * IO操作函数 
 */
/*
#define GT9XXX_RST(x)     do{ x ? \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_PORT, GT9XXX_RST_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       // 复位引脚 

#define GT9XXX_INT        HAL_GPIO_ReadPin(GT9XXX_INT_GPIO_PORT, GT9XXX_INT_GPIO_PIN)     // 读取做的引脚
*/
													
#define GT9XXX_RST(x)     do{ x ? \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_Port, GT9XXX_RST_Pin, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(GT9XXX_RST_GPIO_Port, GT9XXX_RST_Pin, GPIO_PIN_RESET); \
                          }while(0)       // 复位引脚 

#define GT9XXX_INT        HAL_GPIO_ReadPin(GT9XXX_INT_GPIO_Port, GT9XXX_INT_Pin)     // 读取做的引脚 												

/* IIC读写命令 */
#define GT9XXX_CMD_WR       0x28        /* 写命令 */
#define GT9XXX_CMD_RD       0x29        /* 读命令 */

/* GT9XXX 部分寄存器定义  */
#define GT9XXX_CTRL_REG     0x8040      /* GT9XXX控制寄存器 */
#define GT9XXX_CFGS_REG     0x8047      /* GT9XXX配置起始地址寄存器 */
#define GT9XXX_CHECK_REG    0x80FF      /* GT9XXX校验和寄存器 */
#define GT9XXX_PID_REG      0x8140      /* GT9XXX产品ID寄存器 */

#define GT9XXX_GSTID_REG    0x814E      /* GT9XXX当前检测到的触摸情况 */
#define GT9XXX_TP1_REG      0x8150      /* 第一个触摸点数据地址 */
#define GT9XXX_TP2_REG      0x8158      /* 第二个触摸点数据地址 */
#define GT9XXX_TP3_REG      0x8160      /* 第三个触摸点数据地址 */
#define GT9XXX_TP4_REG      0x8168      /* 第四个触摸点数据地址 */
#define GT9XXX_TP5_REG      0x8170      /* 第五个触摸点数据地址 */
#define GT9XXX_TP6_REG      0x8178      /* 第六个触摸点数据地址 */
#define GT9XXX_TP7_REG      0x8180      /* 第七个触摸点数据地址 */
#define GT9XXX_TP8_REG      0x8188      /* 第八个触摸点数据地址 */
#define GT9XXX_TP9_REG      0x8190      /* 第九个触摸点数据地址 */
#define GT9XXX_TP10_REG     0x8198      /* 第十个触摸点数据地址 */
 
/******************************************************************************************/
/* 函数声明 */

uint8_t gt9xxx_wr_reg(uint16_t reg,uint8_t *buf,uint8_t len);
void gt9xxx_rd_reg(uint16_t reg,uint8_t *buf,uint8_t len); 
uint8_t gt9xxx_init(void);
uint8_t gt9xxx_scan(uint8_t mode); 

#endif
