// =================================
// ========== FILE INCLUDE =========
// =================================

#include "stm32f4xx.h"
#include "system.h"    // SysTick, Delay_ms, GetTick
#include "i2c.h"       // Giao tiếp I2C
#include "oled.h"      // OLED hiển thị
#include "adc.h"       // Đọc ADC điều chỉnh mode
#include "pwm.h"       // PWM output theo mode
#include "led.h"       // LED hiển thị mode
#include "exti.h"      // Ngắt ngoài từ nút nhấn

// Biến toàn cục được định nghĩa bên ngoài
extern volatile uint8_t mode;
extern volatile uint8_t countdown;
extern volatile uint8_t button_pressed;


// ======================================
// ======== FUNCTION DEFINITIONS ========
// ======================================

/**
 * @brief Hàm main – khởi tạo hệ thống và xử lý vòng lặp chính
 */
int main(void) {
    // ======== Khởi tạo toàn bộ ngoại vi ========
    SysTick_Init();        // Delay + GetTick
    I2C1_Init();           // Giao tiếp OLED
    ADC_Init();            // Đọc biến trở
    PWM_Init();            // PWM qua TIM4
    LED_Init();            // PA1, PA2, PA3
    GPIO_EXTI_Init();      // Ngắt ngoài từ nút nhấn

    // ======== Hiển thị khởi động ban đầu ========
    SSD1306_Clear();
    SSD1306_PrintTextCentered(3, "SYSTEM READY");
    Delay_ms(2000);
    oled_state = 3;  // Chuyển sang trạng thái "INFINITE"

    // ======== Biến thời gian ========
    uint32_t last_update = 0;       // Cập nhật PWM/LED
    uint32_t last_countdown = 0;    // Giảm countdown
    uint32_t last_display = 0;      // Cập nhật OLED

    // ======== Vòng lặp chính ========
    while (1) {
        uint32_t current_time = GetTick();

        // ✅ Cập nhật OLED mỗi 500ms, luôn hiển thị kể cả khi hệ thống bị tắt
        if ((current_time - last_display) >= 500) {
            switch (oled_state) {
                case 0: // READY
                    SSD1306_Clear();
                    SSD1306_PrintTextCentered(3, "SYSTEM READY");
                    break;
                case 1: // COUNTDOWN
                    SSD1306_DisplayStatus(mode, countdown);
                    break;
                case 2: // SYSTEM STOPPED
                    SSD1306_Clear();
                    SSD1306_PrintTextCentered(3, "SYSTEM STOPPED");
                    break;
                case 3: // INFINITE MODE
                    SSD1306_Clear();
                    SSD1306_PrintTextCentered(2, "TIME: INF");

                    char mode_str[16];
                    sprintf(mode_str, "MODE: %d", mode);
                    SSD1306_PrintTextCentered(4, mode_str);
                    break;
            }
            last_display = current_time;
        }

        // ❌ Nếu hệ thống đang bị tắt, bỏ qua toàn bộ xử lý logic
        if (!system_active) {
            Delay_ms(10);
            continue;
        }

        // ✅ Cập nhật PWM và LED mỗi 100ms
        if ((current_time - last_update) >= 100) {
            if (!button_pressed) {
                // Đọc từ ADC để cập nhật mode
                mode = Mode_Update_From_ADC();
            } else {
                button_pressed = 0; // Đã xử lý nút nhấn
            }

            // Cập nhật PWM và LED tương ứng nếu OLED đang hiển thị trạng thái hợp lệ
            if (oled_state == 1 || oled_state == 3) {
                Update_PWM_From_Mode(mode);
                LED_Update(mode);
            } else {
                // Dừng PWM và tắt LED nếu không ở trạng thái active
                TIM4->CCR2 = 0;
                LED_Update(0);
            }

            last_update = current_time;
        }

        // ✅ Xử lý countdown mỗi 1000ms
        if ((current_time - last_countdown) >= 1000) {
            if (oled_state == 1 && countdown > 0 && mode != 0) {
                countdown--;
                if (countdown == 0) {
                    oled_state = 0; // Trở lại trạng thái "READY"
                }
            }
            last_countdown = current_time;
        }

        // Delay nhỏ để giảm tần suất vòng lặp
        Delay_ms(10);
    }
}


// =================================
// =========== END FILE ============
// =================================
