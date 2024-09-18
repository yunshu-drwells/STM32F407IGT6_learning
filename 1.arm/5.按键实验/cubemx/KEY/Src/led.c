#include "led.h"
void LEDx_TogglePin(GPIO_TypeDef * GPIOx, uint16_t KEYx_PIN){
	/*·­×ªLEDx×´Ì¬*/
	HAL_GPIO_TogglePin(GPIOx, KEYx_PIN);
}
