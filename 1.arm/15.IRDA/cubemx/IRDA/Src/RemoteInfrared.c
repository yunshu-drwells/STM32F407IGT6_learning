#include "RemoteInfrared.h"

#define REPEAT_KEY  0xEE

extern __IO uint32_t GlobalTimingDelay100us;
extern __IO uint32_t GlobalTimingDelay100usTx;


__IO uint32_t FlagGotKey = 0;

__IO Remote_Infrared_data_union RemoteInfrareddata;


/************************************************************************
//����������  
-------------------------Э��--------------------------
��ʼ����9ms,������һ��4.5ms�ĸ�����,֪ͨ������ʼ����������
�����Ƿ���4��8λ��������,��һ������ң��ʶ����(REMOTE_ID),��һ��Ϊ
����(0),�ڶ���Ϊ����(255),�������������Ǽ�ֵ,��һ��Ϊ����
�ڶ���Ϊ����.�������40ms,ң���ٷ���һ��9ms��,2ms�ߵ�����,
��ʾ�����Ĵ���,����һ����֤��ֻ������һ��,������ֶ��,���
����Ϊ�ǳ������¸ü�.

*����: Remote_Infrared_KEY_ISR(INT11_vect )													 
*����: INT0�жϷ������		       									
*����: ��					          									
*����: ��		                           								
*************************************************************************/	
// ��������������Ϊ5ms
const uint32_t TIME_DELAY_6MS = 60;
const uint32_t TIME_DELAY_10MS = 100;

int fputc(int ch, FILE *f)
{ 	
	while( !(USART1->SR & (1<<7)));   //ѭ������,ֱ���������  
	
	USART1->DR = (uint8_t) ch;      
	return ch;
}

void Remote_Infrared_KEY_ISR(void)
{
	static __IO uint8_t  bBitCounter = 0; //����֡λ����
  static __IO uint32_t bKeyCode = 0;
	bBitCounter++;  //��¼�ж�ִ�д���


	if(bBitCounter == 1)        // ��ʼ����9ms
	{
        if(Remote_Infrared_DAT_INPUT){ // �ߵ�ƽ��Ч
            bBitCounter = 0;
        }
        else{
            GlobalTimingDelay100us = TIME_DELAY_10MS; //10ms
        }
	}
	else if(bBitCounter == 2)   // ����4.5ms�ĸ����� ������
	{
        if(Remote_Infrared_DAT_INPUT){
            if((GlobalTimingDelay100us > 2) && (GlobalTimingDelay100us < 18)){  //ʣ��1ms���ң�˵������9ms(�������)
                GlobalTimingDelay100us = TIME_DELAY_6MS;
            }
            else{ 
                bBitCounter = 0; 
                //printf(".");
            }
        }
        else{
            bBitCounter = 0;            
        }
	}
	else if(bBitCounter == 3)   // ����4.5ms�ĸ����� �½���
	{
        if(Remote_Infrared_DAT_INPUT)
        {
            bBitCounter = 0; 
        }
        else
        {
            if((GlobalTimingDelay100us > 5) && (GlobalTimingDelay100us < 20)){    //��ʼ�� 4.5ms
                GlobalTimingDelay100us = TIME_DELAY_6MS;
                //printf("������");
            }
            else if((GlobalTimingDelay100us > 32) && (GlobalTimingDelay100us < 46)){   //�ظ��� 2.25ms
							  //�ظ���
                bBitCounter = 0;
                RemoteInfrareddata.uiRemoteInfraredData = bKeyCode;
                //RemoteInfrareddata.uiRemoteInfraredData = REPEAT_KEY;
                bBitCounter = 0;
                FlagGotKey = 1;
            }            
            else{
                bBitCounter = 0; 
                //printf("%d&", GlobalTimingDelay100us);
            }          
        }
	}    
	else if(bBitCounter > 3 && bBitCounter < 68) //����8λ����
	{  

        if(Remote_Infrared_DAT_INPUT)          //�����������͵�ƽ��ʱ��
        {
            if((GlobalTimingDelay100us > 50) && (GlobalTimingDelay100us < 58)){
                GlobalTimingDelay100us = TIME_DELAY_6MS;
            }
            else{
                bBitCounter = 0; 
                //printf("#");
            }           
        }
        else{
            if((GlobalTimingDelay100us > 50) && (GlobalTimingDelay100us < 58)){       // '0'  0.56ms����
                GlobalTimingDelay100us = TIME_DELAY_6MS;
								bKeyCode <<= 1;  // MSB First 
                bKeyCode += 0x00;
            }
            else if((GlobalTimingDelay100us > 40) && (GlobalTimingDelay100us < 48)){  //'1'   1.685ms����
                GlobalTimingDelay100us = TIME_DELAY_6MS;            
								bKeyCode <<= 1;  // MSB First 
                bKeyCode += 0x01;
            }              
            else{
                bBitCounter = 0; 
            }  
        }

       if(bBitCounter == 67){
            RemoteInfrareddata.uiRemoteInfraredData = bKeyCode;
            bBitCounter = 0;
            FlagGotKey = 1;
            //printf("KeyCode = 0x%X", bKeyCode);
        }
	}
	else{
		bBitCounter = 0;
        //printf("KeyCode = 0x%X", bKeyCode);
	}
}

/************************************************************************
*����: unsigned char Remote_Infrared_KeyDeCode(unsigned char bKeyCode)					 
*����: PS2���̽�������		       									    
*����: bKeyCode ������ 							
*����: ������ASIIC��		                           								
************************************************************************/
uint8_t Remote_Infrared_KeyDeCode(void)
{
//	uint8_t Key = 0xFF;
	if (FlagGotKey == 1)//ͨ��
	{
        FlagGotKey = 0;
		
        if((RemoteInfrareddata.RemoteInfraredDataStruct.bID == (uint8_t)~ RemoteInfrareddata.RemoteInfraredDataStruct.bIDNot)
            && (RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCode == (uint8_t)~ RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCodeNot))
        {
            printf("\n\r IR Receive KeyCode = 0x%02X, ", RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCode);
						//printf("code is %x", RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCode);

						switch(RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCode)
						{
							case 0:
								printf("ERROR  ");
								break;
							case 0xA2:
								printf("CH-    ");
								break;
							case 0X62:
								printf("CH     ");
								break;
							case 0XE2:
								printf("CH+    ");
								break;
							case 0X22:
								printf("PREV    ");
								break;
							case 0X02:
								printf("NEXT    ");
								break;
							case 0XC2:
								printf("PLAY/PAUSE");
								break;
							case 0XE0:
								printf("VOL-   ");
								break;
							case 0XA8:
								printf("VOL+   ");
								break;
							case 0X90:
								printf("EQ     ");
								break;
							case 0X98:
								printf("100+   ");
								break;
							case 0XB0:
								printf("200+   ");
								break;
							case 0x68:
								printf("0      ");
								break;
							case 0x30:
								printf("1      ");
								break;
							case 0x18:
								printf("2      ");
								break;
							case 0x7A:
								printf("3      ");
								break;
							case 0x10:
								printf("4      ");
								break;
							case 0x38:
								printf("5      ");
								break;
							case 0x5A:
								printf("6      ");
								break;
							case 0x42:
								printf("7      ");
								break;
							case 0x4A:
								printf("8      ");
								break;
							case 0x52:
								printf("9      ");
								break;
							default:
									printf("Unknown key!");
						}
	
				}
        else
        {
          printf("\n\r ERR 0x%08X", RemoteInfrareddata.uiRemoteInfraredData);
        }
    }

		return RemoteInfrareddata.RemoteInfraredDataStruct.bKeyCode;
}

