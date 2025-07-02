#include "stm32f0xx.h"

void delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
}

void sw_debounce(void) {
    for (volatile uint32_t i = 0; i < 50000; ++i) __asm__("nop");
}

int main(void) {
    // 1. Enable GPIOC and GPIOA clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN;

    // 2. Configure PA5 as output (LED)
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (1U << (5 * 2));

    // 3. Configure PC13 as input with pull-down
    GPIOC->MODER &= ~(3U << (13 * 2));
    GPIOC->PUPDR &= ~(3U << (13 * 2));
    GPIOC->PUPDR |=  (2U << (13 * 2)); // pull-down

    while (1) {
        if ((GPIOC->IDR & (1U << 13)) != 0) {        // B1 pressed
            sw_debounce();
            if ((GPIOC->IDR & (1U << 13)) != 0) {    // still pressed
                GPIOA->ODR ^= (1U << 5);            // toggle LED
                while ((GPIOC->IDR & (1U << 13)) != 0);
                sw_debounce();
            }
        }
    }
}

