// =================================
// ========== FILE INCLUDE =========
// =================================

#include "stm32f4xx.h"   // Thư viện CMSIS của STM32F4
#include "led.h"         // Header cho led.c (khai báo LED_Init, LED_Update)


// ====================================
// ======== FUNCTION DEFINITIONS ======
// ====================================


/**
 * @brief Khởi tạo các chân LED (PA1, PA2, PA3) làm output push-pull
 *        Dùng để điều khiển LED qua thanh ghi GPIOA
 */
void LED_Init(void) {
    // Bật clock cho GPIOA (bit 0 của RCC->AHB1ENR)
    RCC->AHB1ENR |= (1 << 0); // GPIOAEN

    // Thiết lập PA1, PA2, PA3 là output mode (MODER = 01)
    GPIOA->MODER &= ~((3 << (1 * 2)) | (3 << (2 * 2)) | (3 << (3 * 2))); // Xóa trước
    GPIOA->MODER |=  (1 << (1 * 2)) | (1 << (2 * 2)) | (1 << (3 * 2));   // Đặt lại = 01

    // Thiết lập kiểu output là push-pull (OTYPER = 0)
    GPIOA->OTYPER &= ~((1 << 1) | (1 << 2) | (1 << 3));
}


/**
 * @brief Cập nhật trạng thái LED theo chế độ hiện tại
 *        Chỉ bật 1 trong 3 LED tùy theo current_mode
 *
 * @param current_mode Chế độ hiện tại (1: LED1, 2: LED2, 3: LED3)
 */
void LED_Update(uint8_t current_mode) {
    // Tắt tất cả LED (clear bit PA1, PA2, PA3)
    GPIOA->ODR &= ~((1 << 1) | (1 << 2) | (1 << 3));

    // Bật LED tương ứng với chế độ
    switch (current_mode) {
        case 1: GPIOA->ODR |= (1 << 1); break;  // Bật LED1 (PA1)
        case 2: GPIOA->ODR |= (1 << 2); break;  // Bật LED2 (PA2)
        case 3: GPIOA->ODR |= (1 << 3); break;  // Bật LED3 (PA3)
        default: break;  // Không bật LED nào nếu mode không hợp lệ
    }
}


// =================================
// =========== END FILE ===========
// =================================
