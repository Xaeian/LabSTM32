//#include <stdbool.h>
//#include <stdint.h>
//#include <stdlib.h>
//#include "stm32g0xx.h"
//#include "gpio.h"
//#include "delay.h"
//
////---------------------------------------------------------------------------------------------------------------------
//
//void send_buffer(uint8_t *buffer, uint16_t size)
//{
//  while(size--) {
//    USART1->TDR = *buffer;
//    while(!(USART1->ISR & USART_ISR_TC)) __NOP();
//    USART1->ICR |= USART_ICR_TCCF;
//    buffer++;
//  }
//}
//
//void send_string(char *buffer)
//{
//  while(*buffer) {
//    USART1->TDR = *buffer;
//    while(!(USART1->ISR & USART_ISR_TC)) __NOP();
//    USART1->ICR |= USART_ICR_TCCF;
//    buffer++;
//  }
//}
//
//void send_enter(void)
//{
//  send_buffer((uint8_t *)"\r\n", 2);
//}
//
//const uint32_t divider[] = { 100000, 10000, 1000, 100, 10, 1 };
//
//void send_int(int32_t nbr)
//{
//  char str[12];
//  char *set = str;
//  if(nbr < 0) {
//    *set = '-';
//    set++;
//    nbr = -nbr;
//  }
//  else if(nbr == 0) {
//    send_string("0");
//    return;
//  }
//  bool no_zero = true;
//
//  for(uint8_t i = 0; i < sizeof(divider) / sizeof(uint32_t); i++) {
//    uint8_t value = nbr / divider[i];
//    if(no_zero && value == 0);
//    else {
//      no_zero = false;
//      *set = (nbr / divider[i]) + '0';
//      set++;
//    }
//
//    nbr = nbr % divider[i];
//  }
//  *set = 0;
//  send_string(str);
//}
//
//// PC4-PC5 UART1
//
//char test_str[] = "test";
//char test_str_size = sizeof(test_str);
//
//int32_t test_int = -20;
//
//int main(void)
//{
//  RCC->IOPENR |= RCC_IOPSMENR_GPIOCSMEN;
//  GPIOC->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
//  GPIOC->MODER |= GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1;
//  GPIOC->AFR[0] &= ~(GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5);
//  GPIOC->AFR[0] |= (1 << GPIO_AFRL_AFSEL4_Pos) | (1 << GPIO_AFRL_AFSEL5_Pos);
//
//  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
//  USART1->BRR = SystemCoreClock / 9600;
//  USART1->CR3 |= USART_CR3_OVRDIS;
//
//  while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);
//
//  USART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
//  NVIC_SetPriority(USART1_IRQn, 2);
//  NVIC_EnableIRQ(USART1_IRQn);
//  USART1->ICR |= USART_ICR_TCCF;
//  USART1->RQR |= USART_RQR_RXFRQ;
//
//  while(1) {
//
//    send_int(test_int);
//    send_enter();
//    test_int++;
//
//
//
////    char *p = test;
////    while(--size) {
////      USART1->TDR = *p;
////      while(!(USART1->ISR & USART_ISR_TC)) __NOP();
////      USART1->ICR |= USART_ICR_TCCF;
////      p++;
////    }
////    size = sizeof(test);
//
////    for(uint8_t i = 0; i < size; i++) {
////      USART1->TDR = test[i];
////      delay_ms(10);
////    }
//
//    delay_ms(500);
//
////    USART1->TDR = 'a';
////    USART1->TDR = 'b';
////    USART1->TDR = 'c';
////    USART1->TDR = 'd';
////    delay_ms(1000);
////    __NOP();
//  }
//}
//
//void USART1_IRQHandler(void)
//{
//  if(USART1->ISR & USART_ISR_RXNE_RXFNE)
//  {
//    uint8_t read = (uint8_t)(USART1->RDR);
//    read += 3;
//    if(read > 'z') read -= ('z' - 'a' + 1);
//    USART1->TDR = read;
//  }
//}
//
////---------------------------------------------------------------------------------------------------------------------
//
//void SysTick_Handler(void)
//{
//  // ...
//}
//
////---------------------------------------------------------------------------------------------------------------------
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
////int main2(void)
////{
////  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOCEN;
////  // RCC->IOPENR |= (1 << 0) | (1 << 2)
////
////  GPIOA->MODER &= ~(3 << (2 * 11)); // GPIO_MODER_MODE11
////  GPIOA->MODER |= (1 << (2 * 11)); // GPIO_MODER_MODE11_0
////  GPIOC->MODER &= ~(3 << (2 * 3)); // GPIO_MODER_MODE5
////
////  GPIOA->PUPDR |= (1 << (2 * 13)); // błąd gruby
////  GPIOA->PUPDR |= (2 << (2 * 13)); // błąd mały
////
////  while(1)
////  {
////    if(GPIOC->IDR & (1 << 3)) // if(switch PC3 is clicked)
////    {
////      GPIOA->BRR = (1 << 11); // put out PA11
////      // GPIOA->ODR &= ~GPIO_ODR_OD11;
////    }
////    else
////    {
////      GPIOA->BSRR = (1 << 11); // light up PA11
////      //GPIOA->ODR |= GPIO_ODR_OD11;
////    }
////  }
////}
