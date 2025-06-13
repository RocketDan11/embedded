#include "stm32f0xx.h"

void delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
}

int main(void) {
    // 1. Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // 2. Configure PA5 as output
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));

    // 3. Blink loop
    while (1) {
        GPIOA->ODR ^= (1U << 5); // Toggle LED
        delay(1000000);           // crude delay
    }
}

