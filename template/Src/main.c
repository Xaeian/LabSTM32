#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"
#include "gpio.h"

#define DELAY_TIM 0

#if(DELAY_TIM)

void delay_init(void)
{
  RCC->APBENR1 |= RCC_APBENR1_TIM6EN;
  NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn, 0);
  NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn);
}

volatile bool delay_end_flag;

inline static void _delay(uint16_t tick)
{
  TIM6->ARR = tick;
  TIM6->DIER |= TIM_DIER_UIE;
  delay_end_flag = false;
  TIM6->CR1 |= TIM_CR1_OPM | TIM_CR1_CEN;
  while(!delay_end_flag) __WFI();
}

void delay_ms(uint16_t ms)
{
  TIM6->PSC = (SystemCoreClock / 1000) - 1;
  _delay(ms);
}

void delay_us(uint16_t us)
{
  TIM6->PSC = (SystemCoreClock / 1000000) - 1;
  _delay(us);
}

void TIM6_DAC_LPTIM1_IRQHandler(void)
{
  if(TIM6->SR & TIM_SR_UIF) {
	delay_end_flag = true;
	TIM6->SR &= ~TIM_SR_UIF;
  }
}

#else

#define delay_ms(ms) for(int _i = 3203  *ms; _i; _i--) __NOP()

#endif

void PWM_Init(void)
{
  RCC->APBENR2 |= RCC_APBENR2_TIM1EN;

//  TIM1->EGR &= ~TIM_EGR_UG;
//  TIM1->CR1 &= ~TIM_CR1_CEN;

  TIM1->CCER = TIM_CCER_CC3E | TIM_CCER_CC2E | TIM_CCER_CC1E; // Output enable
  TIM1->CCMR1 = TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 |
                TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
  TIM1->CCMR2 = TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
  // PWM mode

  TIM1->PSC = 160;
  TIM1->ARR = 1000;

  TIM1->DIER &= ~TIM_DIER_UIE;
  TIM1->BDTR |= TIM_BDTR_MOE;
  TIM1->EGR |= TIM_EGR_UG; // Fuse on
  TIM1->CR1 |= TIM_CR1_CEN; // TIM enable
}

GPIO_t app_led = { .gpio_typedef = GPIOC, .pin_no = 8, .mode = GPIO_Mode_Output };
GPIO_t app_sw = { .gpio_typedef = GPIOA, .pin_no = 12, .mode = GPIO_Mode_Input, .pull = GPIO_Pull_PullUp, .revers = true };
GPIO_t pwm1_gpio = { .gpio_typedef = GPIOA, .pin_no = 8, .mode = GPIO_Mode_Alternate, .alternate = 2 };
GPIO_t pwm2_gpio = { .gpio_typedef = GPIOA, .pin_no = 9, .mode = GPIO_Mode_Alternate, .alternate = 2 };
GPIO_t pwm3_gpio = { .gpio_typedef = GPIOA, .pin_no = 10, .mode = GPIO_Mode_Alternate, .alternate = 2 };

uint8_t vaule;

int main(void)
{
  //delay_init();
  GPIO_Init(&app_led);

  GPIO_Set(&app_led);
  GPIO_Init(&app_sw);

  //----------------------PWM

  GPIO_Init(&pwm1_gpio);
  GPIO_Init(&pwm2_gpio);
  GPIO_Init(&pwm3_gpio);
  PWM_Init();

  //------------------

  uint16_t x1 = 0;
  uint16_t x2 = 0;
  uint16_t x3 = 0;

//  TIM1->CCR1 = 200;
//  TIM1->CCR1 = 500;
//  TIM1->CCR1 = 900;

  while(1)
  {
    TIM1->CCR1 = x1;
    x1 += (x1 / 6) + 1;
    if(x1 > 1000) x1 = 0;

    TIM1->CCR2 = x2;
    x2 += (x2 / 8) + 2;
    if(x2 > 1000) x2 = 0;

    TIM1->CCR3 = x3;
    x3 += (x3 / 10) + 1;
    if(x3 > 1000) x3 = 0;

    delay_ms(5);
//	  delay_ms(50);
//	  if(GPIO_In(&app_sw)) {
//	    GPIO_Set(&app_led);
//	  }
//	  else {
//	    GPIO_Rst(&app_led);
//	  }
  }
}

void Blink(void)
{
	GPIOC->ODR ^= (1 << 5);
	TIM6->ARR += 5;
	if(TIM6->ARR > 200) TIM6->ARR = 20;
}




void SysTick_Handler(void)
{
//  x++;
//  if(x > 2) {
//    x = 0;
//	GPIOC->ODR ^= (1 << 5);
//  }

}

