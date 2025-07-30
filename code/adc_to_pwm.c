#include "stm32f0xx.h"

// UART init (optional debug)
void uart2_init(void) {
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    GPIOA->MODER &= ~((3U<<(2*2))|(3U<<(3*2)));
    GPIOA->MODER |=  ((2U<<(2*2))|(2U<<(3*2)));
    GPIOA->AFR[0]  |= (1U<<(2*4))|(1U<<(3*4));

    USART2->BRR = 8000000U/115200U;
    USART2->CR1 = USART_CR1_TE | USART_CR1_UE;
}

// Send 16-bit unsigned int over UART
void uart_send_u16(uint16_t x) {
    char buf[6] = {0}, *p = buf + 5;
    do {
        *--p = '0' + (x % 10);
        x /= 10;
    } while (x);
    while (*p) {
        while (!(USART2->ISR & USART_ISR_TXE));
        USART2->TDR = *p++;
    }
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = '\n';
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = '\r';
}

// ADC1 continuous on PA0
void adc_init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    GPIOA->MODER |= (3U << (0 * 2));  // PA0 as analog input

    RCC->CR2 |= RCC_CR2_HSI14ON;              // Enable HSI14
    while (!(RCC->CR2 & RCC_CR2_HSI14RDY));   // Wait until ready

    ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;         // Use ADC clock
    ADC1->CR = 0;
    ADC1->CR |= ADC_CR_ADCAL;                 // Start calibration
    while (ADC1->CR & ADC_CR_ADCAL);          // Wait for calibration

    ADC1->CHSELR = (1U << 0);                 // Select channel 0 (PA0)
    ADC1->SMPR = 0;                           // Shortest sample time
    ADC1->CFGR1 |= ADC_CFGR1_CONT;            // Continuous mode
    ADC1->CR |= ADC_CR_ADEN;                  // Enable ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY));     // Wait for ready
    ADC1->CR |= ADC_CR_ADSTART;               // Start conversion
}

// PWM output on PA6 (TIM3_CH1, AF1)
void pwm_init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    GPIOA->MODER &= ~(3U << (6 * 2));
    GPIOA->MODER |=  (2U << (6 * 2));          // PA6 → AF
    GPIOA->AFR[0] &= ~(0xF << (6 * 4));
    GPIOA->AFR[0] |=  (1U << (6 * 4));         // AF1 for TIM3_CH1

    TIM3->PSC = 12 - 1;                        // 48 MHz / 12 = 4 MHz timer clock
    TIM3->ARR = 1023;                          // 4 MHz / 1024 ≈ 3.9 kHz PWM frequency
    TIM3->CCR1 = 0;

    TIM3->CCMR1 = (6 << 4);                    // PWM Mode 1 (OC1M = 110)
    TIM3->CCER |= TIM_CCER_CC1E;              // Enable output
    TIM3->CR1 |= TIM_CR1_ARPE;                // Auto-reload preload enable
    TIM3->CR1 |= TIM_CR1_CEN;                 // Enable counter
}

// Convert 12-bit ADC (0–4095) to PWM duty cycle (0–1023)
void update_pwm(uint16_t adc) {
    uint32_t duty = ((uint32_t)adc * 1023U) / 4095U;
    TIM3->CCR1 = duty;
}

int main(void) {
    uart2_init();  // optional
    adc_init();
    pwm_init();

    while (1) {
        if (ADC1->ISR & ADC_ISR_EOC) {
            uint16_t adc_val = ADC1->DR;
            ADC1->ISR = ADC_ISR_EOC;  // clear EOC flag

            update_pwm(adc_val);
            uart_send_u16(adc_val);  // optional debug output
        }
    }
}

