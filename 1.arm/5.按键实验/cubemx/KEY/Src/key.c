#include "key.h"
uint8_t Check_KEY_PRESS(void){
	uint8_t res = 0;
	/*有按键被按下*/
	if(GPIO_PIN_RESET == (HAL_GPIO_ReadPin(GPIOE, KEY0_PIN) & HAL_GPIO_ReadPin(GPIOE, KEY1_PIN) & HAL_GPIO_ReadPin(GPIOE, KEY2_PIN))){
		HAL_Delay(10);  /*消抖*/
		if(HAL_GPIO_ReadPin(GPIOE, KEY0_PIN) == GPIO_PIN_RESET) res |= KEY0_PRESS;
		while(HAL_GPIO_ReadPin(GPIOE, KEY0_PIN) == GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOE, KEY1_PIN) == GPIO_PIN_RESET) res |= KEY1_PRESS;
		while(HAL_GPIO_ReadPin(GPIOE, KEY1_PIN) == GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(GPIOE, KEY2_PIN) == GPIO_PIN_RESET) res |= KEY2_PRESS;
		while(HAL_GPIO_ReadPin(GPIOE, KEY2_PIN) == GPIO_PIN_RESET);
	}
	return res;
}
