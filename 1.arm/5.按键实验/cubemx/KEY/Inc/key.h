#include "stm32f4xx_hal.h"
/*KEYx PIN*/
#define KEY0_PIN GPIO_PIN_2
#define KEY1_PIN GPIO_PIN_3
#define KEY2_PIN GPIO_PIN_4
/*KEY PRESS value*/
#define KEY0_PRESS 1
#define KEY1_PRESS 2
#define KEY2_PRESS 4
/* check the key status*/
uint8_t Check_KEY_PRESS(void);
