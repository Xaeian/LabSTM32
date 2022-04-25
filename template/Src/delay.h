#ifndef DELAY_H_
#define DELAY_H_

#include <stdbool.h>
#include <stdint.h>
#include "stm32g0xx.h"

//---------------------------------------------------------------------------------------------------------------------

#ifndef DELAY_HARDWARE
  #define DELAY_HARDWARE 1
#endif

//---------------------------------------------------------------------------------------------------------------------

#if(DELAY_HARDWARE)
  #ifndef DELAY_TIM
    #define DELAY_TIM 6
  #endif
  #if(DELAY_TIM == 6)
    #define _DELAY_TIM TIM6
    #define _DELAY_HANDLER TIM6_DAC_LPTIM1_IRQHandler
    #define _DELAY_N TIM6_DAC_LPTIM1_IRQn
  #elif(DELAY_TIM == 7)
    #define _DELAY_TIM TIM7
    #define _DELAY_HANDLER TIM7_LPTIM2_IRQHandler
    #define _DELAY_N TIM7_LPTIM2_IRQn
  #endif
  void delay_init(void);
  void delay_ms(uint16_t ms);
  void delay_us(uint16_t us);
#else
  #define delay_ms(ms) for(int _i = 1592 * ms; _i; _i--)
#endif

//---------------------------------------------------------------------------------------------------------------------
#endif
