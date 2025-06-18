// ===============================
// ========== FILE INCLUDE =======
// ===============================

#include "stm32f4xx.h"   // Thư viện CMSIS định nghĩa thanh ghi và cấu trúc của STM32F4
#include "system.h"


// =======================================
// ========== FUNCTION DEFINITIONS =======
// =======================================

// Biến đếm số lần ngắt SysTick – tương ứng với số ms đã trôi qua kể từ lúc khởi động
volatile uint32_t system_tick = 0;


/**
 * @brief Khởi tạo timer SysTick để tạo ngắt mỗi 1ms
 *
 * Mục đích: dùng để tạo delay hoặc đo thời gian, mỗi lần ngắt sẽ tăng biến `system_tick`
 */
void SysTick_Init(void) {
    // Tải giá trị nạp lại (reload) cho bộ đếm: (16,000,000 / 1000) - 1 = 15999
    // Mỗi lần đếm đủ 15999 chu kỳ (tương ứng 1ms với clock 16 MHz), sẽ tạo ngắt
    SysTick->LOAD = (16000000 / 1000) - 1;

    // Reset giá trị hiện tại của SysTick counter về 0
    SysTick->VAL = 0;

    // Bật SysTick:
    // - Bit 2: CLKSOURCE = 1 => chọn clock hệ thống (HCLK = 16 MHz)
    // - Bit 1: TICKINT = 1 => cho phép tạo ngắt
    // - Bit 0: ENABLE = 1 => bắt đầu đếm
    SysTick->CTRL = (1 << 2) |  // CLKSOURCE = processor clock
                    (1 << 1) |  // TICKINT = enable interrupt
                    (1 << 0);   // ENABLE = enable counter
}


/**
 * @brief Hàm xử lý ngắt SysTick – được gọi tự động mỗi 1ms
 * Tác dụng: tăng biến đếm thời gian toàn cục `system_tick`
 */
void SysTick_Handler(void) {
    system_tick++;  // Cộng thêm 1 ms
}


/**
 * @brief Hàm tạo delay (chờ) theo đơn vị mili giây
 * @param ms Số mili giây cần chờ
 *
 * Cơ chế: ghi lại thời điểm bắt đầu, sau đó chờ đến khi chênh lệch đủ `ms` mili giây
 */
void Delay_ms(uint32_t ms) {
    uint32_t start = system_tick;  // Lưu lại thời điểm bắt đầu
    while ((system_tick - start) < ms);  // Bận chờ đến khi đủ thời gian
}


/**
 * @brief Trả về thời gian đã trôi qua kể từ khi bắt đầu chạy (đơn vị ms)
 *
 * Dùng trong các ứng dụng cần kiểm tra thời gian như đo timeout hoặc xác định sự kiện theo thời gian
 */
uint32_t GetTick(void) {
    return system_tick;
}


// =======================================
// ============= END FILE ================
// =======================================
