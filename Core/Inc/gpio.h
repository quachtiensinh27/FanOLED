#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f4xx.h"

void GPIO_Init_All(void);
void GPIO_Init_LED(void);
void GPIO_Init_I2C(void);
void GPIO_Init_ADC(void);
void GPIO_Init_EXTI(void);
void GPIO_Init_PWM(void);

#endif
