#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32g0xx.h"
#define delay_ms(ms) for(int i = 3203*ms; i; i--) __NOP()





int main(void)
{

  // Zegar magistrali A




  RCC->IOPENR |= RCC_IOPSMENR_GPIOASMEN;
  GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
  GPIOA->MODER |= (1 << GPIO_MODER_MODE8_Pos) | (1 << GPIO_MODER_MODE9_Pos) | (1 << GPIO_MODER_MODE10_Pos);

  GPIOA->ODR |= GPIO_ODR_OD8; //R

  GPIOA->ODR &= ~GPIO_ODR_OD8; // G
  GPIOA->ODR |= GPIO_ODR_OD9;

  GPIOA->ODR &= ~GPIO_ODR_OD9;
  GPIOA->ODR |= GPIO_ODR_OD10; //B

  GPIOA->ODR &= ~GPIO_ODR_OD10;


  TIM1: RCC->APBENR2 |= RCC_APBENR2_TIM1EN;

  pwm->tim_typedef->EGR &= ~TIM_EGR_UG;
  pwm->tim_typedef->CR1 &= ~TIM_CR1_CEN;
  pwm->tim_typedef->CCER = 0;


  pwm->tim_typedef->CCER |= ((invert_neg << 3) | TIM_CCER_CC1NE | (invert_pos << 1) | TIM_CCER_CC1E) << (4 * channel);

  TIM1->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; break;
  TIM1->CCMR1 |= TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; break;
  TIM1->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1; break;

  while(1)
  {
    __NOP();
  }
}


