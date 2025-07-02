#include "stm32f0xx.h"
#include <stdlib.h>

void delay_ms(uint32_t ms) {
    volatile uint32_t cnt = ms * 8000U;
    while (cnt--) __asm__("nop");
}

void uart2_init(void) {
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    GPIOA->MODER &= ~((3U<<(2*2)) | (3U<<(3*2)));
    GPIOA->MODER |=  ((2U<<(2*2)) | (2U<<(3*2)));
    GPIOA->AFR[0] |=  (1U<<(2*4)) | (1U<<(3*4));
    USART2->BRR = 8000000U / 115200U;
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void uart_sendchar(char c) {
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = c;
}

void uart_sendstr(const char *s) {
    while (*s) uart_sendchar(*s++);
}

void uart_send_u16(uint16_t x) {
    char buf[6] = {0}, *p = buf + 5;
    do { *--p = '0' + (x % 10); x /= 10; } while (x);
    while (*p) uart_sendchar(*p++);
}

void adc_init(void) {
    RCC->AHBENR   |= RCC_AHBENR_GPIOAEN;             // enable PA0 GPIO
    RCC->APB2ENR  |= RCC_APB2ENR_ADC1EN;             // enable ADC1 clock
    GPIOA->MODER  |= (3U << (0 * 2));                // PA0 = analog mode
    RCC->CR2      |= RCC_CR2_HSI14ON;                // turn on HSI14 for ADC
    while (!(RCC->CR2 & RCC_CR2_HSI14RDY));
    ADC1->CFGR2   &= ~ADC_CFGR2_CKMODE;              // HSI14 as ADC clock
    ADC1->CR      = 0;                               // reset ADC
    ADC1->CR     |= ADC_CR_ADCAL;                    // calibrate
    while (ADC1->CR & ADC_CR_ADCAL);
    ADC1->CHSELR  = (1U << 0);                       // select channel 0 (PA0)
    ADC1->SMPR    = 0;                               // max sample time
    ADC1->CR     |= ADC_CR_ADEN;                     // enable ADC
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

uint16_t adc_read(void) {
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC));
    return (uint16_t)ADC1->DR;
}

int main(void) {
    uart2_init();
    adc_init();
    uart_sendstr("ADC read (0–4095) from PA0:\r\n");
    while (1) {
        uint16_t v = adc_read();
        uart_send_u16(v);
        uart_sendchar('\r');
        uart_sendchar('\n');
        delay_ms(100 );
    }
}

