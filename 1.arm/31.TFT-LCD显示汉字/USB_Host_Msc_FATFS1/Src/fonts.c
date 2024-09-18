#include "string.h"
#include "lcd.h"
#include "fonts.h"
#include "malloc.h"
#include "ff.h"
#include "usart.h"
#include "delay.h"
#include "norflash.h"
#include "fattester.h"


/* 字库区域占用的总扇区数大小(3个字库+unigbk表+字库信息=3238700 字节,约占791个25QXX扇区,一个扇区4K字节) */
#define FONTSECSIZE         791


/* 字库存放起始地址
 * 从第12MB地址开始存放字库
 * 前面12MB大小被文件系统占用
 * 12MB后紧跟3个字库+UNIGBK.BIN,总大小3.09M, 791个扇区,被字库占用了,不能动!
 * 15.10M以后, 用户可以自由使用. 建议用最后的100K字节比较好
 */
#define FONTINFOADDR        12 * 1024 * 1024

 
/* 用来保存字库基本信息，地址，大小等 */
_font_info ftinfo;

/* 字库存放在磁盘中的路径 */
char *const FONT_GBK_PATH[4] =
{
    "/SYSTEM/FONT/UNIGBK.BIN",      /* UNIGBK.BIN的存放位置 */
    "/SYSTEM/FONT/GBK12.FON",       /* GBK12的存放位置 */
    "/SYSTEM/FONT/GBK16.FON",       /* GBK16的存放位置 */
    "/SYSTEM/FONT/GBK24.FON",       /* GBK24的存放位置 */
};

/* 更新时的提示信息 */
char *const FONT_UPDATE_REMIND_TBL[4] =
{
    "Updating UNIGBK.BIN",          /* 提示正在更新UNIGBK.bin */
    "Updating GBK12.FON ",          /* 提示正在更新GBK12 */
    "Updating GBK16.FON ",          /* 提示正在更新GBK16 */
    "Updating GBK24.FON ",          /* 提示正在更新GBK24 */
};

/**
 * @brief       显示当前字体更新进度
 * @param       x, y    : 坐标
 * @param       size    : 字体大小
 * @param       totsize : 整个文件大小
 * @param       pos     : 当前文件指针位置
 * @param       color   : 字体颜色
 * @retval      无
 */
static void fonts_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color)
{
    float prog;
    uint8_t t = 0XFF;
    prog = (float)pos / totsize;
    prog *= 100;

    if (t != prog)
    {
        lcd_show_string(x + 3 * size / 2, y, 240, 320, size, "%", color);
        t = prog;

        if (t > 100)t = 100;

        lcd_show_num(x, y, t, 3, size, color);  /* 显示数值 */
    }
}
#include "usbh_def.h"  //USBH_HandleTypeDef
#include "usbh_core.h"  //USBH_Process
#include "main.h"  //usbh_hid_reconnect
extern USBH_HandleTypeDef hUsbHostFS;    /* USB Host处理结构体 在usb_host.c中定义*/
   
extern uint8_t usb_mount_status;               /* USB 挂载状态 */

