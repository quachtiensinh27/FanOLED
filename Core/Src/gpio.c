/* ==========================================================================================
 * File        : gpio.c
 * Description : Khởi tạo toàn bộ GPIO cho hệ thống: LED, I2C, ADC, EXTI, PWM
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "gpio.h"


/* ========================== [II] FUNC FILE ============================ */

/**
 * @brief  Khởi tạo GPIO cho LED (PA1, PA2, PA3)
 *
 * Chân PA1: LED1
 * Chân PA2: LED2
 * Chân PA3: LED3
 * Mode: Output push-pull, tốc độ thấp, không dùng pull-up/pull-down.
 */
void GPIO_Init_LED(void) {
    // Bật clock cho port GPIOA
	RCC->AHB1ENR |= (1 << 0);

    // Cấu hình mode: PA1, PA2, PA3 thành output (MODER = 0b01)
    GPIOA->MODER &= ~((3 << (1 * 2)) | (3 << (2 * 2)) | (3 << (3 * 2)));  // Xóa bit cũ
    GPIOA->MODER |= (1 << (1 * 2)) | (1 << (2 * 2)) | (1 << (3 * 2));     // Set output mode

    // Output type: push-pull
    GPIOA->OTYPER &= ~((1 << 1) | (1 << 2) | (1 << 3));

    // Tốc độ output: low speed
    GPIOA->OSPEEDR &= ~((3 << (1 * 2)) | (3 << (2 * 2)) | (3 << (3 * 2)));
}

/**
 * @brief  Khởi tạo GPIO cho I2C1 (PB8: SCL, PB9: SDA)
 *
 * I2C dùng alternate function AF4. Output open-drain, có pull-up nội.
 */
void GPIO_Init_I2C(void) {
    // Bật clock cho port GPIOB
	RCC->AHB1ENR |= (1 << 1);

    // Cấu hình mode: Alternate Function (MODER = 0b10)
    GPIOB->MODER &= ~(0xF << (8 * 2));
    GPIOB->MODER |=  (0xA << (8 * 2));  // PB8, PB9 đều AF mode

    // Output type: open-drain (chuẩn I2C)
    GPIOB->OTYPER |= (0x3 << 8);

    // Tốc độ: very high speed (cải thiện timing rise/fall I2C)
    GPIOB->OSPEEDR |= (0xF << (8 * 2));

    // Pull-up: kích hoạt pull-up nội bộ cho SDA và SCL
    GPIOB->PUPDR |= (0x5 << (8 * 2));

    // Alternate Function 4 (AF4) cho I2C trên PB8 & PB9
    GPIOB->AFR[1] |= (0x44 << ((8 - 8) * 4));
}

/**
 * @brief  Khởi tạo GPIO cho ADC (PA0)
 *
 * PA0 được cấu hình chế độ analog input.
 */
void GPIO_Init_ADC(void) {
    // Bật clock cho port GPIOA
	RCC->AHB1ENR |= (1 << 0);

    // Cấu hình mode: analog (MODER = 0b11)
    GPIOA->MODER |= (3 << (0 * 2));
}

/**
 * @brief  Khởi tạo GPIO cho EXTI (PA6, PA7, PB0, PB1)
 *
 * Các chân ngắt ngoài hoạt động ở input mode + pull-up nội.
 */
void GPIO_Init_EXTI(void) {
    // Bật clock cho GPIOA & GPIOB
	RCC->AHB1ENR |= (1 << 0) | (1 << 1);

    // Cấu hình PA6, PA7 và PB0, PB1 là input (MODER = 0b00)
    GPIOA->MODER &= ~((3 << (6 * 2)) | (3 << (7 * 2)));
    GPIOB->MODER &= ~((3 << (0 * 2)) | (3 << (1 * 2)));

    // Kích hoạt pull-up cho các nút nhấn
    GPIOA->PUPDR |= (1 << (6 * 2)) | (1 << (7 * 2));
    GPIOB->PUPDR |= (1 << (0 * 2)) | (1 << (1 * 2));
}

/**
 * @brief  Khởi tạo GPIO cho PWM (PB7: TIM4_CH2)
 *
 * TIM4_CH2 dùng alternate function AF2.
 */
void GPIO_Init_PWM(void) {
    // Bật clock cho GPIOB
	RCC->AHB1ENR |= (1 << 1);

    // Cấu hình PB7 thành Alternate Function mode
    GPIOB->MODER &= ~(3 << (7 * 2));
    GPIOB->MODER |= (2 << (7 * 2));

    // Chọn AF2 cho TIM4_CH2
    GPIOB->AFR[0] |= (2 << (7 * 4));
}

/**
 * @brief  Khởi tạo toàn bộ GPIO hệ thống
 */
void GPIO_Init_All(void) {
    GPIO_Init_LED();
    GPIO_Init_I2C();
    GPIO_Init_ADC();
    GPIO_Init_EXTI();
    GPIO_Init_PWM();
}


/* =========================== [III] END FILE =========================== */
