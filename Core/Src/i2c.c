// ===============================
// ========== FILE INCLUDE =======
// ===============================

#include "stm32f4xx.h"  // Thư viện CMSIS cho dòng STM32F4
#include "i2c.h"        // Header riêng cho mô-đun I2C


// Biến toàn cục được định nghĩa bên ngoài
#define I2C_TIMEOUT 100000  // Số lần chờ tối đa trong các vòng while kiểm tra cờ


// =======================================
// ========== FUNCTION DEFINITIONS =======
// =======================================


/**
 * @brief Khởi tạo I2C1 ở chế độ chuẩn (Standard mode - 100kHz)
 *        Sử dụng các chân PB8 (SCL) và PB9 (SDA)
 */
void I2C1_Init(void) {
    // 1. Bật clock cho GPIOB (chân PB8, PB9 dùng cho I2C)
    RCC->AHB1ENR |= (1 << 1);  // GPIOBEN = 1

    // 2. Bật clock cho I2C1 (trên bus APB1)
    RCC->APB1ENR |= (1 << 21); // I2C1EN = 1

    // 3. Đặt chế độ Alternate Function (AF) cho PB8 và PB9
    GPIOB->MODER &= ~(0xF << (8 * 2));        // Clear MODER8 & MODER9
    GPIOB->MODER |=  (0xA << (8 * 2));        // MODER = 10 (AF mode)

    // 4. Đặt kiểu output là Open-Drain (bắt buộc với I2C)
    GPIOB->OTYPER |= (0x3 << 8);              // OTYPER8 & 9 = 1

    // 5. Đặt tốc độ rất cao cho hai chân này
    GPIOB->OSPEEDR |= (0xF << (8 * 2));       // OSPEEDR = 11 (very high speed)

    // 6. Kích hoạt Pull-up nội để tránh trạng thái floating
    GPIOB->PUPDR |= (0x5 << (8 * 2));         // PUPDR = 01 (pull-up)

    // 7. Gán AF4 (I2C1) cho PB8 và PB9
    GPIOB->AFR[1] |= (0x44 << 0);             // PB8/9 → AF4 (I2C1)

    // 8. Tắt I2C trước khi cấu hình (PE = 0)
    I2C1->CR1 &= ~(1 << 0);  // Disable I2C1

    // 9. Thiết lập CR2 = tốc độ bus APB1 (ở đây giả định = 16MHz)
    I2C1->CR2 = 16;

    // 10. Cấu hình tốc độ chuẩn 100kHz (Standard Mode)
    I2C1->CCR = 80; // CCR = Fpclk / (2 * I2C_speed) = 16MHz / (2*100kHz) = 80

    // 11. Thiết lập TRISE = Fpclk + 1 (theo datasheet)
    I2C1->TRISE = 17;

    // 12. Bật lại I2C1 (PE = 1)
    I2C1->CR1 |= (1 << 0);  // Enable I2C1
}


/**
 * @brief Gửi 1 byte đến 1 thiết bị I2C (giao thức Write)
 *
 * @param addr Địa chỉ 7-bit của thiết bị I2C
 * @param reg Thanh ghi bên trong thiết bị I2C cần ghi
 * @param data Giá trị cần ghi
 * @return uint8_t 1 nếu gửi thành công, 0 nếu timeout
 */
uint8_t I2C_WriteByte(uint8_t addr, uint8_t reg, uint8_t data) {
    uint32_t timeout;

    // 1. Gửi tín hiệu START
    I2C1->CR1 |= (1 << 8); // START

    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 0)) && --timeout); // Chờ cờ SB = 1
    if (!timeout) return 0;

    // 2. Gửi địa chỉ thiết bị (bit cuối = 0 để ghi)
    I2C1->DR = addr << 1;

    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 1)) && --timeout); // Chờ ADDR = 1
    if (!timeout) return 0;
    (void)I2C1->SR2;  // Đọc SR2 để xóa cờ ADDR

    // 3. Gửi địa chỉ thanh ghi nội bộ cần ghi
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 7)) && --timeout); // Chờ TXE = 1
    if (!timeout) return 0;
    I2C1->DR = reg;

    // 4. Gửi dữ liệu
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 7)) && --timeout); // TXE = 1
    if (!timeout) return 0;
    I2C1->DR = data;

    // 5. Chờ truyền xong hoàn toàn (BTF = 1)
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 2)) && --timeout); // BTF = 1
    if (!timeout) return 0;

    // 6. Gửi tín hiệu STOP để kết thúc giao tiếp
    I2C1->CR1 |= (1 << 9);  // STOP

    return 1; // Thành công
}


// ===============================
// =========== END FILE ==========
// ===============================
