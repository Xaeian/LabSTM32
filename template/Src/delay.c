#include "delay.h"

//---------------------------------------------------------------------------------------------------------------------
#if(DELAY_HARDWARE)

void delay_init(void)
{
  RCC->APBENR1 |= RCC_APBENR1_TIM6EN;
  NVIC_SetPriority(_DELAY_N, 0);
  NVIC_EnableIRQ(_DELAY_N);
}

volatile bool delay_end_flag;

inline static void _delay(uint16_t tick)
{
  _DELAY_TIM->ARR = tick;
  _DELAY_TIM->DIER |= TIM_DIER_UIE;
  delay_end_flag = false;
  _DELAY_TIM->CR1 |= TIM_CR1_OPM | TIM_CR1_CEN;
  while(!delay_end_flag) __WFI();
}

void delay_ms(uint16_t ms)
{
  _DELAY_TIM->PSC = (SystemCoreClock / 1000) - 1;
  _delay(ms);
}

void delay_us(uint16_t us)
{
  _DELAY_TIM->PSC = (SystemCoreClock / 1000000) - 1;
  _delay(us);
}

//---------------------------------------------------------------------------------------------------------------------

void _DELAY_HANDLER(void)
{
  if(_DELAY_TIM->SR & TIM_SR_UIF) {
  delay_end_flag = true;
  _DELAY_TIM->SR &= ~TIM_SR_UIF;
  }
}

//---------------------------------------------------------------------------------------------------------------------
#endif
