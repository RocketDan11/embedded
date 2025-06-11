// 1. Enable the GPIOA peripheral clock
RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

/* 2. Set PA5 to "01" (general-purpose output) in MODER.
 * Each pin uses two bits, so PA5 is bits 11:10.
 */
GPIOA->MODER &= ~(0x3U << (5 * 2));   // clear both bits
GPIOA->MODER |=  (0x1U << (5 * 2));   // set to 01

// 3. (Optional) Select push-pull, low-speed, no pull-ups
// GPIOA->OTYPER &= ~(1U << 5);
// GPIOA->OSPEEDR &= ~(0x3U << (5 * 2));
// GPIOA->PUPDR &= ~(0x3U << (5 * 2));

// 4. Drive PA5 high
GPIOA->ODR |= (1U << 5);
