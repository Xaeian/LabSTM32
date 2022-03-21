#include <stdint.h>
#include "system_stm32g0xx.h"

#define __weak __attribute__((weak))

// Constants for linker
extern uint32_t _sidata; // Start address of init data
extern uint32_t _sdata; // Start address of data
extern uint32_t _edata; // End address of data
extern uint32_t _sbss; // Start address of bss
extern uint32_t _ebss; // End address of bss
extern uint32_t _estack; // Top od stack

extern void __libc_init_array(); // Static constructor initializator form libc
extern int main(); // Main program point

void Reset_Handler(void) {
  uint32_t *dataInit = &_sidata;
  uint32_t *data = &_sdata;
  while(data < &_edata) {
    *data++ = *dataInit++;
  }

  uint32_t *bss = &_sbss;
  while(bss < &_ebss) {
    *bss++ = 0;
  }

  SystemInit();
  __libc_init_array();
  main();

  while(1);
}

void Default_Handler(void) {
  while(1);
}

void Void_Handler(void *object) {
}

__weak void HardFault_Handler(void) {
  while(1);
}

__weak void NMI_Handler(void) { Default_Handler(); }
__weak void	SVC_Handler(void) { Default_Handler(); }
__weak void	PendSV_Handler(void) { Default_Handler(); }
__weak void	SysTick_Handler(void) { Default_Handler(); }
__weak void	WWDG_IRQHandler(void) { Default_Handler(); }
__weak void	PVD_IRQHandler(void) { Default_Handler(); }
__weak void	RTC_STAMP_IRQHandler(void) { Default_Handler(); }
__weak void	FLASH_IRQHandler(void) { Default_Handler(); }
__weak void	RCC_IRQHandler(void) { Default_Handler(); }
__weak void UCPD1_UCPD2_IRQHandler(void) { Default_Handler(); }
__weak void CEC_IRQHandler(void) { Default_Handler(); }
__weak void ADC_COMP_IRQHandler(void) { Default_Handler(); }
__weak void	EXTI0_1_IRQHandler(void) { Default_Handler(); }
__weak void	EXTI2_3_IRQHandler(void) { Default_Handler(); }
__weak void	EXTI4_15_IRQHandler(void) { Default_Handler(); }
__weak void	DMA_Channel1_IRQHandler(void) { Default_Handler(); }
__weak void	DMA_Channel2_3_IRQHandler(void) { Default_Handler(); }
__weak void	DMA_Channel4_5_6_7_IRQHandler(void) { Default_Handler(); }
__weak void	TIM1_BRK_UP_TRG_COMP_IRQHandler(void) { Default_Handler(); }
__weak void	TIM1_CC_IRQHandler(void) { Default_Handler(); }
__weak void	TIM2_IRQHandler(void) { Default_Handler(); }
__weak void	TIM3_IRQHandler(void) { Default_Handler(); }
__weak void	TIM6_DAC_LPTIM1_IRQHandler(void) { Default_Handler(); }
__weak void	TIM7_LPTIM2_IRQHandler(void) { Default_Handler(); }
__weak void	TIM14_IRQHandler(void) { Default_Handler(); }
__weak void	TIM15_IRQHandler(void) { Default_Handler(); }
__weak void	TIM16_IRQHandler(void) { Default_Handler(); }
__weak void	TIM17_IRQHandler(void) { Default_Handler(); }
__weak void	I2C1_IRQHandler(void) { Default_Handler(); }
__weak void	I2C2_IRQHandler(void) { Default_Handler(); }
__weak void	SPI1_IRQHandler(void) { Default_Handler(); }
__weak void	SPI2_IRQHandler(void) { Default_Handler(); }
__weak void	USART1_IRQHandler(void) { Default_Handler(); }
__weak void	USART2_IRQHandler(void) { Default_Handler(); }
__weak void	USART3_USART4_LPUART1_IRQHandler(void) { Default_Handler(); }

__attribute__((section(".isr_vector")))
const void (*IRQ[])(void) = {
  (const void (*)(void)) &_estack,
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  SVC_Handler,
  0,
  0,
  PendSV_Handler,
  SysTick_Handler,
  WWDG_IRQHandler,
  PVD_IRQHandler,
  RTC_STAMP_IRQHandler,
  FLASH_IRQHandler,
  RCC_IRQHandler,
  EXTI0_1_IRQHandler,
  EXTI2_3_IRQHandler,
  EXTI4_15_IRQHandler,
  UCPD1_UCPD2_IRQHandler,
  DMA_Channel1_IRQHandler,
  DMA_Channel2_3_IRQHandler,
  DMA_Channel4_5_6_7_IRQHandler,
  ADC_COMP_IRQHandler,
  TIM1_BRK_UP_TRG_COMP_IRQHandler,
  TIM1_CC_IRQHandler,
  TIM2_IRQHandler,
  TIM3_IRQHandler,
  TIM6_DAC_LPTIM1_IRQHandler,
  TIM7_LPTIM2_IRQHandler,
  TIM14_IRQHandler,
  TIM15_IRQHandler,
  TIM16_IRQHandler,
  TIM17_IRQHandler,
  I2C1_IRQHandler,
  I2C2_IRQHandler,
  SPI1_IRQHandler,
  SPI2_IRQHandler,
  USART1_IRQHandler,
  USART2_IRQHandler,
  USART3_USART4_LPUART1_IRQHandler,
  CEC_IRQHandler
};
