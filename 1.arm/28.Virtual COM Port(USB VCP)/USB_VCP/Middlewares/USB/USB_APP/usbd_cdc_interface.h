/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Inc/usbd_cdc_interface.h
  * @author  MCD Application Team
  * @brief   Header for usbd_cdc_interface.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

#include "usbd_cdc.h"


#define USB_USART_REC_LEN       200     /* USB串口接收缓冲区最大字节数 */

/* 轮询周期，最大65ms，最小1ms */
#define CDC_POLLING_INTERVAL    1       /* 轮询周期，最大65ms，最小1ms */

extern uint8_t  g_usb_usart_rx_buffer[USB_USART_REC_LEN];   /* 接收缓冲,最大USB_USART_REC_LEN个字节.末字节为换行符 */
extern uint16_t g_usb_usart_rx_sta;                         /* 接收状态标记 */

extern USBD_CDC_ItfTypeDef  USBD_CDC_fops;


void cdc_vcp_data_tx(uint8_t *buf,uint32_t len);
void cdc_vcp_data_rx(uint8_t* buf, uint32_t len);
void usb_printf(char* fmt,...); 

#endif 
