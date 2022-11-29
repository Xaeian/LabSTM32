#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"
#include "rtc.h"

#define delay_ms(ms) for(int _i = 1592 * ms; _i; _i--);

bool state = false;

RTC_Datetime_t datetime_start = {
  .year = 22,
  .month = 11,
  .month_day = 29,
  .hour = 19,
  .minute = 15,
  .second = 30
};

RTC_Alarm_t alarm_start = {
  .day_mask = true,
  .hour = 19,
  .minute = 16,
  .second = 0
};

int main(void)
{
  RTC_Init();
  RTC_SetDatetime(&datetime_start);
  RTC_Alarm_A_Enable(&alarm_start);
  while(1)
  {
    if(RTC_Event_A()) {
      RTC_Datetime_t datetime_now = RTC_Datetime();
      __NOP();
    }
    __NOP();
  }
}














//int main(void)
//{
//  RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
//  GPIOC->MODER = 0;
//  GPIOC->MODER |= 0x55555555;
//  GPIOC->ODR = 1;
//
//  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
//  GPIOA->MODER &= ~GPIO_MODER_MODE15;
//  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD15;
//  GPIOA->PUPDR |= GPIO_PUPDR_PUPD15_0;
//
//  while(1)
//  {
//    delay_ms(35);
//    if(!state) GPIOC->ODR = (GPIOC->ODR << 1);
//    else GPIOC->ODR = (GPIOC->ODR >> 1);
//    if(~GPIOA->IDR & (1 << 15)) {
//      state ^= 1;
//      delay_ms(200);
//    }
//    if(GPIOC->ODR == (1 << 15)) state = true;
//    else if(GPIOC->ODR == (1 << 0)) state = false;
//  }
//}
