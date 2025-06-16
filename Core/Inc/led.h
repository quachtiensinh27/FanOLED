// ====== led.h ======
#ifndef LED_H
#define LED_H

#include <stdint.h>

extern volatile uint8_t oled_state;

void LED_Init(void);
void LED_Update(uint8_t current_mode, uint8_t countdown);

#endif
