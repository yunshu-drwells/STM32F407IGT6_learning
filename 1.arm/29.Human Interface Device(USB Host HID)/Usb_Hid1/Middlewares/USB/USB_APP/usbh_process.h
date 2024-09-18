/**
 ****************************************************************************************************
 * @file        usbh_process.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-11-27
 * @brief       usbh diskio ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨������ԭ�� F407���������
 * ������Ƶ��www.yuanzige.com
 * ������̳��http://www.openedv.com/forum.php
 * ��˾��ַ��www.alientek.com
 * �����ַ��zhengdianyuanzi.tmall.com
 *
 * �޸�˵��
 * V1.0 200211127
 * ��һ�η���
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

extern USBH_HandleTypeDef g_hUSBHost;    /* USB Host����ṹ�� */

#endif
