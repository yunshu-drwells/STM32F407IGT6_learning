#include "led.h"
void LEDx_TogglePin(GPIO_TypeDef * GPIOx, uint16_t KEYx_PIN){
	/*��תLEDx״̬*/
	HAL_GPIO_TogglePin(GPIOx, KEYx_PIN);
}
