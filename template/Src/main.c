#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"
#include "exstring.h"
#include "hc595.h"

#define delay_ms(ms) for(int _i = 1592 * ms; _i; _i--);

void adc_conf(void)
{
  RCC->IOPENR |= RCC_IOPSMENR_GPIOBSMEN;
  RCC->APBENR2 |= RCC_APBENR2_ADCEN;

  ADC1->CR |= ADC_CR_ADVREGEN; // ADC Voltage Regulator Enable
  for(uint32_t i = 0; i < SystemCoreClock / 500000; i++) __DSB();

  ADC1->CR &= ~ADC_CR_ADEN;
  ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;
  ADC1->CR |= ADC_CR_ADCAL; // Calibration
  while(!(ADC1->ISR & ADC_ISR_EOCAL)) __DSB();
  ADC1->ISR |= ADC_ISR_EOCAL;

  ADC->CCR |= (7 << ADC_CCR_PRESC_Pos);
  ADC1->CHSELR = (1 << 9); // Active channel ADC_IN9 - PB1

  NVIC_EnableIRQ(ADC1_COMP_IRQn);
  NVIC_SetPriority(ADC1_COMP_IRQn, 3);

  ADC1->CFGR2 = (4 << ADC_CFGR2_OVSS_Pos) | (7 << ADC_CFGR2_OVSR_Pos) | ADC_CFGR2_OVSE;
  ADC1->SMPR = (7 << ADC_SMPR_SMP1_Pos);

  do {
    ADC1->CR |= ADC_CR_ADEN;
  } while((ADC1->ISR & ADC_ISR_ADRDY) == 0);
}

void uart_conf(void)
{
  RCC->IOPENR |= RCC_IOPSMENR_GPIOCSMEN;
  GPIOC->MODER &= ~(GPIO_MODER_MODE4_Msk | GPIO_MODER_MODE5_Msk);
  GPIOC->MODER |= (2 << GPIO_MODER_MODE4_Pos) | (2 << GPIO_MODER_MODE5_Pos);
  GPIOC->AFR[0] &= ~(GPIO_AFRL_AFSEL4_Msk | GPIO_AFRL_AFSEL5_Msk);
  GPIOC->AFR[0] |= (1 << GPIO_AFRL_AFSEL4_Pos) | (1 << GPIO_AFRL_AFSEL5_Pos);

  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
  USART1->BRR = SystemCoreClock / 9600;
  USART1->CR3 |= USART_CR3_OVRDIS;
  while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC);
  USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

  USART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
  NVIC_SetPriority(USART1_IRQn, 2);
  NVIC_EnableIRQ(USART1_IRQn);

  USART1->ICR |= USART_ICR_TCCF;
  USART1->RQR |= USART_RQR_RXFRQ;
}

void adc_start()
{
  ADC1->IER |= ADC_IER_EOCIE;
  ADC1->CR |= ADC_CR_ADSTART;
}

uint16_t volatile adc_output;
uint8_t volatile adc_flag;
uint8_t volatile uart_read;

void uart_send_dec(uint16_t nbr)
{
  char *str = itoa_dec(nbr);
  while(*str)
  {
    USART1->TDR = *str;
    str++;
    delay_ms(5);
  }
  USART1->TDR = '\r';
  delay_ms(5);
  USART1->TDR = '\n';
  delay_ms(5);
}

int main(void)
{
  uart_conf();
  adc_conf();
  adc_start();
  HC595_Begin();
  while(1)
  {
    if(adc_flag)
    {
      adc_flag = 0;
      HC595_Dec(adc_output);
      HC595_Send();
      uart_send_dec(adc_output);
      adc_start();
    }
  }
}

void ADC_COMP_IRQHandler(void)
{
  if(ADC1->ISR & ADC_ISR_EOC)
  {
    ADC1->ISR |= ADC_ISR_EOC;
    adc_output = ADC1->DR;
    adc_flag = 1;
  }
}

void USART1_IRQHandler(void)
{
  if(USART1->ISR & USART_ISR_RXNE_RXFNE)
  {
    uart_read = (uint8_t)(USART1->RDR);
  }
}
