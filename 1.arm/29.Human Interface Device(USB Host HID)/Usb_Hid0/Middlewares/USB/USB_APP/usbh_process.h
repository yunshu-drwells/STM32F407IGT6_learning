/**
 ****************************************************************************************************
 * @file        usbh_process.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-11-27
 * @brief       usbh diskio 驱动代码
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
 * V1.0 200211127
 * 第一次发布
 *
 ****************************************************************************************************
 */
#ifndef __USBH_PROCESS_H
#define __USBH_PROCESS_H

#include "usbh_core.h"
#include "usbh_hid.h"
#include "usbh_hid_parser.h"


void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
void usb_demo(USBH_HandleTypeDef *phost);
uint8_t usbh_check_enume_dead(USBH_HandleTypeDef *phost);
void usbh_hid_reconnect(void);

extern USBH_HandleTypeDef g_hUSBHost;    /* USB Host处理结构体 */

#endif
