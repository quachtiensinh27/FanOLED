// ====== led.c ======
#include "stm32f4xx.h"
#include "led.h"

extern volatile uint8_t oled_state;

void LED_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER &= ~((3 << (1 * 2)) | (3 << (2 * 2)) | (3 << (3 * 2)));
    GPIOA->MODER |= (1 << (1 * 2)) | (1 << (2 * 2)) | (1 << (3 * 2));
    GPIOA->OTYPER &= ~((1 << 1) | (1 << 2) | (1 << 3));
    GPIOA->OSPEEDR &= ~((3 << (1 * 2)) | (3 << (2 * 2)) | (3 << (3 * 2)));
}

void LED_Update(uint8_t current_mode, uint8_t countdown) {
    GPIOA->ODR &= ~((1 << 1) | (1 << 2) | (1 << 3));

    // Nếu đang countdown hoặc ở infinite mode thì cho phép sáng
    if (countdown == 0 && oled_state != 3) return;

    switch (current_mode) {
        case 1: GPIOA->ODR |= (1 << 1); break;
        case 2: GPIOA->ODR |= (1 << 2); break;
        case 3: GPIOA->ODR |= (1 << 3); break;
        default: break;
    }
}

