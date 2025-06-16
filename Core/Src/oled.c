/* ==========================================================================================
 * File        : oled.c
 * Description : Thư viện điều khiển màn hình OLED SSD1306 qua I2C
 * MCU         : STM32F401CCU6
 * ==========================================================================================
 */


/* ========================== [I] INCLUDE FILE ========================== */

#include "oled.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>
#include "system.h"


/* ========================== [II] FUNC FILE ============================ */

/*
 * Bảng font 5x8 (mỗi ký tự 5 byte cột), hỗ trợ:
 * - A-Z (26 ký tự)
 * - a-z (26 ký tự)
 * - 0-9 (10 ký tự)
 * - Space (1 ký tự trắng)
 */
const uint8_t font5x8[][5] = {
    // A–Z (0–25)
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x7F,0x20,0x18,0x20,0x7F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x03,0x04,0x78,0x04,0x03}, // Y
    {0x61,0x51,0x49,0x45,0x43}, // Z

    // a–z (26–51)
    {0x20,0x54,0x54,0x54,0x78}, // a
    {0x7F,0x48,0x44,0x44,0x38}, // b
    {0x38,0x44,0x44,0x44,0x20}, // c
    {0x38,0x44,0x44,0x48,0x7F}, // d
    {0x38,0x54,0x54,0x54,0x18}, // e
    {0x08,0x7E,0x09,0x01,0x02}, // f
    {0x0C,0x52,0x52,0x52,0x3E}, // g
    {0x7F,0x08,0x04,0x04,0x78}, // h
    {0x00,0x44,0x7D,0x40,0x00}, // i
    {0x20,0x40,0x44,0x3D,0x00}, // j
    {0x7F,0x10,0x28,0x44,0x00}, // k
    {0x00,0x41,0x7F,0x40,0x00}, // l
    {0x7C,0x04,0x18,0x04,0x78}, // m
    {0x7C,0x08,0x04,0x04,0x78}, // n
    {0x38,0x44,0x44,0x44,0x38}, // o
    {0x7C,0x14,0x14,0x14,0x08}, // p
    {0x08,0x14,0x14,0x18,0x7C}, // q
    {0x7C,0x08,0x04,0x04,0x08}, // r
    {0x48,0x54,0x54,0x54,0x20}, // s
    {0x04,0x3F,0x44,0x40,0x20}, // t
    {0x3C,0x40,0x40,0x20,0x7C}, // u
    {0x1C,0x20,0x40,0x20,0x1C}, // v
    {0x3C,0x40,0x30,0x40,0x3C}, // w
    {0x44,0x28,0x10,0x28,0x44}, // x
    {0x0C,0x50,0x50,0x50,0x3C}, // y
    {0x44,0x64,0x54,0x4C,0x44}, // z

    // 0–9 (52–61)
    {0x3E,0x45,0x49,0x51,0x3E}, // 0
    {0x00,0x41,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9

    // Space (62)
    {0x00,0x00,0x00,0x00,0x00}
};


/**
 * @brief Gửi lệnh điều khiển SSD1306 (control byte = 0x00)
 *
 * @param cmd Lệnh SSD1306
 * @retval 1 thành công, 0 lỗi I2C
 */
uint8_t SSD1306_Command(uint8_t cmd) {
    return I2C_WriteByte(0x3C, 0x00, cmd);
}


/**
 * @brief Gửi dữ liệu hiển thị SSD1306 (control byte = 0x40)
 *
 * @param data Dữ liệu pixel
 * @retval 1 thành công, 0 lỗi I2C
 */
uint8_t SSD1306_Data(uint8_t data) {
    return I2C_WriteByte(0x3C, 0x40, data);
}


/**
 * @brief Khởi tạo SSD1306 OLED theo chuỗi lệnh chuẩn datasheet
 *
 * Bao gồm: clock, charge pump, addressing mode, orientation, contrast...
 */
