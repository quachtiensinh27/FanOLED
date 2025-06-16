// ====== timer.c ======
#include "stm32f4xx.h"
#include "oled.h"
#include "pwm.h"
#include "led.h"
#include "adc.h"
#include "system.h"

extern volatile uint8_t countdown;
extern volatile uint8_t mode;
extern volatile uint8_t button_pressed;
extern volatile uint8_t oled_state;

void TIM3_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 15999;      // 16MHz / 16000 = 1kHz (1ms)
    TIM3->ARR = 100 - 1;    // 100ms
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void) {
    static uint8_t tick_500 = 0;
    static uint8_t tick_1000 = 0;

    TIM3->SR &= ~TIM_SR_UIF;

    // Gọi ADC mỗi 100ms
    ADC1->CR2 |= ADC_CR2_SWSTART;

    if (!button_pressed) {
        mode = Mode_Update_From_ADC();
    } else {
        button_pressed = 0;
    }

    if (oled_state == 1 || oled_state == 3) {
        Update_PWM_From_Mode(mode);
        LED_Update(mode, countdown);
    } else {
        TIM4->CCR2 = 0;
        LED_Update(0, 0);
    }

    tick_500++;
    tick_1000++;

    if (tick_500 >= 5) {
        tick_500 = 0;
        SSD1306_DisplayStatus(mode, countdown);
    }

    if (tick_1000 >= 10) {
        tick_1000 = 0;
        if (oled_state == 1 && countdown > 0 && mode != 0) {
            countdown--;
            if (countdown == 0) {
                oled_state = 0;
            }
        }
    }
}
