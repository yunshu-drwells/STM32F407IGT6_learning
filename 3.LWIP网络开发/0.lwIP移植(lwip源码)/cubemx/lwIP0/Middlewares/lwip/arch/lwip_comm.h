/**
 ****************************************************************************************************
 * @file        lwip_comm.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-12-02
 * @brief       LWIP配置驱动代码
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
 
#ifndef _LWIP_COMM_H
#define _LWIP_COMM_H 
#include "ethernet.h"


/* DHCP进程状态 */
#define LWIP_DHCP_OFF                   (uint8_t) 0     /* DHCP服务器关闭状态 */
#define LWIP_DHCP_START                 (uint8_t) 1     /* DHCP服务器启动状态 */
#define LWIP_DHCP_WAIT_ADDRESS          (uint8_t) 2     /* DHCP服务器等待分配IP状态 */
#define LWIP_DHCP_ADDRESS_ASSIGNED      (uint8_t) 3     /* DHCP服务器地址已分配状态 */
#define LWIP_DHCP_TIMEOUT               (uint8_t) 4     /* DHCP服务器超时状态 */
#define LWIP_DHCP_LINK_DOWN             (uint8_t) 5     /* DHCP服务器链接失败状态 */

#define LWIP_MAX_DHCP_TRIES                       4    /* DHCP服务器最大重试次数 */

/*lwip控制结构体*/
typedef struct  
{
    uint8_t mac[6];                 /* MAC地址 */
    uint8_t remoteip[4];            /* 远端主机IP地址 */ 
    uint8_t ip[4];                  /* 本机IP地址 */
    uint8_t netmask[4];             /* 子网掩码 */
    uint8_t gateway[4];             /* 默认网关的IP地址 */
    uint8_t dhcpstatus;             /* dhcp状态 */
                                        /* 0, 未获取DHCP地址;*/
                                        /* 1, 进入DHCP获取状态*/
                                        /* 2, 成功获取DHCP地址*/
                                        /* 0XFF,获取失败 */
}__lwip_dev;

extern __lwip_dev g_lwipdev;        /* lwip控制结构体 */
 

void lwip_packet_handler(void);                     /* 当接收到数据后调用 */
void lwip_periodic_handle(void);                    /* lwip_periodic_handle */
void lwip_comm_default_ip_set(__lwip_dev *lwipx);   /* lwip 默认IP设置 */
uint8_t lwip_comm_mem_malloc(void);                 /* 内存堆 ram_heap 和内存池 memp_memory 的内存分配 */
void lwip_comm_mem_free(void);                      /* lwip中mem和memp内存释放 */
uint8_t lwip_comm_init(void);                       /* LWIP初始化(LWIP启动的时候使用) */
void lwip_dhcp_process_handle(void);                /* DHCP处理任务 */

#endif













