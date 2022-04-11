#include "gpio.h"

//---------------------------------------------------------

void GPIO_Init(GPIO_t *gpio)
{
  switch((uint32_t)gpio->gpio_typedef) {
    case (uint32_t)GPIOA: RCC->IOPENR |= RCC_IOPENR_GPIOAEN; break;
    case (uint32_t)GPIOB: RCC->IOPENR |= RCC_IOPENR_GPIOBEN; break;
    case (uint32_t)GPIOC: RCC->IOPENR |= RCC_IOPENR_GPIOCEN; break;
    case (uint32_t)GPIOD: RCC->IOPENR |= RCC_IOPENR_GPIODEN; break;
    case (uint32_t)GPIOF: RCC->IOPENR |= RCC_IOPENR_GPIOFEN; break;
  }
  gpio->gpio_typedef->MODER &= ~(0b11 << (2 * gpio->pin_no));
  gpio->gpio_typedef->MODER |= gpio->mode << (2 * gpio->pin_no);
  gpio->gpio_typedef->PUPDR &= ~(0b11 << (2 * gpio->pin_no));
  gpio->gpio_typedef->PUPDR |= gpio->pull << (2 * gpio->pin_no);
}

//---------------------------------------------------------

void GPIO_Set(GPIO_t *gpio)
{
  gpio->gpio_typedef->BSRR = (1 << gpio->pin_no);
}

void GPIO_Rst(GPIO_t *gpio)
{
  gpio->gpio_typedef->BSRR = (1 << (16 + gpio->pin_no));
}

void GPIO_Tgl(GPIO_t *gpio)
{
  gpio->gpio_typedef->ODR ^= (1 << gpio->pin_no);
}

//---------------------------------------------------------

bool GPIO_In(GPIO_t *gpio)
{
  bool in;
  if(gpio->gpio_typedef->IDR & (1 << gpio->pin_no)) in = true;
  else in = false;
  if(gpio->revers) in = !in;
  return in;
}

//---------------------------------------------------------
