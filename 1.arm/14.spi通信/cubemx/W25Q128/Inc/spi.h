
#ifndef __SPI_H
#define __SPI_H

# include "stdint.h"

#include "stm32f4xx.h"
#include "core_cm4.h"
#include "stm32f4xx_hal.h"
# include "stm32f4xx_hal_spi.h"
# include "stm32f4xx_hal_def.h"


/******************************************************************************************/
/* SPI2 ���� ���� */

#define SPI2_SCK_GPIO_PORT              GPIOI
#define SPI2_SCK_GPIO_PIN               GPIO_PIN_1
#define SPI2_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)   /* PI��ʱ��ʹ�� */

#define SPI2_MISO_GPIO_PORT             GPIOI
#define SPI2_MISO_GPIO_PIN              GPIO_PIN_2
#define SPI2_MISO_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)   /* PI��ʱ��ʹ�� */

#define SPI2_MOSI_GPIO_PORT             GPIOI
#define SPI2_MOSI_GPIO_PIN              GPIO_PIN_3
#define SPI2_MOSI_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOI_CLK_ENABLE(); }while(0)   /* PI��ʱ��ʹ�� */

/* SPI2��ض��� */
#define SPI2_SPI                        SPI2
#define SPI2_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI2_CLK_ENABLE(); }while(0)    /* SPI2ʱ��ʹ�� */

/******************************************************************************************/


/* SPI�����ٶ����� */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7


void spi2_init(void);
void spi2_set_speed(uint8_t speed);
uint8_t spi2_read_write_byte(uint8_t txdata);

#endif
























