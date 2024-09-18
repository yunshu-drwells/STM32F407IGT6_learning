#include "stm32f4xx_hal.h"
/*GPIPx*/
#define GPIO_GROUP GPIOE

/*KEYx PIN*/
#define KEY0_PIN GPIO_PIN_2
#define KEY1_PIN GPIO_PIN_3
#define KEY2_PIN GPIO_PIN_4

/*KEY PRESS value*/
#define KEY0_PRESS 1
#define KEY1_PRESS 2
#define KEY2_PRESS 4

#define KEY0        HAL_GPIO_ReadPin(GPIO_GROUP, KEY0_PIN)     /* 读取KEY0引脚 */
#define KEY1        HAL_GPIO_ReadPin(GPIO_GROUP, KEY1_PIN)     /* 读取KEY1引脚 */
#define KEY2        HAL_GPIO_ReadPin(GPIO_GROUP, KEY2_PIN)     /* 读取KEY2引脚 */

/* check the key status*/
uint8_t Check_KEY_PRESS(void);
