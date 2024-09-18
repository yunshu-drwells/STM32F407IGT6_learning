#include "string.h"
#include "lcd.h"
#include "fonts.h"
#include "malloc.h"
#include "ff.h"
#include "usart.h"
#include "delay.h"
#include "norflash.h"
#include "fattester.h"


/* �ֿ�����ռ�õ�����������С(3���ֿ�+unigbk��+�ֿ���Ϣ=3238700 �ֽ�,Լռ791��25QXX����,һ������4K�ֽ�) */
#define FONTSECSIZE         791


/* �ֿ�����ʼ��ַ
 * �ӵ�12MB��ַ��ʼ����ֿ�
 * ǰ��12MB��С���ļ�ϵͳռ��
 * 12MB�����3���ֿ�+UNIGBK.BIN,�ܴ�С3.09M, 791������,���ֿ�ռ����,���ܶ�!
 * 15.10M�Ժ�, �û���������ʹ��. ����������100K�ֽڱȽϺ�
 */
#define FONTINFOADDR        12 * 1024 * 1024

 
/* ���������ֿ������Ϣ����ַ����С�� */
_font_info ftinfo;

/* �ֿ����ڴ����е�·�� */
char *const FONT_GBK_PATH[4] =
{
    "/SYSTEM/FONT/UNIGBK.BIN",      /* UNIGBK.BIN�Ĵ��λ�� */
    "/SYSTEM/FONT/GBK12.FON",       /* GBK12�Ĵ��λ�� */
    "/SYSTEM/FONT/GBK16.FON",       /* GBK16�Ĵ��λ�� */
    "/SYSTEM/FONT/GBK24.FON",       /* GBK24�Ĵ��λ�� */
};

/* ����ʱ����ʾ��Ϣ */
char *const FONT_UPDATE_REMIND_TBL[4] =
{
    "Updating UNIGBK.BIN",          /* ��ʾ���ڸ���UNIGBK.bin */
    "Updating GBK12.FON ",          /* ��ʾ���ڸ���GBK12 */
    "Updating GBK16.FON ",          /* ��ʾ���ڸ���GBK16 */
    "Updating GBK24.FON ",          /* ��ʾ���ڸ���GBK24 */
};

/**
 * @brief       ��ʾ��ǰ������½���
 * @param       x, y    : ����
 * @param       size    : �����С
 * @param       totsize : �����ļ���С
 * @param       pos     : ��ǰ�ļ�ָ��λ��
 * @param       color   : ������ɫ
 * @retval      ��
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

        lcd_show_num(x, y, t, 3, size, color);  /* ��ʾ��ֵ */
    }
}
#include "usbh_def.h"  //USBH_HandleTypeDef
#include "usbh_core.h"  //USBH_Process
#include "main.h"  //usbh_hid_reconnect
extern USBH_HandleTypeDef hUsbHostFS;    /* USB Host����ṹ�� ��usb_host.c�ж���*/
   
extern uint8_t usb_mount_status;               /* USB ����״̬ */

