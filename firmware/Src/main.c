#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32g0xx.h"
#define delay_ms(ms) for(int i = 3203*ms; i; i--) __NOP()


#define RESET_VALUE -100

int main(void)
{

  // Zegar magistrali A







  RCC->IOPENR |= RCC_IOPSMENR_GPIOASMEN;
  GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
  GPIOA->MODER |= (2 << GPIO_MODER_MODE8_Pos) | (2 << GPIO_MODER_MODE9_Pos) | (2 << GPIO_MODER_MODE10_Pos);

  GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk);
  GPIOA->AFR[1] |= (2 << GPIO_AFRH_AFSEL8_Pos) |(2 << GPIO_AFRH_AFSEL9_Pos) | (2 << GPIO_AFRH_AFSEL10_Pos);

  RCC->APBENR2 |= RCC_APBENR2_TIM1EN;

  //TIM1->EGR &= ~TIM_EGR_UG;
  //TIM1->CR1 &= ~TIM_CR1_CEN;

  //TIM1->CCER = 0;

  TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E;

  TIM1->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM1->CCMR1 |= TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
  TIM1->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;

  TIM1->PSC = 1599;
  TIM1->ARR = 100;

  TIM1->CR1 = TIM_CR1_ARPE;


  TIM1->DIER &= ~TIM_DIER_UIE;
  TIM1->BDTR |= TIM_BDTR_MOE;
  TIM1->EGR |= TIM_EGR_UG;
  TIM1->CR1 |= TIM_CR1_CEN;

  int8_t val = RESET_VALUE;

  while(1)
  {
    if(val < 0) TIM1->CCR1 = 0;
    else TIM1->CCR1 = val;
    val++;
    if(val > 100) val = RESET_VALUE;
    delay_ms(2);
  }
}


