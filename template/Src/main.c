#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"
#include "gpio.h"
#include "delay.h"

//---------------------------------------------------------------------------------------------------------------------

GPIO_t pwm1_gpio = { .gpio_typedef = GPIOA, .pin_no = 8, .mode = GPIO_Mode_Alternate, .alternate = 2 };
GPIO_t pwm2_gpio = { .gpio_typedef = GPIOA, .pin_no = 9, .mode = GPIO_Mode_Alternate, .alternate = 2 };
GPIO_t pwm3_gpio = { .gpio_typedef = GPIOA, .pin_no = 10, .mode = GPIO_Mode_Alternate, .alternate = 2 };

void PWM_Init(void)
{
  GPIO_Init(&pwm1_gpio); // set PA8 on 2 alternate function
  GPIO_Init(&pwm2_gpio); // ... PA9 ...
  GPIO_Init(&pwm3_gpio); // ... PA10 ...

  RCC->APBENR2 |= RCC_APBENR2_TIM1EN; // turn on the clock on TIM1

  TIM1->CCER = TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E; // output enable
  TIM1->CCMR1 = TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 |
                TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM1->CCMR2 = TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1; // PWM mode

  TIM1->PSC = 159;
  TIM1->ARR = 1000;
  // T = (PSC - 1) * ARR / SystemCoreClock

  TIM1->DIER &= ~TIM_DIER_UIE;
  TIM1->BDTR |= TIM_BDTR_MOE;
  TIM1->EGR |= TIM_EGR_UG; // turn off Fuse
  TIM1->CR1 |= TIM_CR1_CEN; // TIM enable
}

//---------------------------------------------------------------------------------------------------------------------

int16_t diff(int16_t value, uint8_t a, uint8_t b)
{
  return (value / a) + b;
}

int main(void)
{
  delay_init();
  PWM_Init();

  int16_t value1 = 0;
  // int16_t value2 = 0;
  // int16_t value3 = 0;
  bool state1 = true;
  // bool state2 = true;
  // bool state3 = true;

  while(1)
  {
    TIM1->CCR1 = value1;
    delay_ms(5);
    if(state1) {
      value1 += diff(value1, 6, 1);
      if(value1 > 1000) {
        value1 = 1000;
        state1 = !state1;
      }
    } else {
      value1 += diff(value1, 6, 1);
      if(value1 < 0) {
        value1 = 0;
        state1 = !state1;
      }
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------

void SysTick_Handler(void)
{
  // ...
}
//---------------------------------------------------------------------------------------------------------------------
