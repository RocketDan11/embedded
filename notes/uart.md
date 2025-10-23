USARTx_BRR (Baud Rate Register)

Defines Baud rate according to:
BRR = \frac(f_ck, Baud) 
eg: f_ck = 8 MHz, Baud = 9600,
\frac(8,9600) ~~ 833

Write this value directly to BRR
```c
USART1->BRR = 833u;
```
*integer approximation is sufficient for common baud.

---

USARTx_CR1 (Control Register 1)

 Main control settings for transmitter and receiver
 - TE (bit 3) -> Enable transmitter
 - RE (bit 2) -> Enable receiver
 - UE (bit 13) -> Enable USART peripheral
 - RXNEIE (bit 5) -> Enable receive interrupt (optional)
 - M1:M0 (bits 28 & 12) -> Set word length (00 = 8 bits)
 - PCE (bit 10) -> Parity control enable (0 = no parity)
 
 USARTx_CR2 (Control Register 2)
 
 Configures stop bits and additional options
 - STOP[1:0](bits 13:12) -> Stop bit selection
 	- 00 = 1 stop bit
 	- 10 = 2 stop bits
 - Other fields are used for synchronous mode or clock settings (leave default for UART)
 
 ---
 
 Interrupt & Status Register (USARTx_ISR)
 
 Used for checking transmit/receive 
 - TXE (bit 7) -> Transmit data register empty.
 - RXNE (bit 5) -> Recieve register not empty.
 - TC (bit 6) -> Transmission complete.
 - Common usage:
 	- Wait for TXE == 1 before writing new data.
 	- Wait for RXNE == 1 before reading recieved data.

---

Example Code
------------

```c
#include "stm32f0xx.h"
#include <stdint.h>

void uart1_init(void){
	// Enable GPIO and USART1 clocks
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	//Configure PA9 (TX) and PA10 (RX) as AF1
	GPIO_>MODER &= ~((3u << (9*2)) | (3u << (10*2)));
	GPIO->MODER |= ((2u << (9*2)) | (2u << (10*2)));
	GPIO->AFR[1] |= (1u << ((9-8)*4)) | (1u << ((10-8)*4));
	
	//9600 Baud at 8 MHz -> BRR = 833
	USART->BRR = 833u;
	
	//8N1 config: 8 data bits, no parity, 1 stop bit
	USART->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void uart_send_char(char c){
	while(!(USART1->ISR & USART_ISR_TXE));
	USART->TDR = c;
}

int main(void){
	uart1_init();
	while(1){
		uart1_send_char('A');
		for(volatile uint32_t i=0; i<800000; i++); //delay 1 sec
 	}
}
```

 
 
 
 
 
 
 
