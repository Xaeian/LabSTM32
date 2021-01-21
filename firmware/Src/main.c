#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32g0xx.h"
#define delay_ms(ms) for(int i = 3203*ms; i; i--) __NOP()




bool dir;

int main(void)
{

  // Zegar magistrali A

  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
  RCC->IOPENR |= RCC_IOPSMENR_GPIOASMEN;

  GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
  GPIOA->MODER |= (2 << GPIO_MODER_MODE9_Pos) | (2 << GPIO_MODER_MODE10_Pos);

  GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk);
  GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFSEL9_Pos) | (1 << GPIO_AFRH_AFSEL10_Pos);

  USART1->BRR = SystemCoreClock / 9600;
  USART1->CR3 |= USART_CR3_OVRDIS;

  while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);

  USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

  //USART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
  //NVIC_SetPriority(USART1_IRQn, 2);
  //NVIC_EnableIRQ(USART1_IRQn);

  USART1->ICR |= USART_ICR_TCCF;
  USART1->RQR |= USART_RQR_RXFRQ;

  // Set AF1 PA9

    // Clr PA9 -> AF -> 1
    // Set PA9 -> AF -> 1

  uint8_t send = '0';

  while(1)
  {
    USART1->TDR = send;
    send++;
    delay_ms(200);
  }
}

/*
void USART1_IRQHandler(void)
{
  if(USART1->ISR & USART_ISR_RXNE_RXFNE)
  {
    uint8_t var = (uint8_t)(USART1->RDR);
  }
}
*/