/**
 * @brief       ����ĳһ���ֿ�
 * @param       x, y    : ��ʾ��Ϣ����ʾ��ַ
 * @param       size    : ��ʾ��Ϣ�����С
 * @param       fpath   : ����·��
 * @param       fx      : ���µ�����
 *   @arg                 0, ungbk;
 *   @Arg                 1, gbk12;
 *   @arg                 2, gbk16;
 *   @arg                 3, gbk24;
 * @param       color   : ������ɫ
 * @retval      0, �ɹ�; ����, �������;
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
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));  /* �����ڴ� */
		//printf("fonts_update_fontx mymalloc FIL * successed!\n");
    if (fftemp == NULL)rval = 1;

    tempbuf = mymalloc(SRAMIN, 4096);               /* ����4096���ֽڿռ� */
		//printf("fonts_update_fontx mymalloc tempbuf successed!\n");
	
    if (tempbuf == NULL)rval = 1;
	
		/*
		while (hUsbHostFS.gState == HOST_DEV_DISCONNECTED || hUsbHostFS.gState == HOST_CHECK_CLASS) {
				// �豸δ���ӻ����ڼ���豸����
				// ִ���������Ӳ���
				printf("hUsbHostFS.gState: %d\n", hUsbHostFS.gState);
				usb_mount_status = 0;
			 	//ѭ������ֱ������u�̳ɹ�
				while(usb_mount_status==0)          // �ȴ� USB �豸������� 
				{
						lcd_clear(WHITE);               // ���� 
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
    if (res)rval = 2;   /* ���ļ�ʧ�� */
    if (rval == 0)
    {
				//printf("update *.BIN\n");
        switch (fx)
        {
            case 0: /* ���� UNIGBK.BIN */
								//printf("updating UNIGBK.BIN\n");
                ftinfo.ugbkaddr = FONTINFOADDR + sizeof(ftinfo);    /* ��Ϣͷ֮�󣬽���UNIGBKת����� */
                ftinfo.ugbksize = fftemp->obj.objsize;              /* UNIGBK��С */
                flashaddr = ftinfo.ugbkaddr;
                break;

            case 1: /* ���� GBK12.BIN */
								//printf("updating GBK12.BIN \n");
                ftinfo.f12addr = ftinfo.ugbkaddr + ftinfo.ugbksize; /* UNIGBK֮�󣬽���GBK12�ֿ� */
                ftinfo.gbk12size = fftemp->obj.objsize;             /* GBK12�ֿ��С */
                flashaddr = ftinfo.f12addr;                         /* GBK12����ʼ��ַ */
                break;

            case 2: /* ���� GBK16.BIN */
								//printf("updating GBK16.BIN \n");
                ftinfo.f16addr = ftinfo.f12addr + ftinfo.gbk12size; /* GBK12֮�󣬽���GBK16�ֿ� */
                ftinfo.gbk16size = fftemp->obj.objsize;             /* GBK16�ֿ��С */
                flashaddr = ftinfo.f16addr;                         /* GBK16����ʼ��ַ */
                break;

            case 3: /* ���� GBK24.BIN */
								//printf("updating GBK24.BIN \n");
                ftinfo.f24addr = ftinfo.f16addr + ftinfo.gbk16size; /* GBK16֮�󣬽���GBK24�ֿ� */
                ftinfo.gbk24size = fftemp->obj.objsize;             /* GBK24�ֿ��С */
                flashaddr = ftinfo.f24addr;                         /* GBK24����ʼ��ַ */
                break;
        }

        while (res == FR_OK)   /* ��ѭ��ִ�� */
        {
            res = f_read(fftemp, tempbuf, 4096, (UINT *)&bread);    /* ��ȡ���� */
            if (res != FR_OK)break;     /* ִ�д��� */

            norflash_write(tempbuf, offx + flashaddr, bread);       /* ��0��ʼд��bread������ */
            offx += bread;
            fonts_progress_show(x, y, size, fftemp->obj.objsize, offx, color);    /* ������ʾ */

            if (bread != 4096)break;    /* ������. */
            delay_ms(20);
        }

        f_close(fftemp);
    }

    myfree(SRAMIN, fftemp);     /* �ͷ��ڴ� */
    myfree(SRAMIN, tempbuf);    /* �ͷ��ڴ� */
    return res;
}

int find_file(const char *path, const char *filename) {
    FRESULT res;
    DIR dir;
    FILINFO fno;

    res = f_opendir(&dir, path); // ��Ŀ¼
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno); // ��ȡĿ¼�е�һ����Ŀ
            if (res != FR_OK || fno.fname[0] == 0) break; // ����򵽴�Ŀ¼ĩβ
            if (fno.fattrib & AM_DIR) {
                // �����Ŀ¼���ݹ����
                if (find_file(fno.fname, filename)) {
                    f_closedir(&dir);
                    return 1; // �ҵ��ļ�
                }
            } else {
                // ������ļ�������ļ���
                if (strcmp(fno.fname, filename) == 0) {
										//printf("find file: %s/%s\n", path, fno.fname);
                    f_closedir(&dir);
                    return 1; // �ҵ��ļ�
                }
            }
        }
        f_closedir(&dir); // �ر�Ŀ¼
    } else {
        printf("can not open: %s\n", path);
    }
    return 0; // δ�ҵ��ļ�
}

