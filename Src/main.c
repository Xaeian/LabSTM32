#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32g0xx.h"
#define delay_ms(ms) for(int i = 3203*ms; i; i--) __NOP()

int main(void)
{
  RCC->IOPENR |= (1 << 0);
  GPIOA->MODER = (1 << (2 * 5));

  while(1)
  {
    GPIOA->ODR |= (1 << 5);
    delay_ms(200);
    GPIOA->ODR &= ~(1 << 5);
    delay_ms(200);
  }
}
