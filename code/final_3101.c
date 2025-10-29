#include "stm32f0xx.h"

#include <stdint.h>



#define STATE_ON    0

#define STATE_OFF   1

#define STATE_BLINK 2

#define DEBOUNCE_DELAY 50



volatile uint8_t led_state = STATE_ON;

volatile uint32_t last_button_time = 0;

volatile uint32_t button_press_count = 0; // Debug counter



// Function prototypes

void gpio_init(void);

void update_led_state(void);



int main(void) {

    // HAL init is required for HAL_GetTick() to work

    HAL_Init();

    gpio_init();



    // Initialize LED to ON state

    GPIOA->ODR |= (1U << 5);



    while(1) {

        update_led_state();

    }

}



void gpio_init(void) {

    // Enable port clocks for GPIOA (LED) and GPIOC (button)

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;



    // Configure PA5 as output (LED)

    GPIOA->MODER &= ~(3U << (5 * 2));  // Clear mode bits

    GPIOA->MODER |= (1U << (5 * 2));   // Set as output



    // Configure PC13 as input (USER button)

    GPIOC->MODER &= ~(3U << (13 * 2)); // Input mode (00)



    // Enable pull-up on PC13

    GPIOC->PUPDR &= ~(3U << (13 * 2)); // Clear pull-up/down bits

    GPIOC->PUPDR |= (1U << (13 * 2));  // Set pull-up (01)



    // Enable SYSCFG clock for EXTI configuration

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;



    // Route PC13 to EXTI line 13 (EXTICR4 controls lines 12-15)

    // EXTICR4[7:4] controls EXTI13

    SYSCFG->EXTICR[3] &= ~(0xF << 4);  // Clear bits for EXTI13

    SYSCFG->EXTICR[3] |= (0x2 << 4);   // Port C = 0010



    // Configure EXTI13 for falling edge trigger (button press)

    EXTI->FTSR |= (1U << 13);  // Falling edge trigger

    EXTI->RTSR &= ~(1U << 13); // Disable rising edge



    // Unmask EXTI line 13

    EXTI->IMR |= (1U << 13);



    // Clear any pending EXTI13 interrupt

    EXTI->PR |= (1U << 13);



    // Set priority (optional, but good practice)

    NVIC_SetPriority(EXTI4_15_IRQn, 3);



    // Enable NVIC for EXTI4_15

    NVIC_EnableIRQ(EXTI4_15_IRQn);

}



void update_led_state(void) {

    static uint32_t last_blink_time = 0;

    uint32_t current_time = HAL_GetTick();



    switch(led_state) {

        case STATE_ON:

            // LED solid ON

            GPIOA->ODR |= (1U << 5);

            break;



        case STATE_OFF:

            // LED solid OFF

            GPIOA->ODR &= ~(1U << 5);

            break;



        case STATE_BLINK:

            // Toggle LED every 500ms

            if (current_time - last_blink_time >= 500) {

                GPIOA->ODR ^= (1U << 5);

                last_blink_time = current_time;

            }

            break;



        default:

            led_state = STATE_ON;

            break;

    }

}



// EXTI4_15 interrupt handler

void EXTI4_15_IRQHandler(void) {

    // Check if EXTI13 triggered the interrupt

    if (EXTI->PR & (1U << 13)) {

        button_press_count++; // Debug: increment counter



        // Debounce: check if enough time has passed since last button press

        uint32_t current_time = HAL_GetTick();

        if ((current_time - last_button_time) > DEBOUNCE_DELAY) {

            // Update state machine

            led_state++;

            if (led_state > STATE_BLINK) {

                led_state = STATE_ON;

            }



            // Update debounce timestamp

            last_button_time = current_time;

        }



        // Clear EXTI13 pending flag (MUST be done by writing 1)

        EXTI->PR = (1U << 13);

    }

}