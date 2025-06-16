/* ==========================================================================================
 * File        : led.c
 * Description : Điều khiển bật tắt LED trạng thái theo mode và countdown
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "stm32f4xx.h"
#include "led.h"


/* ========================== [II] FUNC FILE ============================ */

// Biến toàn cục dùng chung từ các module khác (oled_state)
extern volatile uint8_t oled_state;

/**
 * @brief  Cập nhật trạng thái LED theo mode và countdown
 *
 * LED sử dụng:
 * - PA1: LED cho mode 1
 * - PA2: LED cho mode 2
 * - PA3: LED cho mode 3
 *
 * Logic:
 * - Nếu đang đếm ngược (countdown > 0) hoặc ở chế độ INFINITE (oled_state == 3)
 *   thì LED mới được phép bật theo mode hiện tại.
 * - Nếu không thì tắt toàn bộ LED.
 *
 * @param  current_mode  Mode hiện tại (0~3)
 * @param  countdown     Thời gian còn lại (s)
 */
void LED_Update(uint8_t current_mode, uint8_t countdown) {
    // Tắt toàn bộ LED trước mỗi lần cập nhật (reset bit PA1, PA2, PA3)
    GPIOA->ODR &= ~((1 << 1) | (1 << 2) | (1 << 3));

    // Nếu không trong chế độ sáng LED thì thoát
    if (countdown == 0 && oled_state != 3) return;

    // Bật LED tương ứng với mode
    switch (current_mode) {
        case 1: GPIOA->ODR |= (1 << 1); break;  // Mode 1 → LED1 (PA1)
        case 2: GPIOA->ODR |= (1 << 2); break;  // Mode 2 → LED2 (PA2)
        case 3: GPIOA->ODR |= (1 << 3); break;  // Mode 3 → LED3 (PA3)
        default: break;                         // Mode 0: không bật LED
    }
}


/* =========================== [III] END FILE =========================== */
