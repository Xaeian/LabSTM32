#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32g0xx.h"
#define delay_ms(ms) for(int i = 3203*ms; i; i--) __NOP()


#define AUTO_RELOAD 1000

int32_t pwm_val(uint8_t *dir, int32_t *val, int32_t inc)
{
  if(!*dir) {
    *val += inc;
    if(*val >= AUTO_RELOAD) { *val = AUTO_RELOAD; *dir ^= 1; }
  }
  else {
    *val -= inc;
    if(*val <= 0) { *val = 0; *dir ^= 1; }
  }
  return *val;
}

int main(void)
{
  RCC->IOPENR |= RCC_IOPSMENR_GPIOASMEN;
  GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
  GPIOA->MODER |= (2 << GPIO_MODER_MODE8_Pos) | (2 << GPIO_MODER_MODE9_Pos) | (2 << GPIO_MODER_MODE10_Pos);

  GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk);
  GPIOA->AFR[1] |= (2 << GPIO_AFRH_AFSEL8_Pos) |(2 << GPIO_AFRH_AFSEL9_Pos) | (2 << GPIO_AFRH_AFSEL10_Pos);

  RCC->APBENR2 |= RCC_APBENR2_TIM1EN;

  TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E;

  TIM1->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM1->CCMR1 |= TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
  TIM1->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;

  TIM1->PSC = 159;
  TIM1->ARR = AUTO_RELOAD;

  TIM1->CR1 = TIM_CR1_ARPE;

  TIM1->DIER &= ~TIM_DIER_UIE;
  TIM1->BDTR |= TIM_BDTR_MOE;
  TIM1->EGR |= TIM_EGR_UG;
  TIM1->CR1 |= TIM_CR1_CEN;

  uint8_t dir[3] = { 0, 0, 0 };
  int32_t val[3] = { 0, 0, 0 };


  while(1)
  {
    TIM1->CCR1 = (uint32_t)pwm_val(&dir[0], &val[0], 2);
    TIM1->CCR2 = (uint32_t)pwm_val(&dir[1], &val[1], 3);
    TIM1->CCR3 = (uint32_t)pwm_val(&dir[2], &val[2], 5);
    delay_ms(1);
  }
}


