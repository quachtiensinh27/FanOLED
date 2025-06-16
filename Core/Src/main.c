/* ==========================================================================================
 * File        : main.c
 * Description : Chương trình khởi tạo hệ thống điều khiển PWM + ADC + OLED + LED
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "stm32f4xx.h"    // CMSIS: định nghĩa thanh ghi STM32
#include "gpio.h"         // Tách riêng khởi tạo GPIO
#include "system.h"       // SysTick delay
#include "i2c.h"          // Giao tiếp I2C với OLED
#include "oled.h"         // Giao diện OLED SSD1306
#include "adc.h"          // Cấu hình ADC đọc mode
#include "pwm.h"          // Điều khiển PWM qua TIM4
#include "led.h"          // LED hiển thị trạng thái mode
#include "exti.h"         // Xử lý ngắt ngoài (nút nhấn)
#include "timer.h"        // Bộ điều khiển chu kỳ (TIM3)


/* ========================== [II] FUNC FILE ============================ */

int main(void)
{
    // Khởi tạo delay bằng SysTick (1ms)
    SysTick_Init();

    // Khởi tạo toàn bộ GPIO liên quan
    GPIO_Init_All();

    // Khởi tạo ngoại vi phần cứng
    I2C1_Init();        // OLED giao tiếp I2C1
    ADC_Init();         // Khởi tạo ADC1 đọc cảm biến/biến trở
    PWM_Init();         // Khởi tạo TIM4 PWM đầu ra
    GPIO_EXTI_Init();   // Cấu hình EXTI ngắt ngoài từ nút nhấn
    TIM3_Init();        // Khởi tạo timer 100ms điều khiển chính

    // Thiết lập trạng thái ban đầu của hệ thống
    mode = 1;               // Chế độ mặc định
    countdown = 0;          // Không đếm khi bắt đầu
    button_pressed = 0;     // Không có nút nào được nhấn
    oled_state = 3;         // Chế độ mặc định: INFINITE MODE
    system_active = 1;      // Hệ thống đang hoạt động

    // Hiển thị trạng thái khởi động trên OLED
    SSD1306_Clear();
    SSD1306_PrintTextCentered(3, "SYSTEM READY");
    Delay_ms(2000);         // Đợi 2 giây
    oled_state = 3;         // Sau khởi động, chuyển về INFINITE MODE

    // Vòng lặp chính: không làm gì, mọi logic xử lý trong interrupt
    while (1);
}


/* =========================== [III] END FILE =========================== */