#include "exfuns.h"  //fs[1]
/**
 * @brief       ���������ļ�
 *   @note      �����ֿ�һ�����(UNIGBK,GBK12,GBK16,GBK24)
 * @param       x, y    : ��ʾ��Ϣ����ʾ��ַ
 * @param       size    : ��ʾ��Ϣ�����С
 * @param       src     : �ֿ���Դ����
 *   @arg                 "1:", FLASH��
 * @param       color   : ������ɫ
 * @retval      0, �ɹ�; ����, �������;
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
    pname = mymalloc(SRAMIN, 100);                         /* ����100�ֽ��ڴ� */
    buf = mymalloc(SRAMIN, 4096);                           /* ����4K�ֽ��ڴ� */
    fftemp = (FIL *)mymalloc(SRAMIN, sizeof(FIL));          /* �����ڴ� */

    if (buf == NULL || pname == NULL || fftemp == NULL)
    {
        myfree(SRAMIN, fftemp);
        myfree(SRAMIN, pname);
        myfree(SRAMIN, buf);
        return 5;           /* �ڴ�����ʧ�� */
    }
		//���������ļ��Ƿ����
		/*
		for (i = 0; i < 4; i++) // �Ȳ����ļ�UNIGBK,GBK12,GBK16,GBK24 �Ƿ����� 
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src���ݵ�pname 
        strcat((char *)pname, "/SYSTEM/FONT/");    // ׷�Ӿ����ļ�·�� 
				printf("open pname: %s; ", pname);
				// �����ļ�
				res = find_file((char *)pname, (char *)FONT_GBK_PATH[i]);
        if (res)
        {
						printf("not found %s\n", (char *)FONT_GBK_PATH[i]);
            rval |= 1 << 7; // ��Ǵ��ļ�ʧ�� 
            break;          // ������,ֱ���˳� 
        }
    }
		*/

		//����ʽ�������ļ�������ͬʱ�ж��ļ��Ƿ�����Ƿ���
    for (i = 0; i < 4; i++) // �Ȳ����ļ�UNIGBK,GBK12,GBK16,GBK24 �Ƿ����� 
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src���ݵ�pname 
        strcat((char *)pname, (char *)FONT_GBK_PATH[i]);    // ׷�Ӿ����ļ�·�� 
				printf("open pname: %s; ", pname);

        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);// ���Դ�
				printf("f_open return :%d; ", res);
        if (res)
        {
            rval |= 1 << 7; // ��Ǵ��ļ�ʧ�� 
            break;          // ������,ֱ���˳� 
        }
				//�ر��ļ�
				//while(!f_close(fftemp));
				res = f_close(fftemp);
				printf("f_close return :%d\n", res);
    }
		
		//���Դ��ĸ��ļ�֮�󣬺���Ͳ����ٴδ���
		/*
		for (i = 0; i < 4; i++) // �Ȳ����ļ�UNIGBK,GBK12,GBK16,GBK24 �Ƿ����� 
    {
        delay_ms(100);
        strcpy((char *)pname, (char *)src);                 // copy src���ݵ�pname
        strcat((char *)pname, (char *)FONT_GBK_PATH[i]);    // ׷�Ӿ����ļ�·��
				printf("open pname: %s; ", pname);

        res = f_open(fftemp, (const TCHAR *)pname, FA_READ);// ���Դ�
				printf("f_open return :%d; ", res);
        if (res)
        {
            rval |= 1 << 7; // ��Ǵ��ļ�ʧ�� 
            break;          // ������,ֱ���˳� 
        }
				//�ر��ļ�
				//while(!f_close(fftemp));
				res = f_close(fftemp);
				printf("f_close return :%d\n", res);
    }
		*/
		
		myfree(SRAMIN, fftemp); /* �ͷ��ڴ� */

    if (rval == 0)          /* �ֿ��ļ�������. */
    {
        lcd_show_string(x, y, 240, 320, size, "Erasing sectors... ", color);            // ��ʾ���ڲ������� 

        for (i = 0; i < FONTSECSIZE; i++)           // �Ȳ����ֿ�����,���д���ٶ� 
        {
            fonts_progress_show(x + 20 * size / 2, y, size, FONTSECSIZE, i, color);     // ������ʾ 
						
            norflash_read((uint8_t *)buf, ((FONTINFOADDR / 4096) + i) * 4096, 4096);    // ������������������ 

            for (j = 0; j < 1024; j++)              // У���������������
            {
                if (buf[j] != 0xFFFFFFFF) break;    // �������ݲ�Ϊ`0xFFFFFFFF`������Ҫ���������� 
            }

            if (j != 1024)
            {
                norflash_erase_sector((FONTINFOADDR / 4096) + i); // ��Ҫ���������� 
								//printf("norflash_erase_sector:%d\n", (FONTINFOADDR / 4096) + i);
            }
        }
				printf("Erasing sectors down!\n");
				
				//������ɣ�����
				//lcd_show_string(x, y, 240, 320, size, "Updating UNIGBK.BIN     ", color);
        for (i = 0; i < 4; i++) /* ���θ���UNIGBK,GBK12,GBK16,GBK24 */
        {
            lcd_show_string(x, y, 240, 320, size, FONT_UPDATE_REMIND_TBL[i], color);
            strcpy((char *)pname, (char *)src);                     /* copy src���ݵ�pname */
            strcat((char *)pname, (char *)FONT_GBK_PATH[i]);        /* ׷�Ӿ����ļ�·�� */
						printf("start update pname: %s; ", pname);
					
            res = fonts_update_fontx(x + 20 * size / 2, y, size, pname, i, color);  // �����ֿ� 

            if (res)
            {
								myfree(SRAMIN, buf);
                myfree(SRAMIN, pname);
                return 1 + i;
            }
        }

        /* ȫ�����º��� */
        ftinfo.fontok = 0xAA;
        norflash_write((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));           /* �����ֿ���Ϣ */
    }

    myfree(SRAMIN, pname);  /* �ͷ��ڴ� */
		myfree(SRAMIN, buf);
    return rval;            /* �޴���. */
}

/**
 * @brief       ��ʼ������
 * @param       ��
 * @retval      0, �ֿ����; ����, �ֿⶪʧ;
 */
uint8_t fonts_init(void)
{
    uint8_t t = 0;

    while (t < 10)  /* ������ȡ10��,���Ǵ���,˵��ȷʵ��������,�ø����ֿ��� */
    {
        t++;
        norflash_read((uint8_t *)&ftinfo, FONTINFOADDR, sizeof(ftinfo));    /* ����ftinfo�ṹ������ */

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
