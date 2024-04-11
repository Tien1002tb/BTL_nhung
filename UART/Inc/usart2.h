#include "stm32f10x.h"

void USART2_Config(void);
void USART2_SendChar(uint8_t ch);
void USART2_SendString(uint8_t *str);
void USART2_ReceiveString(void);
void USART2_IRQHandler(void);