#ifndef __FONTS_H
#define __FONTS_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"

/* ������Ϣ�����׵�ַ
 * ռ41���ֽ�,��1���ֽ����ڱ���ֿ��Ƿ����.����ÿ8���ֽ�һ��,�ֱ𱣴���ʼ��ַ���ļ���С
 */
extern uint32_t FONTINFOADDR;

/* �ֿ���Ϣ�ṹ�嶨��
 * ���������ֿ������Ϣ����ַ����С��
 */
typedef __PACKED_STRUCT
{
    uint8_t fontok;             /* �ֿ���ڱ�־��0XAA���ֿ��������������ֿⲻ���� */
    uint32_t ugbkaddr;          /* unigbk�ĵ�ַ */
    uint32_t ugbksize;          /* unigbk�Ĵ�С */
    uint32_t f12addr;           /* gbk12��ַ */
    uint32_t gbk12size;         /* gbk12�Ĵ�С */
    uint32_t f16addr;           /* gbk16��ַ */
    uint32_t gbk16size;         /* gbk16�Ĵ�С */
    uint32_t f24addr;           /* gbk24��ַ */
    uint32_t gbk24size;         /* gbk24�Ĵ�С */
} _font_info;

/* �ֿ���Ϣ�ṹ�� */
extern _font_info ftinfo;

uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);  /* ����ȫ���ֿ� */
uint8_t fonts_init(void);       /* ��ʼ���ֿ� */

#endif
