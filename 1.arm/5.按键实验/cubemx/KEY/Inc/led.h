#include "stm32f4xx_hal.h"
/*LEDx PIN*/
#define LED0_PIN GPIO_PIN_0
#define LED1_PIN GPIO_PIN_1
void LEDx_TogglePin(GPIO_TypeDef * GPIOx, uint16_t KEYx_PIN);
