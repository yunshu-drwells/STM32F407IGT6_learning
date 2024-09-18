#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f4xx_hal.h"

//IO方向设置
#define DS18B20_IO_IN()  {GPIOG->MODER&=~(3<<(6*2));GPIOG->MODER|=0<<(6*2);}	//PG6输入模式
#define DS18B20_IO_OUT() {GPIOG->MODER&=~(3<<(6*2));GPIOG->MODER|=1<<(6*2);} 	//PG6输出模式
 
////IO操作函数											   
#define	DS18B20_OUT_LOW   HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15,  GPIO_PIN_RESET)   //数据端口	PG6
#define	DS18B20_OUT_HIGH  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15,  GPIO_PIN_SET)   //数据端口	PG6
#define	DS18B20_DQ_IN     HAL_GPIO_ReadPin(GPIOG,  GPIO_PIN_15)  //数据端口	PG6 



uint8_t DS18B20_Init(void);					//初始化DS18B20
short DS18B20_Get_Temp(void);				//获取温度
void DS18B20_Start(void);						//开始温度转换
void DS18B20_Write_Byte(uint8_t dat);//写入一个字节
uint8_t DS18B20_Read_Byte(void);		//读出一个字节
uint8_t DS18B20_Read_Bit(void);			//读出一个位
uint8_t DS18B20_Check(void);				//检测是否存在DS18B20
void DS18B20_Reset(void);							//复位DS18B20 
#endif
