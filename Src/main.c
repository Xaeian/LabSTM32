#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32g0xx.h"
#define delay_ms(ms) for(int i = 3203*ms; i; i--) __NOP()

#define IWDG_REFRESH 0x0000AAAA
#define IWDG_WRITE_ACCESS 0x00005555
#define IWDG_START 0x0000CCCC

typedef enum
{
  IWDG_Time_125us = 0,
  IWDG_Time_250us = 1,
  IWDG_Time_500us = 2,
  IWDG_Time_1ms = 3,
  IWDG_Time_2ms = 4,
  IWDG_Time_4ms = 5,
  IWDG_Time_8ms = 6
}
IWDG_Time_e;

/*
bool IWDG_Status(void)
{
  if(RCC->CSR & RCC_CSR_IWDGRSTF)
  {
    RCC->CSR |= RCC_CSR_RMVF;
    return 1;
  }
  return 0;
}
*/

int main(void)
{


  RCC->CSR |= RCC_CSR_LSION;
  while(RCC->CSR & RCC_CSR_LSIRDY);

  IWDG->KR = IWDG_START;
  IWDG->KR = IWDG_WRITE_ACCESS;
  IWDG->PR = IWDG_Time_4ms;
  IWDG->RLR = 500;
  while(IWDG->SR);
  delay_ms(100);
  IWDG->KR = IWDG_REFRESH;

  RCC->IOPENR |= RCC_IOPENR_GPIOCEN; // GPIO-C Enable
  GPIOC->MODER &= ~GPIO_MODER_MODE13; // PC13 Input

  RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // GPIO-A Enable
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0; // PA5 Output
  GPIOA->BSRR |= GPIO_BSRR_BS5;

  /*
  EXTI->EXTICR[3] |= (2 << 8);
  EXTI->FTSR1 |= EXTI_FTSR1_FT13;
  EXTI->RTSR1 |= EXTI_RTSR1_RT13;
  EXTI->IMR1 |= EXTI_IMR1_IM13;
  NVIC_EnableIRQ(EXTI4_15_IRQn);

  // ARM <-- Rdzeń  < === SysTick
  // ST <-- Peryfgeria <-- TIM1, TIM2 ... TIM17
  // 16MHz
  /*
  RCC->APBENR1 |= RCC_APBENR1_TIM7EN;

  TIM7->PSC = 15999;
  TIM7->ARR = 1000;

  TIM7->DIER |= TIM_DIER_UIE;
  NVIC_EnableIRQ(TIM7_LPTIM2_IRQn);

  TIM7->CR1 |= TIM_CR1_CEN;
  TIM7->SR &= ~TIM_SR_UIF;

  SysTick_Config(16000000);
  */

  while(1)
  {
    if(!(GPIOC->IDR & (1 << 13)));
    else
    {
      IWDG->KR = IWDG_REFRESH;
    }
  }
}

void SysTick_Handler(void)
{

}

void TIM7_LPTIM2_IRQHandler(void)
{
  if(TIM7->SR & TIM_SR_UIF)
  {
    TIM7->SR &= ~TIM_SR_UIF;
    //GPIOA->ODR ^= GPIO_ODR_OD5;
  }
}

//OK :)

void EXTI4_15_IRQHandler(void)
{
  if(EXTI->FPR1 & (1 << 13))
  {
    EXTI->FPR1 |= (1 << 13);
    GPIOA->BSRR |= GPIO_BSRR_BS5;
  }

  if(EXTI->RPR1 & (1 << 13))
  {
    EXTI->RPR1 |= (1 << 13);
    GPIOA->BSRR |= GPIO_BSRR_BR5;
  }
}




