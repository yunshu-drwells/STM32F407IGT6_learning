#include "stm32f4xx_hal.h"

#define	Remote_Infrared_DAT_INPUT HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_8)

typedef struct _Remote_Infrared_data_struct //��������߽��յ������ݽṹ������
{
	uint8_t bKeyCodeNot; //��ַ����
	uint8_t bKeyCode; //��ַ��
	uint8_t bIDNot; //�����
	uint8_t bID; //������
}Remote_Infrared_data_struct;

typedef union _Remote_Infrared_data_union //��������߽��յ������ݽṹ������
{
	Remote_Infrared_data_struct RemoteInfraredDataStruct; 
	uint32_t uiRemoteInfraredData; 
}Remote_Infrared_data_union;

int fputc(int ch, FILE *f);
void Remote_Infrared_KEY_ISR(void);
uint8_t Remote_Infrared_KeyDeCode(void);
