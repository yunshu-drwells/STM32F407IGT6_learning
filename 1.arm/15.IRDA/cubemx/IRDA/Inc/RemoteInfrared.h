#include "stm32f4xx_hal.h"

#define	Remote_Infrared_DAT_INPUT HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_8)

typedef struct _Remote_Infrared_data_struct //定义红外线接收到的数据结构体类型
{
	uint8_t bKeyCodeNot; //地址反码
	uint8_t bKeyCode; //地址码
	uint8_t bIDNot; //命令反码
	uint8_t bID; //命令码
}Remote_Infrared_data_struct;

typedef union _Remote_Infrared_data_union //定义红外线接收到的数据结构体类型
{
	Remote_Infrared_data_struct RemoteInfraredDataStruct; 
	uint32_t uiRemoteInfraredData; 
}Remote_Infrared_data_union;

int fputc(int ch, FILE *f);
void Remote_Infrared_KEY_ISR(void);
uint8_t Remote_Infrared_KeyDeCode(void);
