#include "fonts.h"
#include "ff.h"
#include "delay.h"
#include "norflash.h"

 
/* 用来保存字库基本信息，地址，大小等 */
_font_info ftinfo;

/**
 * @brief       初始化字体
 * @param       无
 * @retval      0, 字库完好; 其他, 字库丢失;
 */
uint8_t fonts_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* 连续读取10次,都是错误,说明确实是有问题,得更新字库了 */
    {
        t++;
        norflash_read((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));    /* 读出ftinfo结构体数据 */

        if (ftinfo.fontok == 0XAA)
        {
            break;
        }
        
        delay_ms(20);
    }

    if (ftinfo.fontok != 0XAA)
    {
        return 1;
    }
    
    return 0;
}
