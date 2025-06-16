// ====== main.c ======
#include "stm32f4xx.h"
#include "system.h"
#include "i2c.h"
#include "oled.h"
#include "adc.h"
#include "pwm.h"
#include "led.h"
#include "exti.h"
#include "timer.h"

int main(void) {
    SysTick_Init();
    I2C1_Init();
    ADC_Init();
    PWM_Init();
    LED_Init();
    GPIO_EXTI_Init();
    TIM3_Init();

    mode = 1;
    countdown = 0;
    button_pressed = 0;
    oled_state = 3;
    system_active = 1;

    SSD1306_Clear();
    SSD1306_PrintTextCentered(3, "SYSTEM READY");
    Delay_ms(2000);
    oled_state = 3;

    while (1);
}
