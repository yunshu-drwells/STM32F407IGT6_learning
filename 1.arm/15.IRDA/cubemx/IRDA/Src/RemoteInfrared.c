#include "RemoteInfrared.h"

#define REPEAT_KEY  0xEE

extern __IO uint32_t GlobalTimingDelay100us;
extern __IO uint32_t GlobalTimingDelay100usTx;


__IO uint32_t FlagGotKey = 0;

__IO Remote_Infrared_data_union RemoteInfrareddata;


/************************************************************************
//处理红外接收  
-------------------------协议--------------------------
开始拉低9ms,接着是一个4.5ms的高脉冲,通知器件开始传送数据了
接着是发送4个8位二进制码,第一二个是遥控识别码(REMOTE_ID),第一个为
正码(0),第二个为反码(255),接着两个数据是键值,第一个为正码
第二个为反码.发送完后40ms,遥控再发送一个9ms低,2ms高的脉冲,
表示按键的次数,出现一次则证明只按下了一次,如果出现多次,则可
以认为是持续按下该键.

*名称: Remote_Infrared_KEY_ISR(INT11_vect )													 
*功能: INT0中断服务程序		       									
*参数: 无					          									
*返回: 无		                           								
*************************************************************************/	
// 检测脉冲宽度最长脉宽为5ms
const uint32_t TIME_DELAY_6MS = 60;
const uint32_t TIME_DELAY_10MS = 100;

int fputc(int ch, FILE *f)
{ 	
	while( !(USART1->SR & (1<<7)));   //循环发送,直到发送完毕  
	
	USART1->DR = (uint8_t) ch;      
	return ch;
}

void Remote_Infrared_KEY_ISR(void)
{
	static __IO uint8_t  bBitCounter = 0; //按键帧位计数
  static __IO uint32_t bKeyCode = 0;
	bBitCounter++;  //记录中断执行次数


	if(bBitCounter == 1)        // 开始拉低9ms
	{
        if(Remote_Infrared_DAT_INPUT){ // 高电平无效
            bBitCounter = 0;
        }
        else{
            GlobalTimingDelay100us = TIME_DELAY_10MS; //10ms
        }
	}
	else if(bBitCounter == 2)   // 持续4.5ms的高脉冲 上升沿
	{
        if(Remote_Infrared_DAT_INPUT){
            if((GlobalTimingDelay100us > 2) && (GlobalTimingDelay100us < 18)){  //剩余1ms左右，说明经过9ms(考虑误差)
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
	else if(bBitCounter == 3)   // 持续4.5ms的高脉冲 下降沿
	{
        if(Remote_Infrared_DAT_INPUT)
        {
            bBitCounter = 0; 
        }
        else
        {
            if((GlobalTimingDelay100us > 5) && (GlobalTimingDelay100us < 20)){    //起始码 4.5ms
                GlobalTimingDelay100us = TIME_DELAY_6MS;
                //printf("引导码");
            }
            else if((GlobalTimingDelay100us > 32) && (GlobalTimingDelay100us < 46)){   //重复码 2.25ms
							  //重复码
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
	else if(bBitCounter > 3 && bBitCounter < 68) //接收8位数据
	{  

        if(Remote_Infrared_DAT_INPUT)          //检测数据脉冲低电平的时间
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
            if((GlobalTimingDelay100us > 50) && (GlobalTimingDelay100us < 58)){       // '0'  0.56ms左右
                GlobalTimingDelay100us = TIME_DELAY_6MS;
								bKeyCode <<= 1;  // MSB First 
                bKeyCode += 0x00;
            }
            else if((GlobalTimingDelay100us > 40) && (GlobalTimingDelay100us < 48)){  //'1'   1.685ms左右
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
*名称: unsigned char Remote_Infrared_KeyDeCode(unsigned char bKeyCode)					 
*功能: PS2键盘解码程序		       									    
*参数: bKeyCode 键盘码 							
*返回: 按键的ASIIC码		                           								
************************************************************************/
uint8_t Remote_Infrared_KeyDeCode(void)
{
//	uint8_t Key = 0xFF;
	if (FlagGotKey == 1)//通码
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