/**
 * @brief       更新某一个字库
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息字体大小
 * @param       fpath   : 字体路径
 * @param       fx      : 更新的内容
 *   @arg                 0, ungbk;
 *   @Arg                 1, gbk12;
 *   @arg                 2, gbk16;
 *   @arg                 3, gbk24;
 * @param       color   : 字体颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
static uint8_t fonts_update_fontx(uint16_t x, uint16_t y, uint8_t size, uint8_t *fpath, uint8_t fx, uint16_t color)
{
		//printf("fonts_update_fontx start!\n");
    uint32_t flashaddr = 0;
    FIL *fftemp;
    uint8_t *tempbuf;
    uint8_t res;
    uint16_t bread;
    uint32_t offx = 0;
    uint8_t rval = 0;
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* 分配内存 */
		//printf("fonts_update_fontx mymalloc FIL * successed!\n");
    if (fftemp == NULL)rval = 1;

    tempbuf = mymalloc(SRAMIN, 4096);               /* 分配4096个字节空间 */
		//printf("fonts_update_fontx mymalloc tempbuf successed!\n");
	
    if (tempbuf == NULL)rval = 1;
	
		/*
		while (hUsbHostFS.gState == HOST_DEV_DISCONNECTED || hUsbHostFS.gState == HOST_CHECK_CLASS) {
				// 设备未连接或正在检查设备的类
				// 执行重新连接操作
				printf("hUsbHostFS.gState: %d\n", hUsbHostFS.gState);
				usb_mount_status = 0;
			 	//循环操作直到挂载u盘成功
				while(usb_mount_status==0)          // 等待 USB 设备挂载完成 
				{
						lcd_clear(WHITE);               // 清屏 
						lcd_show_string(30, 30, 200, 16, 16, "STM32", RED);
						lcd_show_string(30, 50, 200, 16, 16, "USB Connecting...", RED);
						USBH_Process(&hUsbHostFS);
						delay_ms(20);
						//usbh_hid_reconnect();
				}
		}
		*/

    res = f_open(fftemp, (const TCHAR *)fpath, FA_READ);
		//printf("fonts_update_fontx f_open return :%d\n", res);
		//printf("fftemp->obj.objsize: %u\n", fftemp->obj.objsize);
    if (res)rval = 2;   /* 打开文件失败 */
    if (rval == 0)
    {
				//printf("update *.BIN\n");
        switch (fx)
        {
            case 0: /* 更新 UNIGBK.BIN */
								//printf("updating UNIGBK.BIN\n");
                ftinfo.ugbkaddr = FONTINFOADDR + sizeof(ftinfo);    /* 信息头之后，紧跟UNIGBK转换码表 */
                ftinfo.ugbksize = fftemp->obj.objsize;              /* UNIGBK大小 */
                flashaddr = ftinfo.ugbkaddr;
                break;

            case 1: /* 更新 GBK12.BIN */
								//printf("updating GBK12.BIN \n");
                ftinfo.f12addr = ftinfo.ugbkaddr + ftinfo.ugbksize; /* UNIGBK之后，紧跟GBK12字库 */
                ftinfo.gbk12size = fftemp->obj.objsize;             /* GBK12字库大小 */
                flashaddr = ftinfo.f12addr;                         /* GBK12的起始地址 */
                break;

            case 2: /* 更新 GBK16.BIN */
								//printf("updating GBK16.BIN \n");
                ftinfo.f16addr = ftinfo.f12addr + ftinfo.gbk12size; /* GBK12之后，紧跟GBK16字库 */
                ftinfo.gbk16size = fftemp->obj.objsize;             /* GBK16字库大小 */
                flashaddr = ftinfo.f16addr;                         /* GBK16的起始地址 */
                break;

            case 3: /* 更新 GBK24.BIN */
								//printf("updating GBK24.BIN \n");
                ftinfo.f24addr = ftinfo.f16addr + ftinfo.gbk16size; /* GBK16之后，紧跟GBK24字库 */
                ftinfo.gbk24size = fftemp->obj.objsize;             /* GBK24字库大小 */
                flashaddr = ftinfo.f24addr;                         /* GBK24的起始地址 */
                break;
        }

        while (res == FR_OK)   /* 死循环执行 */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);    /* 读取数据 */
            if (res != FR_OK)break;     /* 执行错误 */

            norflash_write(tempbuf, offx + flashaddr, bread);       /* 从0开始写入bread个数据 */
            offx += bread;
            fonts_progress_show(x, y, size, fftemp->obj.objsize, offx, color);    /* 进度显示 */

            if (bread != 4096)break;    /* 读完了. */
            delay_ms(20);
        }

        f_close(fftemp);
    }

    myfree(SRAMIN, fftemp);     /* 释放内存 */
    myfree(SRAMIN, tempbuf);    /* 释放内存 */
    return res;
}

int find_file(const char *path, const char *filename) {
    FRESULT res;
    DIR dir;
    FILINFO fno;

    res = f_opendir(&dir, path); // 打开目录
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno); // 读取目录中的一个项目
            if (res != FR_OK || fno.fname[0] == 0) break; // 错误或到达目录末尾
            if (fno.fattrib & AM_DIR) {
                // 如果是目录，递归调用
                if (find_file(fno.fname, filename)) {
                    f_closedir(&dir);
                    return 1; // 找到文件
                }
            } else {
                // 如果是文件，检查文件名
                if (strcmp(fno.fname, filename) == 0) {
										//printf("find file: %s/%s\n", path, fno.fname);
                    f_closedir(&dir);
                    return 1; // 找到文件
                }
            }
        }
        f_closedir(&dir); // 关闭目录
    } else {
        printf("can not open: %s\n", path);
    }
    return 0; // 未找到文件
}

#include "exfuns.h"  //fs[1]
/**
 * @brief       更新字体文件
 *   @note      所有字库一起更新(UNIGBK,GBK12,GBK16,GBK24)
 * @param       x, y    : 提示信息的显示地址
 * @param       size    : 提示信息字体大小
 * @param       src     : 字库来源磁盘
 *   @arg                 "1:", FLASH盘
 * @param       color   : 字体颜色
 * @retval      0, 成功; 其他, 错误代码;
 */
uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color)
{
		//printf("fonts_update_font start\n");
    uint8_t *pname;
    uint32_t *buf;
    uint8_t res = 0;
    uint16_t i, j;
    FIL *fftemp;
    uint8_t rval = 0;
    res = 0xFF;
    ftinfo.fontok = 0xFF;
    pname = mymalloc(SRAMIN, 100);                         /* 申请100字节内存 */
    buf = mymalloc(SRAMIN, 4096);                           /* 申请4K字节内存 */
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));          /* 分配内存 */

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        myfree(SRAMIN, fftemp);
        myfree(SRAMIN, pname);
        myfree(SRAMIN, buf);
        return 5;           /* 内存申请失败 */
    }
		//查找三个文件是否存在
		/*
		for (i = 0; i < 4; i++) // 先查找文件UNIGBK,GBK12,GBK16,GBK24 是否正常 
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src内容到pname 
        strcat((char *)pname, "/SYSTEM/FONT/");    // 追加具体文件路径 
				printf("open pname: %s; ", pname);
				// 查找文件
				res = find_file((char *)pname, (char *)FONT_GBK_PATH[i]);
        if (res)
        {
						printf("not found %s\n", (char *)FONT_GBK_PATH[i]);
            rval |= 1 << 7; // 标记打开文件失败 
            break;          // 出错了,直接退出 
        }
    }
		*/

		//读方式打开三个文件，可以同时判断文件是否存在是否损坏
    for (i = 0; i < 4; i++) // 先查找文件UNIGBK,GBK12,GBK16,GBK24 是否正常 
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src内容到pname 
        strcat((char *)pname, (char *)FONT_GBK_PATH[i]);    // 追加具体文件路径 
				printf("open pname: %s; ", pname);

        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);// 尝试打开
				printf("f_open return :%d; ", res);
        if (res)
        {
            rval |= 1 << 7; // 标记打开文件失败 
            break;          // 出错了,直接退出 
        }
				//关闭文件
				//while(!f_close(fftemp));
				res = f_close(fftemp);
				printf("f_close return :%d\n", res);
    }
		
		//尝试打开四个文件之后，后面就不能再次打开了
		/*
		for (i = 0; i < 4; i++) // 先查找文件UNIGBK,GBK12,GBK16,GBK24 是否正常 
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src内容到pname
        strcat((char *)pname, (char *)FONT_GBK_PATH[i]);    // 追加具体文件路径
				printf("open pname: %s; ", pname);

        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);// 尝试打开
				printf("f_open return :%d; ", res);
        if (res)
        {
            rval |= 1 << 7; // 标记打开文件失败 
            break;          // 出错了,直接退出 
        }
				//关闭文件
				//while(!f_close(fftemp));
				res = f_close(fftemp);
				printf("f_close return :%d\n", res);
    }
		*/
		
		myfree(SRAMIN, fftemp); /* 释放内存 */

    if (rval == 0)          /* 字库文件都存在. */
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);            // 提示正在擦除扇区 

        for (i = 0; i < FONTSECSIZE; i++)           // 先擦除字库区域,提高写入速度 
        {
            fonts_progress_show(x + 20 * size / 2, y, size, FONTSECSIZE, i, color);     // 进度显示 
						
            norflash_read((uint8_t *)buf, ((FONTINFOADDR / 4096) + i) * 4096, 4096);    // 读出整个扇区的内容 

            for (j = 0; j < 1024; j++)              // 校验这个扇区的数据
            {
                if (buf[j] != 0xFFFFFFFF) break;    // 发现数据不为`0xFFFFFFFF`，则需要擦除该扇区 
            }

            if (j != 1024)
            {
                norflash_erase_sector((FONTINFOADDR / 4096) + i); // 需要擦除的扇区 
								//printf("norflash_erase_sector:%d\n", (FONTINFOADDR / 4096) + i);
            }
        }
				printf("Erasing sectors down!\n");
				
				//进度完成，清行
				//lcd_show_string(x, y, 240, 320, size, "Updating UNIGBK.BIN     ", color);
        for (i = 0; i < 4; i++) /* 依次更新UNIGBK,GBK12,GBK16,GBK24 */
        {
            lcd_show_string(x, y, 240, 320, size, FONT_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);                     /* copy src内容到pname */
            strcat((char *)pname, (char *)FONT_GBK_PATH[i]);        /* 追加具体文件路径 */
						printf("start update pname: %s; ", pname);
					
            res = fonts_update_fontx(x + 20 * size / 2, y, size, pname, i, color);  // 更新字库 

            if (res)
            {
								myfree(SRAMIN, buf);
                myfree(SRAMIN, pname);
                return 1 + i;
            }
        }

        /* 全部更新好了 */
        ftinfo.fontok = 0xAA;
        norflash_write((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));           /* 保存字库信息 */
    }

    myfree(SRAMIN, pname);  /* 释放内存 */
		myfree(SRAMIN, buf);
    return rval;            /* 无错误. */
}

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
