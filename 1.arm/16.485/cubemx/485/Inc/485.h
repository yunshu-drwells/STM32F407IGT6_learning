#include "stm32f4xx_hal.h"

/* RS485 引脚 和 串口 定义 */
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

/* 控制RS485_RE脚, 控制RS485发送/接收状态
 * RS485_RE = 1, 进入接收模式
 * RS485_RE = 0, 进入发送模式
 */
#define RS485_EN_RX                 1           /* 使能（1）/禁止（0）RS485接收 */


#define RS485_REC_LEN               64          /* 定义最大接收字节数 64 */

void rs485_init(uint32_t baudrate);
void rs485_send_data(uint8_t *buf, uint8_t len);
//void RS485_UX_IRQHandler(void);
void USART3_IRQHandler(void);
void rs485_receive_data(uint8_t *buf, uint8_t *len);
