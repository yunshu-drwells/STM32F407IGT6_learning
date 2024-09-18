#include "stm32f4xx_hal.h"

/* RS485 ���� �� ���� ���� */
#define RS485_TX_GPIO_PORT GPIOB
#define RS485_TX_GPIO_PIN GPIO_PIN_10
#define RS485_TX_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)
#define RS485_RX_GPIO_PORT GPIOB
#define RS485_RX_GPIO_PIN GPIO_PIN_11
#define RS485_RX_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)
#define RS485_UX USART3
#define RS485_UX_IRQn USART3_IRQn
//#define RS485_UX_IRQHandler USART3_IRQHandler
#define RS485_UX_CLK_ENABLE() do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0)

/* ����RS485_RE��, ����RS485����/����״̬
 * RS485_RE = 1, �������ģʽ
 * RS485_RE = 0, ���뷢��ģʽ
 */
#define RS485_EN_RX                 1           /* ʹ�ܣ�1��/��ֹ��0��RS485���� */


#define RS485_REC_LEN               64          /* �����������ֽ��� 64 */

void rs485_init(uint32_t baudrate);
void rs485_send_data(uint8_t *buf, uint8_t len);
//void RS485_UX_IRQHandler(void);
void USART3_IRQHandler(void);
void rs485_receive_data(uint8_t *buf, uint8_t *len);
