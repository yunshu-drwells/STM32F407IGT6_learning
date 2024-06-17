/**
 ****************************************************************************************************
 * @file        ethernetif.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-02-14
 * @brief       网卡驱动代码
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
 * V1.0 20211014
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#include "lwip/err.h"
#include "lwip/netif.h"


err_t ethernetif_init(struct netif *netif);  /* 网卡初始化函数 */
void ethernetif_input(struct netif *netif);  /* 数据包输入函数 */
u32_t sys_now(void);  //lwip内核时钟
#endif