uint8_t SSD1306_Init(void) {
    Delay_ms(100);  // Đợi OLED ổn định sau khi cấp nguồn

    const uint8_t init_seq[] = {
        0xAE,       // Display OFF
        0xD5, 0x80, // Set display clock divide ratio/oscillator frequency
        0xA8, 0x3F, // Set multiplex ratio (1/64 duty)
        0xD3, 0x00, // Set display offset
        0x40,       // Set start line address
        0x8D, 0x14, // Enable charge pump
        0x20, 0x00, // Memory addressing mode: Horizontal
        0xA1,       // Set segment re-map
        0xC8,       // COM output scan direction: remapped
        0xDA, 0x12, // Set COM pins hardware configuration
        0x81, 0xCF, // Set contrast control
        0xD9, 0xF1, // Set pre-charge period
        0xDB, 0x40, // Set VCOMH deselect level
        0xA4,       // Entire display ON (resume)
        0xA6,       // Normal display (not inverted)
        0xAF        // Display ON
    };

    // Gửi toàn bộ chuỗi lệnh khởi tạo
    for (int i = 0; i < sizeof(init_seq); i++) {
        if (!SSD1306_Command(init_seq[i])) return 0;
    }
    return 1;
}


/**
 * @brief Đặt vị trí con trỏ vẽ trên OLED (đơn vị cột, trang)
 *
 * @param col  Cột (0~127)
 * @param page Trang (0~7)
 */
void SSD1306_SetCursor(uint8_t col, uint8_t page) {
    SSD1306_Command(0xB0 + page);                  // Set page address
    SSD1306_Command(0x00 + (col & 0x0F));          // Set lower column
    SSD1306_Command(0x10 + ((col >> 4) & 0x0F));   // Set higher column
}


/**
 * @brief Xóa toàn bộ nội dung OLED (clear screen)
 */
void SSD1306_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        SSD1306_SetCursor(0, page);
        for (uint8_t col = 0; col < 128; col++) {
            SSD1306_Data(0x00);
        }
    }
}


/**
 * @brief In 1 ký tự đơn lên OLED (font 5x8)
 *
 * @param ch Ký tự cần in (A-Z, a-z, 0-9, ký tự khác thành space)
 */
void SSD1306_PrintChar(char ch) {
    const uint8_t* chr;

    // Tra cứu mã font
    if (ch >= 'A' && ch <= 'Z')
        chr = font5x8[ch - 'A'];
    else if (ch >= 'a' && ch <= 'z')
        chr = font5x8[ch - 'a' + 26];
    else if (ch >= '0' && ch <= '9')
        chr = font5x8[ch - '0' + 52];
    else
        chr = font5x8[62];  // Ký tự khác → space

    // Gửi 5 byte font data + 1 cột trống để cách ký tự
    for (int i = 0; i < 5; i++) SSD1306_Data(chr[i]);
    SSD1306_Data(0x00);
}


/**
 * @brief In chuỗi text căn giữa theo trang
 *
 * @param page Trang hiển thị (0-7)
 * @param str  Chuỗi cần in
 */
void SSD1306_PrintTextCentered(uint8_t page, const char* str) {
    uint8_t len = strlen(str);
    uint8_t col = (128 - len * 6) / 2;  // Tính toán căn giữa

    SSD1306_SetCursor(col, page);
    while (*str)
        SSD1306_PrintChar(*str++);
}


/**
 * @brief In trạng thái hệ thống lên OLED
 *
 * Gồm 3 dòng:
 * - Tiêu đề: "DEVICE STATUS"
 * - Mode hiện tại
 * - Countdown còn lại hoặc trạng thái READY
 */
void SSD1306_DisplayStatus(uint8_t current_mode, uint8_t seconds_left) {
    char buffer[32];

    SSD1306_Clear();

    SSD1306_PrintTextCentered(1, "DEVICE STATUS");

    sprintf(buffer, "MODE %d", current_mode);
    SSD1306_PrintTextCentered(3, buffer);

    if (seconds_left > 0)
        sprintf(buffer, "TIME %ds", seconds_left);
    else
        sprintf(buffer, "READY");

    SSD1306_PrintTextCentered(5, buffer);
}


/* =========================== [III] END FILE =========================== */
