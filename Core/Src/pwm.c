// ===============================
// ========== FILE INCLUDE =======
// ===============================

#include "stm32f4xx.h"   // Thư viện CMSIS cho STM32F4
#include "pwm.h"         // Header cho pwm.c (khai báo PWM_Init, Update_PWM_From_Mode)


// =======================================
// ========== FUNCTION DEFINITIONS =======
// =======================================

/**
 * @brief Khởi tạo TIM4 kênh 2 để tạo tín hiệu PWM trên chân PB7
 *
 * TIM4_CH2 được ánh xạ với PB7 (Alternate Function 2 - AF2)
 * Tần số PWM được tính bởi:
 *     f_PWM = f_APB1 / ((PSC + 1) * (ARR + 1))
 * Với f_APB1 = 16 MHz, PSC = 1599, ARR = 100 → f_PWM ≈ 100 Hz
 */
void PWM_Init(void) {
    // 1. Bật clock cho GPIOB (PB7)
    RCC->AHB1ENR |= (1 << 1);  // GPIOBEN = 1

    // 2. Bật clock cho TIM4 (trên bus APB1)
    RCC->APB1ENR |= (1 << 2);  // TIM4EN = 1

    // 3. Cấu hình PB7 ở chế độ Alternate Function
    GPIOB->MODER &= ~(3 << (7 * 2));   // Xóa 2 bit MODER7
    GPIOB->MODER |=  (2 << (7 * 2));   // MODER7 = 10 (AF mode)

    // 4. Gán chức năng AF2 (TIM4_CH2) cho PB7
    GPIOB->AFR[0] &= ~(0xF << (7 * 4)); // Xóa trước
    GPIOB->AFR[0] |=  (2 << (7 * 4));   // AF2 cho PB7

    // 5. Cấu hình bộ định thời TIM4
    TIM4->PSC = 1599;  // Prescaler: f_TIM = f_APB1 / (PSC + 1)
    TIM4->ARR = 100;   // Auto-reload value: xác định chu kỳ PWM
    TIM4->CCR2 = 0;    // Giá trị khởi đầu cho duty cycle = 0%

    // 6. Cấu hình chế độ PWM mode 1 cho kênh 2
    TIM4->CCMR1 &= ~(7 << 12);  // Xóa OC2M
    TIM4->CCMR1 |=  (6 << 12);  // OC2M = 110 → PWM mode 1

    // 7. Kích hoạt preload cho CCR2 (đồng bộ hóa cập nhật)
    TIM4->CCMR1 |= (1 << 11);   // OC2PE = 1

    // 8. Cho phép kênh 2 xuất tín hiệu PWM ra chân PB7
    TIM4->CCER |= (1 << 4);     // CC2E = 1

    // 9. Bật bộ đếm TIM4 để bắt đầu hoạt động
    TIM4->CR1 |= (1 << 0);      // CEN = 1
}


/**
 * @brief Cập nhật độ rộng xung PWM theo chế độ (mode)
 *
 * @param mode Giá trị từ 0 đến 3, ứng với mức độ duty cycle:
 *             - 0: 0% (tắt)
 *             - 1: 40%
 *             - 2: 70%
 *             - 3: 100%
 */
void Update_PWM_From_Mode(uint8_t mode) {
    switch (mode) {
        case 3: TIM4->CCR2 = 100; break;  // 100% duty
        case 2: TIM4->CCR2 = 70;  break;  // 70%
        case 1: TIM4->CCR2 = 40;  break;  // 40%
        case 0: TIM4->CCR2 = 0;   break;  // 0% duty (OFF)
        default: TIM4->CCR2 = 0;  break;  // Giá trị không hợp lệ → OFF
    }
}


// =======================================
// ============= END FILE ================
// =======================================
