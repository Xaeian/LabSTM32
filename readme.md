Gdzie można kupić płytkę **NUCLEO**?

- [STM32G070RB Kamami](https://kamami.pl/stm-nucleo-64/573439-nucleo-g070rb-zestaw-startowy-z-mikrokontrolerem-z-rodziny-stm32-stm32g070rb.html?search_query=stm32g0+nucleo&results=4)
- [STM32G071B1 Kamami](https://kamami.pl/stm-nucleo-64/584254-nucleo-g0b1re-zestaw-startowy-z-mikrokontrolerem-z-rodziny-stm32-stm32g0b1re.html?search_query=stm32g0+nucleo&results=4)
- [STM32G071RB Mouser](https://www.mouser.pl/ProductDetail/STMicroelectronics/NUCLEO-G071RB?qs=PqoDHHvF64%2FiIo1nQ3ZWtw%3D%3D)
- [STM32G070RB Farnell](https://www.mouser.pl/ProductDetail/STMicroelectronics/NUCLEO-G071RB?qs=PqoDHHvF64%2FiIo1nQ3ZWtw%3D%3D)

## 1. Świecąca linijka LED

Wyprowadzenia `PC0`-`PC15` są podłączone do kolejnych diod.
W programie jedynka z rejestru `GPIOC->ODR` jest przesuwana na lewo (`<<`).
Gdy dochodzi do diody `PC15` kierunek zmienia się na prawo (`>>`).
Całość sprawia wrażenie, że święcący punk przesuwa się na lewo i prawo.

```cpp
#define delay_ms(ms) for(int _i = 3203 * ms; _i; _i--) __NOP()

bool state = false;

int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
  GPIOC->MODER = 0;
  GPIOC->MODER |= 0x55555555;
  GPIOC->ODR = 1;

  while(1)
  {
    delay_ms(20);
    if(!state) GPIOC->ODR = (GPIOC->ODR << 1);
    else GPIOC->ODR = (GPIOC->ODR >> 1);
    if(GPIOC->ODR == (1 << 15)) state = true;
    else if(GPIOC->ODR == (1 << 0)) state = false;
  }
}
```

## Przycisk i dioda

Gdy przycisk `PA13` jest wciśnięty dioda LED `PC5` świeci.
W przeciwnym wypadku dioda LED pozostaje zgaszona

```cpp
int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOCEN; // turn on clock signal on GPIOC
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // turn on clock signal on GPIOA

  GPIOC->MODER &= ~(3 << (2 * 5));
  GPIOC->MODER |= (1 << (2 * 5)); // PC5: output

  GPIOA->MODER &= ~(3 << (2 * 13)); // PA13: input
  GPIOA->PUPDR |= (1 << (2 * 13)); // PA13: pull-up

  while(1)
  {
    if(~GPIOA->IDR & (1 << 13)) // if(switch PA13 is clicked)
    {
      GPIOC->BSRR = (1 << 5); // light up PC5
    }
    else
    {
      GPIOC->BRR = (1 << 5); // put out PC5
    }
  }
}
```

# Timery

Ustaliliśmy, już że nasz procek jest owocem pracy dwóch firm. ST Microelectronic i ARM. Zatem nie powinn

## Licznik SysTick

SysTick jest bardzo prostym licznikim 24-bitowym licznikiem, który jedyne co może robić to zliczać w górę takt sygnału zegarowego. Podczas konfiguracji `SysTick_Config` określamy do jakiej wartości zliczanie ma się odbywać. Gdy to nastąpi wartość rejestru zliczającego jest zerowana, a rdzeń przestaje wykonywać bierzące zadanie i przechodzi do funkcji `SysTick_Handler`. Taką zagrywkę ze strony mikrokontrolera nazwyamy przerwaniem, a funkcję jego obsługą.

W przykładzie licznik ten jest skonfigurowany, aby przerwanie występowało co `100ms`, a w obsłudze przetwania dioda podłączona do `PD5` zmienia stan na przeciwny.

```cpp
int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
  GPIOC->MODER &= ~(3 << (2 * 5));
  GPIOC->MODER |= (1 << (2 * 5));
  SysTick_Config(SystemCoreClock / 10); // 1s / 10 = 100ms
  while(1) __NOP();
}

void SysTick_Handler(void)
{
  GPIOC->ODR ^= (1 << 5);
}
```

Nasz licznik maksymalniae 2^12

```cpp
uint8_t value;
void SysTick_Handler(void)
{
  if(value++ > 2) {
    x = 0;
    GPIOC->ODR ^= (1 << 5);
  }
}
```

## Licznik SysTick

```cpp
int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
  GPIOC->MODER &= ~(3 << (2 * 5));
  GPIOC->MODER |= (1 << (2 * 5));

  RCC->APBENR1 |= RCC_APBENR1_TIM6EN;
  TIM6->PSC = 15999;
  TIM6->ARR = 100;
  TIM6->DIER |= TIM_DIER_UIE; // Peryferium
  NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn, 0);
  NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn); // Rdzeń ARM

  TIM6->CR1 |= TIM_CR1_CEN;





  while(1)
  {
//    GPIOC->ODR |= (1 << 5);
//    delay_ms(200);
//    GPIOC->ODR &= ~(1 << 5);
//    delay_ms(200);
  }
}

void Blink(void)
{
	GPIOC->ODR ^= (1 << 5);
	TIM6->ARR += 5;
	if(TIM6->ARR > 200) TIM6->ARR = 20;
}


void TIM6_DAC_LPTIM1_IRQHandler(void)
{
  if(TIM6->SR & TIM_SR_UIF) {
	Blink();
	TIM6->SR &= ~TIM_SR_UIF;
  }
}

void SysTick_Handler(void)
{
//  x++;
//  if(x > 2) {
//    x = 0;
//	GPIOC->ODR ^= (1 << 5);
//  }

}
```
