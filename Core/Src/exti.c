// =================================
// ========== FILE INCLUDE =========
// =================================

#include "stm32f4xx.h"  // Thư viện CMSIS cho STM32F4
#include "exti.h"       // Header cho exti.c (khai báo GPIO_EXTI_Init, các IRQ handler)
#include "system.h"     // Hàm GetTick()

// Biến toàn cục được định nghĩa bên ngoài
volatile uint8_t countdown = 0;       // Bộ đếm thời gian (giây)
volatile uint8_t mode = 1;            // Chế độ hoạt động hiện tại
volatile uint8_t button_pressed = 0;  // Cờ nhấn nút
volatile uint8_t system_active = 1;   // Trạng thái hệ thống (ON/OFF)
volatile uint8_t oled_state = 3;      // Trạng thái hiển thị OLED


// ======================================
// ======== FUNCTION DEFINITIONS ========
// ======================================

/**
 * @brief Khởi tạo ngắt ngoài (EXTI) cho các chân:
 *        - PA6, PA7
 *        - PB0, PB1
 *        Tất cả được cấu hình để kích hoạt ngắt cạnh xuống.
 */
void GPIO_EXTI_Init(void) {
    // 1. Bật clock cho GPIOA và GPIOB
    RCC->AHB1ENR |= (1 << 0) | (1 << 1);  // GPIOAEN, GPIOBEN

    // 2. Bật clock cho SYSCFG để cấu hình EXTI
    RCC->APB2ENR |= (1 << 14);  // SYSCFGEN

    // 3. Thiết lập các chân PA6, PA7, PB0, PB1 là input (MODER = 00)
    GPIOA->MODER &= ~((3 << (6 * 2)) | (3 << (7 * 2)));
    GPIOB->MODER &= ~((3 << (0 * 2)) | (3 << (1 * 2)));

    // 4. Kích hoạt điện trở kéo lên (pull-up)
    GPIOA->PUPDR |= (1 << (6 * 2)) | (1 << (7 * 2));
    GPIOB->PUPDR |= (1 << (0 * 2)) | (1 << (1 * 2));

    // 5. Gán EXTI dòng 6, 7 cho chân PA6, PA7 (EXTICR[1])
    SYSCFG->EXTICR[1] &= ~((0xF << 8) | (0xF << 12));  // PA = 0000

    // 6. Gán EXTI dòng 0, 1 cho chân PB0, PB1 (EXTICR[0])
    SYSCFG->EXTICR[0] &= ~((0xF << 0) | (0xF << 4));
    SYSCFG->EXTICR[0] |= (1 << 0) | (1 << 4);          // PB = 0001

    // 7. Cho phép ngắt từ EXTI dòng 0,1,6,7
    EXTI->IMR |= (1 << 0) | (1 << 1) | (1 << 6) | (1 << 7);

    // 8. Kích hoạt ngắt cạnh xuống (falling edge)
    EXTI->FTSR |= (1 << 0) | (1 << 1) | (1 << 6) | (1 << 7);

    // 9. Kích hoạt ngắt trong NVIC
    NVIC_EnableIRQ(EXTI9_5_IRQn);  // PA6, PA7
    NVIC_EnableIRQ(EXTI0_IRQn);    // PB0
    NVIC_EnableIRQ(EXTI1_IRQn);    // PB1

    // 10. Thiết lập mức ưu tiên ngắt
    NVIC_SetPriority(EXTI9_5_IRQn, 0);  // Cao nhất
    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_SetPriority(EXTI1_IRQn, 1);
}


/**
 * @brief Xử lý sự kiện nhấn nút tại PA6 hoặc PA7
 *        - PA6: Bật/tắt hệ thống
 *        - PA7: Bắt đầu đếm lùi 10s
 */
void EXTI9_5_IRQHandler(void) {
    static uint32_t last_press_time = 0;
    uint32_t current_time = GetTick();

    // Chống dội nút (debounce) trong 50 ms
    if ((current_time - last_press_time) < 50) {
        EXTI->PR |= (1 << 6) | (1 << 7);  // Xóa cờ ngắt
        return;
    }

    // ==== Xử lý PA6: Tắt/Bật hệ thống ====
    if (EXTI->PR & (1 << 6)) {
        system_active ^= 1;  // Đảo trạng thái hệ thống

        if (!system_active) {
            countdown = 0;
            mode = 0;
            oled_state = 2;

            GPIOA->ODR &= ~((1 << 1) | (1 << 2) | (1 << 3));  // Tắt LED
            TIM4->CCR2 = 0;  // Dừng PWM

            // Tạm thời tắt các ngắt khác
            EXTI->IMR &= ~((1 << 7) | (1 << 0) | (1 << 1));
        } else {
            countdown = 0;
            mode = 1;
            oled_state = 3;

            // Cho phép lại các ngắt khác
            EXTI->IMR |= (1 << 7) | (1 << 0) | (1 << 1);
        }

        button_pressed = 1;
        EXTI->PR |= (1 << 6);  // Xóa cờ ngắt
    }

    // ==== Xử lý PA7: Đặt countdown = 10s ====
    if (EXTI->PR & (1 << 7)) {
        if (system_active) {
            countdown = 10;
            oled_state = 1;
            button_pressed = 1;
        }
        EXTI->PR |= (1 << 7);  // Xóa cờ ngắt
    }

    last_press_time = current_time;
}


/**
 * @brief Xử lý nhấn nút tại PB0 → Đặt countdown = 20s
 */
void EXTI0_IRQHandler(void) {
    static uint32_t last_press_time = 0;
    uint32_t current_time = GetTick();

    if ((current_time - last_press_time) > 50) {
        countdown = 20;
        oled_state = 1;
        button_pressed = 1;
        last_press_time = current_time;
    }

    EXTI->PR |= (1 << 0);  // Xóa cờ ngắt
}


/**
 * @brief Xử lý nhấn nút tại PB1 → Đặt countdown = 30s
 */
void EXTI1_IRQHandler(void) {
    static uint32_t last_press_time = 0;
    uint32_t current_time = GetTick();

    if ((current_time - last_press_time) > 50) {
        countdown = 30;
        oled_state = 1;
        button_pressed = 1;
        last_press_time = current_time;
    }

    EXTI->PR |= (1 << 1);  // Xóa cờ ngắt
}


// =======================================
// ============= END FILE ================
// =======================================
