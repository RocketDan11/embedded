#include "stm32f0xx.h"

// LED output pins: PA0, PA1, PA4
#define LED_MASK ((1U << 0) | (1U << 1) | (1U << 4))

// Button input pin: PA6
#define BUTTON_PIN (1U << 6)

void delay_ms(volatile uint32_t ms) {
    for (; ms > 0; ms--)
        for (volatile uint32_t i = 0; i < 8000; i++) __asm__("nop");
}

void enable_gpioa_clock(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
}

void set_led_pins_output(void) {
    // PA0
    GPIOA->MODER &= ~(3U << (0 * 2));
    GPIOA->MODER |=  (1U << (0 * 2));

    // PA1
    GPIOA->MODER &= ~(3U << (1 * 2));
    GPIOA->MODER |=  (1U << (1 * 2));

    // PA4
    GPIOA->MODER &= ~(3U << (4 * 2));
    GPIOA->MODER |=  (1U << (4 * 2));
}

void set_button_input_pullup(void) {
    // Set PA6 as input
    GPIOA->MODER &= ~(3U << (6 * 2));

    // Enable internal pull-up
    GPIOA->PUPDR &= ~(3U << (6 * 2));
    GPIOA->PUPDR |=  (1U << (6 * 2));
}

int main(void) {
    enable_gpioa_clock();
    set_led_pins_output();
    set_button_input_pullup();

    uint8_t state = 0;
    uint8_t last_button = 1; // Initially HIGH (unpressed)

    GPIOA->ODR &= ~LED_MASK;

    while (1) {
        uint8_t current_button = (GPIOA->IDR & BUTTON_PIN) ? 1 : 0;

        // Detect falling edge (press)
        if (last_button && !current_button) {
            delay_ms(1); // debounce
            // Re-read after debounce
            current_button = (GPIOA->IDR & BUTTON_PIN) ? 1 : 0;
            if (!current_button) {
                // Valid press detected
                state = (state + 1) % 5;

                GPIOA->ODR &= ~LED_MASK;

                switch (state) {
                    case 0: GPIOA->ODR |= (1U << 0); break;
                    case 1: GPIOA->ODR |= (1U << 1); break;
                    case 2: GPIOA->ODR |= (1U << 4); break;
                    case 3: GPIOA->ODR |= LED_MASK;  break;
                    case 4: /* All OFF */ break;
                }
            }
        }

        last_button = current_button;
    }
}
