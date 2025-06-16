/* ==========================================================================================
 * File        : pwm.c
 * Description : Khởi tạo và điều khiển PWM bằng TIM4 Channel 2 (PB7)
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "stm32f4xx.h"
#include "pwm.h"


/* ========================== [II] FUNC FILE ============================ */

/**
 * @brief  Khởi tạo PWM sử dụng Timer 4, Channel 2 (output: PB7)
 *
 * - Tần số Timer gốc: 16MHz (APB1 clock)
 * - Prescaler: 1599  → f_timer = 16MHz / (1599 + 1) = 10kHz
 * - Auto-reload (ARR): 100 → Chu kỳ PWM = 100 counts (10kHz / 100 = 100Hz)
 * - PWM mode 1 (active high)
 */
void PWM_Init(void) {
    // Bật clock cho TIM4 (trên bus APB1)
	RCC->APB1ENR |= (1 << 2);

    // Cấu hình bộ chia tần số (Prescaler)
    TIM4->PSC = 1599;  // Chia từ 16MHz về 10kHz

    // Cấu hình chu kỳ PWM (ARR = period)
    TIM4->ARR = 100;   // PWM frequency ~100Hz

    // Thiết lập giá trị duty cycle ban đầu (30%)
    TIM4->CCR2 = 30;

    // Cấu hình PWM Mode 1 trên kênh 2 (CCMR1 → OC2M = 110b)
    TIM4->CCMR1 &= ~(7 << 12); // Xóa các bit OC2M trước
    TIM4->CCMR1 |= (6 << 12); // PWM mode 1 (active high)

    // Cho phép preload cho CCR2 (enable preload buffer)
    TIM4->CCMR1 |= (1 << 11);

    // Bật kênh output compare 2 (enable output compare)
    TIM4->CCER |= (1 << 4);

    // Bật Timer 4 (counter enable)
    TIM4->CR1 |= (1 << 0);
}


/**
 * @brief  Cập nhật duty cycle PWM theo mode hệ thống
 *
 * Mapping mode:
 * - Mode 3 → 100% duty (CCR2 = 100)
 * - Mode 2 → 80% duty  (CCR2 = 80)
 * - Mode 1 → 60% duty  (CCR2 = 60)
 * - Mode 0 → 0% duty (tắt PWM)
 *
 * @param mode  Chế độ điều khiển (0~3)
 */
void Update_PWM_From_Mode(uint8_t mode) {
    switch (mode) {
        case 3: TIM4->CCR2 = 100; break;  // 100% tốc độ
        case 2: TIM4->CCR2 = 80;  break;
        case 1: TIM4->CCR2 = 60;  break;
        case 0: TIM4->CCR2 = 0;   break;  // Dừng
        default: TIM4->CCR2 = 0;  break;  // Trường hợp lỗi
    }
}


/* =========================== [III] END FILE =========================== */
