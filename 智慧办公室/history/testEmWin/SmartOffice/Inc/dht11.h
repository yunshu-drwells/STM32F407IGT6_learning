#include "stm32f4xx_hal.h"
/* DHT11 引脚 定义 */ 
#define DHT11_DQ_GPIO_PORT GPIOG 
#define DHT11_DQ_GPIO_PIN GPIO_PIN_15
#define DHT11_DQ_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0) /* PG口时钟使能 */ 

/* IO操作函数 */ 
#define DHT11_DQ_OUT(x) do{ x ? \
HAL_GPIO_WritePin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN, GPIO_PIN_SET) : \
HAL_GPIO_WritePin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN, GPIO_PIN_RESET); \
}while(0) /* 数据端口输出 */ 

/* 数据端口输入 */ 
#define DHT11_DQ_IN HAL_GPIO_ReadPin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN)

uint8_t dht11_init(void);
static void dht11_reset(void);
uint8_t dht11_check(void);
uint8_t dht11_read_bit(void);
static uint8_t dht11_read_byte(void);
uint8_t dht11_read_data(uint16_t *temp, uint16_t *humi);
