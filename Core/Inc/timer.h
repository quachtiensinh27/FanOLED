// ====== timer.h ======
#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>

// Khởi tạo TIM3 với ngắt định kỳ 100ms
void TIM3_Init(void);

// Trình xử lý ngắt TIM3 (được định nghĩa trong timer.c)
void TIM3_IRQHandler(void);

#endif // __TIMER_H
