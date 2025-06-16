/* ==========================================================================================
 * File        : timer.c
 * Description : Khởi tạo và điều khiển bộ định thời TIM3 quản lý toàn bộ hệ thống
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "stm32f4xx.h"
#include "oled.h"
#include "pwm.h"
#include "led.h"
#include "adc.h"
#include "system.h"


/* ========================== [II] FUNC FILE ============================ */

// Các biến trạng thái toàn cục được chia sẻ từ module khác
extern volatile uint8_t countdown;
extern volatile uint8_t mode;
extern volatile uint8_t button_pressed;
extern volatile uint8_t oled_state;


/**
 * @brief  Khởi tạo Timer 3 với chu kỳ 100ms làm bộ điều khiển trung tâm
 *
 * - Timer input clock: 16MHz (APB1 timer clock x2)
 * - Prescaler: 15999 → 16MHz / (15999+1) = 1kHz (1ms tick)
 * - Auto-reload (ARR): 100 → tạo ra chu kỳ 100ms
 */
void TIM3_Init(void) {
    // Bật clock cho TIM3 (trên bus APB1)
	RCC->APB1ENR |= (1 << 1);

    // Chia tần số: 16MHz → 1kHz
    TIM3->PSC = 15999;

    // Cấu hình chu kỳ đếm: 100ms (ARR = 100-1)
    TIM3->ARR = 100 - 1;

    // Bật ngắt update interrupt
    TIM3->DIER |= (1 << 0);

    // Bắt đầu đếm
    TIM3->CR1 |= (1 << 0);

    // Bật ngắt TIM3 trong NVIC
    NVIC_EnableIRQ(TIM3_IRQn);
}


/**
 * @brief  Ngắt định kỳ mỗi 100ms - điều khiển toàn bộ logic hệ thống
 *
 * Thực hiện:
 * - Gửi lệnh ADC sampling
 * - Đọc mode từ ADC (nếu không có nút nhấn)
 * - Điều chỉnh PWM, LED theo mode
 * - Cập nhật OLED mỗi 500ms
 * - Giảm countdown mỗi 1000ms
 */
void TIM3_IRQHandler(void) {
    static uint8_t tick_500 = 0;   // Đếm nội bộ 500ms
    static uint8_t tick_1000 = 0;  // Đếm nội bộ 1000ms

    // Xóa cờ ngắt update (UIF)
    TIM3->SR &= ~(1 << 0);

    /** 1. Kích hoạt ADC bắt đầu chuyển đổi **/
    ADC1->CR2 |= (1 << 30);

    /** 2. Cập nhật mode từ ADC nếu không có nút nhấn **/
    if (!button_pressed) {
        mode = Mode_Update_From_ADC();
    } else {
        button_pressed = 0;  // Reset cờ sau khi xử lý nút
    }

    /** 3. Cập nhật PWM và LED theo oled_state **/
    if (oled_state == 1 || oled_state == 3) {
        Update_PWM_From_Mode(mode);
        LED_Update(mode, countdown);
    } else {
        TIM4->CCR2 = 0;           // Tắt PWM khi STOP
        LED_Update(0, 0);         // Tắt toàn bộ LED
    }

    /** 4. Đếm tick cho các chu kỳ lớn hơn **/
    tick_500++;
    tick_1000++;

    /** 5. Cập nhật OLED mỗi 500ms **/
    if (tick_500 >= 5) {
        tick_500 = 0;
        SSD1306_DisplayStatus(mode, countdown);
    }

    /** 6. Giảm countdown mỗi 1000ms **/
    if (tick_1000 >= 10) {
        tick_1000 = 0;

        if (oled_state == 1 && countdown > 0 && mode != 0) {
            countdown--;

            if (countdown == 0) {
                oled_state = 0;  // Hết thời gian → dừng
            }
        }
    }
}


/* =========================== [III] END FILE =========================== */
