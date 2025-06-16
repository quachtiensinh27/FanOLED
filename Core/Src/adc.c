// ====== adc.c ======
#include "stm32f4xx.h"
#include "adc.h"
#include "system.h"

volatile uint16_t adc_latest_value = 0;

void ADC_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER |= (3 << (0 * 2));

    ADC1->SQR3 = 0;
    ADC1->SMPR2 |= (7 << 0);
    ADC1->CR2 |= ADC_CR2_ADON;

    ADC1->CR1 |= ADC_CR1_EOCIE;
    NVIC_EnableIRQ(ADC_IRQn);

    Delay_ms(2);
}

void ADC_IRQHandler(void) {
    if (ADC1->SR & ADC_SR_EOC) {
        adc_latest_value = ADC1->DR;
    }
}

uint8_t Mode_Update_From_ADC(void) {
    uint16_t adc_value = adc_latest_value;
    if (adc_value < 200) return 0;
    else if (adc_value < 1365) return 1;
    else if (adc_value < 2730) return 2;
    else return 3;
}
