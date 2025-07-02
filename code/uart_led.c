#include "stm32f0xx.h"
#include <stdlib.h>

volatile uint32_t blink_delay = 500;  // initial blink delay (ms)
volatile char rx_buf[8];
volatile int rx_pos = 0;
volatile int command_ready = 0;

// Crude ms delay ≈8 MHz HSI
void delay_ms(uint32_t ms) {
    volatile uint32_t cnt = ms * 8000U;
    while (cnt--) __asm__("nop");
}

void uart2_init(void) {
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // PA2=TX, PA3=RX → AF1
    GPIOA->MODER &= ~((3U<<4) | (3U<<6));
    GPIOA->MODER |=  ((2U<<4) | (2U<<6));
    GPIOA->AFR[0] &= ~((0xFU<<8) | (0xFU<<12));
    GPIOA->AFR[0] |=  ((1U<<8) | (1U<<12));

    USART2->BRR = 8000000U / 115200U;
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;

    NVIC_EnableIRQ(USART2_IRQn);
}

void uart2_send(char c) {
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = c;
}

void uart2_sendstr(const char *s) {
    while (*s) uart2_send(*s++);
}

void USART2_IRQHandler(void) {
    if (USART2->ISR & USART_ISR_RXNE) {
        (void)USART2->ISR;
        char c = USART2->RDR & 0xFF;

        // Echo back
        while (!(USART2->ISR & USART_ISR_TXE));
        USART2->TDR = c;

        // Collect digits until newline
        if ((c >= '0' && c <= '9') && rx_pos < 7) {
            rx_buf[rx_pos++] = c;
        }
        else if ((c == '\r' || c == '\n') && rx_pos > 0) {
            rx_buf[rx_pos] = '\0';
            command_ready = 1;
        }
    }
}

int main(void) {
    uart2_init();

    // LED PA5 setup
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER = (GPIOA->MODER & ~(3U<<(5*2))) | (1U<<(5*2));

    uart2_sendstr("Enter blink delay in ms:\r\n");

    while (1) {
        // Process completed command
        if (command_ready) {
            command_ready = 0;
            uint32_t n = atoi((char*)rx_buf);
            blink_delay = (n > 0) ? n : blink_delay;
            char reply[32];
            snprintf(reply, sizeof(reply), "Blink = %lu ms\r\n", (unsigned long)blink_delay);
            uart2_sendstr(reply);
            rx_pos = 0;
        }

        // Blink LED
        GPIOA->ODR ^= (1U << 5);
        delay_ms(blink_delay);
    }
}


