// ===============================
// ========== FILE INCLUDE =======
// ===============================

#include "stm32f4xx.h"
#include "adc.h"


// ======================================
// ======== FUNCTION DEFINITIONS ========
// ======================================

/**
 * @brief Khởi tạo ADC1 đọc từ chân PA0 (kênh ADC_IN0)
 */
void ADC_Init(void) {
    // ===== Bật clock cho ADC1 (bit 8 của RCC->APB2ENR) =====
    RCC->APB2ENR |= (1 << 8); // ADC1EN

    // ===== Bật clock cho GPIOA (bit 0 của RCC->AHB1ENR) =====
    RCC->AHB1ENR |= (1 << 0); // GPIOAEN

    // ===== PA0 vào chế độ analog: MODER0 = 11 =====
    GPIOA->MODER |= (3 << (0 * 2)); // Bit 1:0 = 11

    // ===== Chọn kênh chuyển đổi: ADC1_IN0 -> SQR3[4:0] = 00000 =====
    ADC1->SQR3 = 0;

    // ===== Cài đặt thời gian lấy mẫu cho kênh 0: 480 chu kỳ (SMPR2) =====
    ADC1->SMPR2 |= (7 << 0); // SMP0 = 111

    // ===== Bật ADC1 (bit ADON trong CR2) =====
    ADC1->CR2 |= (1 << 0); // ADON = 1
}


/**
 * @brief Đọc giá trị từ kênh ADC đã cấu hình
 * @return Giá trị 12-bit từ ADC1->DR
 */
uint16_t ADC_Read(void) {
    // Bắt đầu chuyển đổi (SWSTART = 1, bit 30 của CR2)
    ADC1->CR2 |= (1 << 30);

    // Chờ đến khi hoàn tất chuyển đổi (EOC = 1, bit 1 của SR)
    while (!(ADC1->SR & (1 << 1)));

    // Trả về kết quả đọc được
    return ADC1->DR;
}


/**
 * @brief Cập nhật mode dựa trên giá trị ADC
 * @return Mode tương ứng (0 đến 3)
 */
uint8_t Mode_Update_From_ADC(void) {
    uint16_t adc_value = ADC_Read();

    if (adc_value < 200) return 0;         // Vùng tắt
    else if (adc_value < 1365) return 1;   // 0.8V ~ mode 1
    else if (adc_value < 2730) return 2;   // ~1.65V ~ mode 2
    else return 3;                         // >2.2V ~ mode 3
}


// ===============================
// =========== END FILE ==========
// ===============================
