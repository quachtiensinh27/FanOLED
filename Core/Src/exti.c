/* ==========================================================================================
 * File        : exti.c
 * Description : Cấu hình và xử lý các ngắt ngoài từ nút nhấn (PA6, PA7, PB0, PB1)
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "stm32f4xx.h"
#include "exti.h"
#include "system.h"


/* ========================== [II] FUNC FILE ============================ */

// Biến toàn cục điều khiển trạng thái hệ thống
volatile uint8_t countdown = 0;        // Giá trị thời gian đếm ngược
volatile uint8_t mode = 1;             // Mode hoạt động hiện tại
volatile uint8_t button_pressed = 0;   // Ghi nhận nút đã nhấn
volatile uint8_t system_active = 1;    // 1: đang hoạt động, 0: đã dừng
volatile uint8_t oled_state = 3;       // Trạng thái hiển thị OLED


/**
 * @brief  Cấu hình ngắt ngoài EXTI cho các nút nhấn
 *
 * Các chân sử dụng:
 * - PA6: nút ON/OFF hệ thống
 * - PA7: nút đếm ngược 10s
 * - PB0: nút đếm ngược 20s
 * - PB1: nút đếm ngược 30s
 *
 * Kích hoạt falling edge, mask ngắt, map EXTI đến GPIO tương ứng.
 */
void GPIO_EXTI_Init(void) {
    // Bật clock cho SYSCFG (để cấu hình EXTI)
	RCC->APB2ENR |= (1 << 14);

    // Map EXTI line đến GPIO
	SYSCFG->EXTICR[1] &= ~((0xF << 8) | (0xF << 12));
	SYSCFG->EXTICR[0] |= (1 << 0) | (1 << 4);

    // Unmask EXTI lines: Cho phép ngắt tại các chân 6, 7, 0, 1
    EXTI->IMR |= (1 << 6) | (1 << 7) | (1 << 0) | (1 << 1);

    // Kích hoạt falling edge trigger cho EXTI lines
    EXTI->FTSR |= (1 << 6) | (1 << 7) | (1 << 0) | (1 << 1);

    // Bật ngắt trong NVIC
    NVIC_EnableIRQ(EXTI9_5_IRQn);   // PA6, PA7 nằm trong EXTI9_5
    NVIC_EnableIRQ(EXTI0_IRQn);     // PB0
    NVIC_EnableIRQ(EXTI1_IRQn);     // PB1
}


/**
 * @brief  Xử lý ngắt EXTI cho PA6 (ON/OFF) và PA7 (Countdown 10s)
 */
void EXTI9_5_IRQHandler(void) {
    static uint32_t last_press_time = 0;
    uint32_t current_time = GetTick();

    // Chống dội phím phần mềm: bỏ qua nếu thời gian nhấn quá gần (<50ms)
    if ((current_time - last_press_time) < 50) {
        EXTI->PR |= (1 << 6) | (1 << 7);  // Xóa cờ ngắt
        return;
    }

    // ==== PA6: Nút ON/OFF hệ thống ====
    if (EXTI->PR & (1 << 6)) {
        system_active ^= 1;  // Đảo trạng thái hệ thống

        if (!system_active) {
            // Tắt hệ thống: dừng countdown, mode về 0, tắt LED, báo STOPPED
            countdown = 0;
            mode = 0;
            GPIOA->ODR &= ~((1 << 1) | (1 << 2) | (1 << 3));
            oled_state = 2;  // STOPPED
        } else {
            // Bật lại hệ thống: reset mode, countdown, báo INFINITE
            countdown = 0;
            mode = 1;
            oled_state = 3;  // INFINITE
        }

        button_pressed = 1;
        EXTI->PR |= (1 << 6);  // Clear cờ ngắt
    }

    // ==== PA7: Nút COUNTDOWN 10s ====
    if (EXTI->PR & (1 << 7)) {
        countdown = 10;
        oled_state = 1;        // COUNTDOWN mode
        button_pressed = 1;
        EXTI->PR |= (1 << 7);  // Clear cờ ngắt
    }

    last_press_time = current_time;
}


/**
 * @brief  Xử lý ngắt EXTI cho PB0 (COUNTDOWN 20s)
 */
void EXTI0_IRQHandler(void) {
    static uint32_t last_press_time = 0;
    uint32_t current_time = GetTick();

    if ((current_time - last_press_time) > 50) {
        countdown = 20;
        oled_state = 1;         // COUNTDOWN mode
        button_pressed = 1;
        last_press_time = current_time;
    }

    EXTI->PR |= (1 << 0);       // Clear cờ ngắt
}


/**
 * @brief  Xử lý ngắt EXTI cho PB1 (COUNTDOWN 30s)
 */
void EXTI1_IRQHandler(void) {
    static uint32_t last_press_time = 0;
    uint32_t current_time = GetTick();

    if ((current_time - last_press_time) > 50) {
        countdown = 30;
        oled_state = 1;         // COUNTDOWN mode
        button_pressed = 1;
        last_press_time = current_time;
    }

    EXTI->PR |= (1 << 1);       // Clear cờ ngắt
}


/* =========================== [III] END FILE =========================== */
