#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"
#include "gpio.h"
//#include "delay.h"

//---------------------------------------------------------------------------------------------------------------------

#define delay_ms(ms) for(int _i = 1592 * ms; _i; _i--);

bool state = false;

int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOCEN;
  GPIOA->MODER &= ~(3 << (2 * 5));
  GPIOA->MODER |= (1 << (2 * 5)); // PA5: output
  GPIOC->MODER &= ~(3 << (2 * 13));
  GPIOC->PUPDR |= (1 << (2 * 13)); // PC13: output

  EXTI->EXTICR[3] &= EXTI_EXTICR4_EXTI13_Msk;
  EXTI->EXTICR[3] |= EXTI_EXTICR4_EXTI13_1;
  EXTI->FTSR1 |= (1 << 13);
  EXTI->RTSR1 |= (1 << 13);
  EXTI->IMR1 |= (1 << 13);
  NVIC_SetPriority(EXTI4_15_IRQn, 0);
  NVIC_EnableIRQ(EXTI4_15_IRQn);

  while(1)
  {
    __NOP();
  }
}

void EXTI4_15_IRQHandler(void)
{
  if(EXTI->FPR1 & (1 << 13)) {
    EXTI->FPR1 |= (1 << 13);
    GPIOA->ODR ^= (1 << 5);
  }
  if(EXTI->RPR1 & (1 << 13)) {
    EXTI->RPR1 |= (1 << 13);
  }
}


//int main(void)
//{
//  RCC->IOPENR |= RCC_IOPENR_GPIOCEN; // turn on clock signal on GPIOC
//  RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // turn on clock signal on GPIOA
//
//  GPIOC->MODER &= ~(3 << (2 * 5));
//  GPIOC->MODER |= (1 << (2 * 5)); // PC5: output
//
//  GPIOA->MODER &= ~(3 << (2 * 13)); // PA13: input
//  GPIOA->PUPDR |= (1 << (2 * 13)); // PA13: pull-up
//
//  while(1)
//  {
//    if(~GPIOA->IDR & (1 << 13)) // if(switch PA13 is clicked)
//    {
//      GPIOC->BSRR = (1 << 5); // light up PC5
//    }
//    else
//    {
//      GPIOC->BRR = (1 << 5); // put out PC5
//    }
//  }
//}
















//int main2(void)
//{
//  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOCEN;
//  // RCC->IOPENR |= (1 << 0) | (1 << 2)
//
//  GPIOA->MODER &= ~(3 << (2 * 11)); // GPIO_MODER_MODE11
//  GPIOA->MODER |= (1 << (2 * 11)); // GPIO_MODER_MODE11_0
//  GPIOC->MODER &= ~(3 << (2 * 3)); // GPIO_MODER_MODE5
//
//  GPIOA->PUPDR |= (1 << (2 * 13)); // błąd gruby
//  GPIOA->PUPDR |= (2 << (2 * 13)); // błąd mały
//
//  while(1)
//  {
//    if(GPIOC->IDR & (1 << 3)) // if(switch PC3 is clicked)
//    {
//      GPIOA->BRR = (1 << 11); // put out PA11
//      // GPIOA->ODR &= ~GPIO_ODR_OD11;
//    }
//    else
//    {
//      GPIOA->BSRR = (1 << 11); // light up PA11
//      //GPIOA->ODR |= GPIO_ODR_OD11;
//    }
//  }
//}





//int main(void)
//{
//  RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
//  GPIOC->MODER &= ~(3 << (2 * 5));
//  GPIOC->MODER |= (1 << (2 * 5)); // gpio init
//
//  RCC->APBENR1 |= RCC_APBENR1_TIM7EN; // turn on clock signal on TIM7
//  TIM7->PSC = 15999; // prescaler register
//  TIM7->ARR = 10; // auto-reload register
//  // T = (PSC - 1) * ARR / 16MHz (SystemCoreClock)
//  TIM7->DIER |= TIM_DIER_UIE; // interupt enable (peryfery)
//  NVIC_SetPriority(TIM7_LPTIM2_IRQn, 0); // interupt prioryty: 0 (arm-core)
//  NVIC_EnableIRQ(TIM7_LPTIM2_IRQn); // interupt enable (arm-core)
//  TIM7->CR1 |= TIM_CR1_CEN; // TIM7 enable
//
//  while(1) { __NOP(); }
//}
//
//void TIM7_LPTIM2_IRQHandler(void)
//{
//  if(TIM7->SR & TIM_SR_UIF) { // if TIM7 auto-reload event
//    GPIOC->ODR ^= (1 << 5); // blink led
//    TIM7->SR &= ~TIM_SR_UIF; // clear TIM7 auto-reload flag
//  }
//}








