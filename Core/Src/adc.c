/* ==========================================================================================
 * File        : adc.c
 * Description : Khởi tạo và xử lý ADC1 đọc tín hiệu đầu vào chuyển mode hoạt động
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "stm32f4xx.h"
#include "adc.h"
#include "system.h"


/* ========================== [II] FUNC FILE ============================ */

// Biến lưu trữ giá trị ADC mới nhất đọc được (cập nhật trong interrupt)
volatile uint16_t adc_latest_value = 0;

/**
 * @brief  Khởi tạo ADC1 để đọc tín hiệu analog từ PA0 (kênh 0)
 */
void ADC_Init(void) {
    // Bật clock ADC1 (nằm trên bus APB2)
	RCC->APB2ENR |= (1 << 8);

    // Chọn kênh đầu tiên trong regular sequence (SQR3: rank 1 -> channel 0)
    ADC1->SQR3 = 0;

    // Cấu hình thời gian lấy mẫu cho kênh 0: 480 cycles (tối đa - SMPR2[2:0] = 0b111)
    ADC1->SMPR2 |= (7 << 0);

    // Bật ADC (ADON = 1)
    ADC1->CR2 |= (1 << 0);

    // Kích hoạt ngắt khi kết thúc chuyển đổi (End of Conversion interrupt)
    ADC1->CR1 |= (1 << 5);

    // Bật ngắt ADC trong NVIC
    NVIC_EnableIRQ(ADC_IRQn);

    // Delay nhỏ sau khi bật ADC (datasheet yêu cầu để ADC ổn định)
    Delay_ms(2);
}

/**
 * @brief  Ngắt ADC: Đọc giá trị ADC khi chuyển đổi hoàn tất
 */
void ADC_IRQHandler(void) {
    // Kiểm tra cờ EOC (End of Conversion)
    if (ADC1->SR & (1 << 1)) {
        adc_latest_value = ADC1->DR;  // Đọc giá trị ADC, tự động clear cờ EOC
    }
}

/**
 * @brief  Chuyển đổi giá trị ADC sang mode hệ thống
 * @retval Mode hệ thống (0, 1, 2, 3)
 */
uint8_t Mode_Update_From_ADC(void) {
    uint16_t adc_value = adc_latest_value;

    if (adc_value < 200) return 0;
    else if (adc_value < 1365) return 1;
    else if (adc_value < 2730) return 2;
    else return 3;
}


/* =========================== [III] END FILE =========================== */
