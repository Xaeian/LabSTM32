#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"
#include "gpio.h"

// #define delay_ms(ms) for(int _i = 3203  *ms; _i; _i--) __NOP()

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


GPIO_t app_led = { .gpio_typedef = GPIOC, .pin_no = 7, .mode = GPIO_Mode_Output };
GPIO_t app_sw = { .gpio_typedef = GPIOA, .pin_no = 12, .mode = GPIO_Mode_Input, .pull = GPIO_Pull_PullUp, .revers = true };


uint8_t vaule;

int main(void)
{
  delay_init();
  GPIO_Init(&app_led);
  GPIO_Init(&app_sw);

  while(1)
  {
	  delay_ms(200);
	  if(GPIO_In(&app_sw)) {
	    GPIO_Set(&app_led);
	  }
	  else {
	    GPIO_Rst(&app_led);
	  }

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

