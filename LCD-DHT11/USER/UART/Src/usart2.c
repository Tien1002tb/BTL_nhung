#include "usart2.h"
#include "stdio.h"
#include "string.h"

/*
	PA2 - TX
	PA3 - RX
*/

void USART2_Config(void)
{
	// Enable the Alternate Function for PINs
	RCC->APB2ENR |= 1;
	// Enable UART2
	RCC->APB1ENR |=0x20000;
	// Enable the related PINs
	RCC->APB2ENR |= (1 << 2);
	GPIOA->CRL |= 0xB00;
	// Setup the baude rate for 9600 bps
	USART2->BRR = 0xEA6; 
	// Enable Uart Transmit
	USART2->CR1 |= 8;
	// Enable Uart Recive
	USART2->CR1 |= 4;
	// Enable Uart
	USART2->CR1 |= 0x2000;
}

void USART2_SendChar(uint8_t ch)
{
	while((USART2->SR & (1<<6)) == 0x00){};
		USART2->DR = ch;
}

struct __FILE {
    int dummy;
};

void USART2_SendString(uint8_t *str)
{
	while(*str != 0)
	{
		USART2_SendChar(*str++);
	}
}

FILE __stdout;
 
int fputc(int ch, FILE *f) {
	USART2_SendChar(ch);
  return ch;
}
