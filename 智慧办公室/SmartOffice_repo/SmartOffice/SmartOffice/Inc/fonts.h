#ifndef __FONTS_H
#define __FONTS_H

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"

/* 字体信息保存首地址
 * 占41个字节,第1个字节用于标记字库是否存在.后续每8个字节一组,分别保存起始地址和文件大小
 */
#define FONTINFOADDR        12 * 1024 * 1024

/* 字库信息结构体定义
 * 用来保存字库基本信息，地址，大小等
 */
typedef __PACKED_STRUCT
{
    uint8_t fontok;             /* 字库存在标志，0XAA，字库正常；其他，字库不存在 */
    uint32_t ugbkaddr;          /* unigbk的地址 */
    uint32_t ugbksize;          /* unigbk的大小 */
    uint32_t f12addr;           /* gbk12地址 */
    uint32_t gbk12size;         /* gbk12的大小 */
    uint32_t f16addr;           /* gbk16地址 */
    uint32_t gbk16size;         /* gbk16的大小 */
    uint32_t f24addr;           /* gbk24地址 */
    uint32_t gbk24size;         /* gbk24的大小 */
} _font_info;

/* 字库信息结构体 */
extern _font_info ftinfo;

uint8_t fonts_init(void);       /* 初始化字库 */

#endif
