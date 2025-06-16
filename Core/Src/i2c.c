/* ==========================================================================================
 * File        : i2c.c
 * Description : Cấu hình và thực thi giao tiếp I2C1 master mode (OLED SSD1306)
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "stm32f4xx.h"
#include "i2c.h"


/* ========================== [II] FUNC FILE ============================ */

// Timeout sử dụng để tránh kẹt bus I2C
#define I2C_TIMEOUT 100000

/**
 * @brief  Khởi tạo I2C1 làm master mode
 *
 * Thông số cài đặt:
 * - Clock APB1: 16 MHz
 * - I2C speed: ~100 kHz (standard mode)
 */
void I2C1_Init(void) {
    // Bật clock cho I2C1 (bus APB1)
	RCC->APB1ENR |= (1 << 21);

    // Đảm bảo tắt I2C trước khi cấu hình
	I2C1->CR1 &= ~(1 << 0);

    // Cấu hình tần số (CR2): đơn vị MHz (16 MHz)
    I2C1->CR2 = 16;

    // Cấu hình CCR (chu kỳ clock): CCR = 80 → SCL ~100kHz (theo datasheet)
    I2C1->CCR = 80;

    // Cấu hình TRISE: (max rise time) → 17 (theo công thức datasheet)
    I2C1->TRISE = 17;

    // Bật lại I2C sau khi cấu hình xong
    I2C1->CR1 |= (1 << 0);
}


/**
 * @brief  Gửi 1 byte dữ liệu qua I2C1 theo chuẩn SSD1306 OLED
 * @param  addr  Địa chỉ thiết bị I2C (7-bit, đã bỏ bit R/W)
 * @param  reg   Giá trị control byte (SSD1306: 0x00 = command, 0x40 = data)
 * @param  data  Dữ liệu cần gửi
 * @retval 1 nếu thành công, 0 nếu timeout
 */
uint8_t I2C_WriteByte(uint8_t addr, uint8_t reg, uint8_t data) {
    uint32_t timeout;

    // Bắt đầu truyền (generate START condition)
    I2C1->CR1 |= (1 << 8);

    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 0)) && --timeout); // Chờ gửi xong START
    if (!timeout) return 0;

    // Gửi địa chỉ thiết bị
    I2C1->DR = addr << 1;  // 7-bit address, bit R/W = 0 (write)
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 1)) && --timeout); // Chờ ACK
    if (!timeout) return 0;

    // Đọc SR2 để clear cờ ADDR (bắt buộc khi dùng polling)
    (void)I2C1->SR2;

    // Gửi control byte (command/data selector)
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 7)) && --timeout);
    if (!timeout) return 0;
    I2C1->DR = reg;

    // Gửi dữ liệu chính
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 7)) && --timeout);
    if (!timeout) return 0;
    I2C1->DR = data;

    // Chờ hoàn tất truyền toàn bộ byte
    timeout = I2C_TIMEOUT;
    while (!(I2C1->SR1 & (1 << 2)) && --timeout);
    if (!timeout) return 0;

    // Kết thúc truyền (generate STOP condition)
    I2C1->CR1 |= (1 << 9);

    return 1;  // Thành công
}


/* =========================== [III] END FILE =========================== */
