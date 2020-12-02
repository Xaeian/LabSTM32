#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32g0xx.h"
#define delay_ms(ms) for(int i = 3203*ms; i; i--) __NOP()






int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // GPIO-A Enable
  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0; // PA5 Output

  RCC->CSR |= RCC_CSR_LSION;
  while(RCC->CSR & RCC_CSR_LSIRDY);


  delay_ms(200);
  GPIOA->BSRR |= GPIO_BSRR_BS5;
  delay_ms(200);
  GPIOA->BSRR |= GPIO_BSRR_BR5;




  delay_ms(100);

  RCC->IOPENR |= RCC_IOPENR_GPIOCEN; // GPIO-C Enable
  GPIOC->MODER &= ~GPIO_MODER_MODE13; // PC13 Input










  /*


  //


  SysTick_Config(16000000);
  */

  while(1)
  {
    if(!(GPIOC->IDR & (1 << 13)));
    else
    {

    }
  }
}

void SysTick_Handler(void)
{

}



//OK :)


