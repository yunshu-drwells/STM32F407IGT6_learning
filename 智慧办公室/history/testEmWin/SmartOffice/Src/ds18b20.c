#include "ds18b20.h"
#include "delay.h"
uint32_t usctick = 0;
uint32_t time_delay = 0;
extern TIM_HandleTypeDef htim6;

//复位DS18B20
void DS18B20_Reset(void)	   
{                 
	DS18B20_IO_OUT();   //设置为输出
	DS18B20_OUT_LOW ; 	  //拉低DQ
	delay_us(650);      //拉低650us
	DS18B20_OUT_HIGH ; 	//拉高DQ 
	delay_us(20);       //20US
}

//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();    //设置为输入
	
	 //等待DS18B20拉低总线回应，如果超过200us未拉低，则认为未回应
  while ((DS18B20_DQ_IN == 1) && (retry<200))   
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;        //DS18B20超时未拉低总线
	else retry=0;                  //DS18B20拉低总线
	 
  while ( (DS18B20_DQ_IN == 0 ) && ( retry < 240) )    //测试拉低总线的时间是否在240us内
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	      //超过240us错误   
	
	return 0;                     //正确回应
}

//从DS18B20读取一个位
//返回值：1/0
uint8_t DS18B20_Read_Bit(void) 
{
	uint8_t data;
	DS18B20_IO_OUT();   //设置为输出
	DS18B20_OUT_LOW ; 	  //拉低DQ
	delay_us(3);
	DS18B20_OUT_HIGH ; 	//拉高DQ 
	DS18B20_IO_IN();    //设置为输入
	delay_us(12);
	
	if(DS18B20_DQ_IN)  data=1;
	else data=0;
	
	delay_us(50);  
	
	return data;
}

//从DS18B20读取一个字节
//返回值：读到的数据,先读数据的低位
uint8_t DS18B20_Read_Byte(void)   
{        
	uint8_t i,j,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<i)|dat;
    }						    
	return dat;
}

//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
    DS18B20_IO_OUT();     //设置为输出
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // 写1
        {
            DS18B20_OUT_LOW ; 	  //拉低DQ
            delay_us(2);                            
            DS18B20_OUT_HIGH ; 	  //拉高DQ 
            delay_us(60);             
        }
        else            //写0
        {
            DS18B20_OUT_LOW ; 	  //拉低DQ
            delay_us(60);             
            DS18B20_OUT_HIGH ; 	  //拉高DQ 
            delay_us(2);                          
        }
    }
}
 

void DS18B20_Start(void)
{ 
		//开始温度转换
    DS18B20_Reset();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0x44);   // convert
	
		//开始读取温度
		DS18B20_Reset();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0xbe);   // convert
}

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DS18B20_Init(void)
{
	DS18B20_Reset();
	return DS18B20_Check();
}

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
    short tem;
	
    DS18B20_Start ();           //开始转换读取
	    
    TL=DS18B20_Read_Byte();     // LSB   
    TH=DS18B20_Read_Byte();     // MSB  	
    if(TH>7)										//温度为负  
    {
        TH=~TH;
        TL=~TL; 
        temp=0;      //温度为负  
    }else temp=1;    //温度为正	  
	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(double)tem*0.625;//转换  获得不带符号位的11位温度值   
		
	if(temp)return tem; //返回温度值
	else return -tem;    
}
