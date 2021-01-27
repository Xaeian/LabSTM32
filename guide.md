---
STM32 | Podstawy
---

# 1. Operacje bitowe

TODO

# 2. IDE, Dioda, Przycisk

Moim nieskromnym zdaniem w nauce programowania, _potem w sumie też_, ważne jest żeby osiągać jakieś efekty szybko. Praca z systemami wbudowanymi posiada niekończące się zagadanienia poboczne, jak konfiguracja środowiska, hardware itd. Zatem pominiemy to wszystko, ściągniemy środowisko [**STM32CubeIDE**](https://www.st.com/en/development-tools/stm32cubeide.html), dostarczane **ST**, następnie klonujemy [repozutorium **nucleo**](https://github.com/Xaeian/nucleo) z przygotowanym proejktem bazowym.

    git clone https://github.com/Xaeian/nucleo.git

```c
int main(void)
{
  RCC->IOPENR |= (1 << 0);

  GPIOA->MODER &= ~(3 << (2 * 5));
  GPIOA->MODER |= (1 << (2 * 5));

  while(1)
  {
    GPIOA->ODR |= (1 << 5);
    delay_ms(200);
    GPIOA->ODR &= ~(1 << 5);
    delay_ms(200);
  }
}
```

W nowszych procesorach, które dodatkowo są zoptymalizowane pod kontem aplikacji energooszczędnych wszystkie peryferia trzeba włączyć. Tak samo jest tutaj. Na początku programu wpisujemy 1 do rejestru [`RCC->IOPENR`](http://www.sqrt.pl/datasheet/STM32G0x1.pdf#page=147) na pozycji 0. Tą operacją doprowadzamy sygnał zegarowy do peryferium **GPIOA**.

W kolejnych liniach ustawiamy wyprowadzenie **PA5** na wyjście przez ustawienie wartości `01` do odpowiedniego pola rejestru [`GPIOA->MODER`](http://www.sqrt.pl/datasheet/STM32G0x1.pdf#page=205).

W pętli głównej na przemian ustawiamy i kasujemy bit z rejestru [`GPIOA->ODR`](http://www.sqrt.pl/datasheet/STM32G0x1.pdf#page=207). Te operacje na przemian ustawiają na wyjściu stan wysoki i niski, co skutkuje zapalaniem i gaszeniem diody. Oczywiście efekt ten jest zauważalny dzięki funkcji delay_ms.

```c
#define delay_ms(ms) for(int __i = 3203*ms; __i; __i--) __NOP()
```

W procesorach STM32 nie mamy wbudowanej funkcji `delay_ms` tak jak to działało w mikrokontrolerach **AVR**. Po prostu nie ma takeij potrzeby, ponieważ w procesorze Atmega328P mieliśmy 3 timery z czego w **Arduino** jeden był wykorzystywany do _nie wiem czego_. Procesory **STM32** z rodziny **G0** mamy do dyspozycji 12 timerów (nie licząc watchdoga) i jeden z nich możemy wykorzystać w celu oprogramowania funkcji `delay_ms`. Takie rozwiązanie będzie znacznie bardziej dokładne i precyzyjne. Nie mniej takie rozwiązanie będzie wystarczające w wielu przypadkach.

Na tym etapie warto usunąć linijki zawierające funkcje delay*ms oraz odpalić kod w debugerze - \_ikonka takiego żuka*.

Wracając do samego kodu. W przypadku niektórych rejestrów dobrą praktyką może okazać się zastąpienie przesunięć bitowych definicjami, które są zawartę w plikach nagówkowych dostarczomchy przez ST.

```c
int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

  GPIOA->MODER &= ~GPIO_MODER_MODE5;
  GPIOA->MODER |= GPIO_MODER_MODE5_0;

  while(1)
  {
    GPIOA->ODR |= GPIO_ODR_OD5;
    delay_ms(200);
    GPIOA->ODR &= ~GPIO_ODR_OD5;
    delay_ms(200);
  }
}
```

Oczywiście operacje migania można zrealizować prościej, za pomocą operacji XOR

```c
while(1)
{
  GPIOA->ODR ^= GPIO_ODR_OD5;
  delay_ms(200);
}
```

Najlepszym jednak sposobem jest na zmianę stanu wyjścia jest skorzystanie z rejestru [`GPIOA->BSRR`](http://www.sqrt.pl/datasheet/STM32G0x1.pdf#page=208). Pozwala on _atomowo_ wstawić do rejestru `GPIOA->ODR` 1 lub 0. Instrukcja `GPIOA->ODR |= GPIO_ODR_OD5` w praktyce wykonuje 3 operacje.

- Pobranie wartości z rejestru `GPIOA->ODR`
- Wykonanie sumy bitowej z wartością `GPIO_ODR_OD5`
- Wpisanie do rejestru `GPIOA->ODR` wyniku operacji

W bardziej skomplikowanych aplikacjach na skutek przerwania jakaś instrukcja mogłaby się wcisnąć w środek naszej operacji, co może wprowadzać komplikacje. Korzystając z rejestru `GPIOA->BSRR` procesor wykonuje jedną operacją, co czynie tą operację niepodzielną - _atomową_.

```c
while(1)
{
  GPIOA->BSRR |= GPIO_BSRR_BS5;
  delay_ms(200);
  GPIOA->BSRR |= GPIO_BSRR_BR5;
  delay_ms(200);
}
```

Migająca dioda już za nami - teraz pora na przycisk. Zanim jednak zabierzemy się za jego konfigurację warto przejrzeć się wszystkim [konfiguracją **GPIO**](http://www.sqrt.pl/datasheet/STM32G0x1.pdf#page=197)

Przycisk na płytce **nucleo** jest od razu podciągnięty do zasilania _(pull-up)_, więc wystarczy ustawić na jako wejście i odczytywać jego stan. Niech nasz program świeci diodą w przypadku wciśnięcia przycisku, a w przeciwnym razie niech dioda pozostanie zgaszona

[//]: 197

```c
RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
GPIOC->MODER &= ~GPIO_MODER_MODE13;

while(1)
{
  if(!(GPIOC->IDR & (1 << 13))) GPIOA->BSRR |= GPIO_BSRR_BS5;
  else GPIOA->BSRR |= GPIO_BSRR_BR5;
}
```

```c
while(1)
{
  GPIOA->BSRR |= GPIO_BSRR_BS5;
  //delay_ms(200);
  GPIOA->BSRR |= GPIO_BSRR_BR5;
  //delay_ms(200);
}
```

# 2. Timery

Nasz mikrokontroler został zaprojektowany przez firmę ARM oraz ST. W

//Timery są to liczniki, które
//ARM <- Rdzeń <= SysTick ST <- Peryfgeria <- TIM1, TIM2...TIM17 -> 16MHz

```cpp
RCC->APBENR1 |= RCC_APBENR1_TIM7EN;

TIM7->PSC = 15999;
TIM7->ARR = 1000;

TIM7->DIER |= TIM_DIER_UIE;
NVIC_EnableIRQ(TIM7_LPTIM2_IRQn);

TIM7->CR1 |= TIM_CR1_CEN;
TIM7->SR &= ~TIM_SR_UIF;
```

Handler

```cpp
void TIM7_LPTIM2_IRQHandler(void)
{
  if(TIM7->SR & TIM_SR_UIF)
  {
    TIM7->SR &= ~TIM_SR_UIF;
    //GPIOA->ODR ^= GPIO_ODR_OD5;
  }
}
```

<!---

Pracując na płytkach NUCLEO dobrze jest mieć pod ręką rysunek z oznaczonymi wyprowadzeniamu.

[User manual Nucleo G0](http://www.sqrt.pl/datasheet/NUCLEO-G0.pdf#page=32)





-->

# 3. Przerwania zewnętrzne

Inicjacja

```cpp
EXTI->EXTICR[3] |= (2 << 8);
EXTI->FTSR1 |= EXTI_FTSR1_FT13;
EXTI->RTSR1 |= EXTI_RTSR1_RT13;
EXTI->IMR1 |= EXTI_IMR1_IM13;
NVIC_EnableIRQ(EXTI4_15_IRQn);
```

Handler

```cpp
void EXTI4_15_IRQHandler(void)
{
  if(EXTI->FPR1 & (1 << 13))
  {
    EXTI->FPR1 |= (1 << 13);
    GPIOA->BSRR |= GPIO_BSRR_BS5;
  }

  if(EXTI->RPR1 & (1 << 13))
  {
    EXTI->RPR1 |= (1 << 13);
    GPIOA->BSRR |= GPIO_BSRR_BR5;
  }
}
```

# 4. Watchdog

Nagłówki:

```cpp
#define IWDG_REFRESH 0x0000AAAA
#define IWDG_WRITE_ACCESS 0x00005555
#define IWDG_START 0x0000CCCC

typedef enum
{
  IWDG_Time_125us = 0,
  IWDG_Time_250us = 1,
  IWDG_Time_500us = 2,
  IWDG_Time_1ms = 3,
  IWDG_Time_2ms = 4,
  IWDG_Time_4ms = 5,
  IWDG_Time_8ms = 6
}
IWDG_Time_e;
```

Inicjacja:

```cpp
if(RCC->CSR & RCC_CSR_IWDGRSTF)
{
  RCC->CSR |= RCC_CSR_RMVF;
  delay_ms(200);
  GPIOA->BSRR |= GPIO_BSRR_BS5;
  delay_ms(200);
  GPIOA->BSRR |= GPIO_BSRR_BR5;
}

IWDG->KR = IWDG_START;
IWDG->KR = IWDG_WRITE_ACCESS;
IWDG->PR = IWDG_Time_4ms;
IWDG->RLR = 500;
while(IWDG->SR);
IWDG->KR = IWDG_REFRESH; // Konfiguracja IWDG
```

Reset:

```cpp
IWDG->KR = IWDG_REFRESH;
```

Sprawdzenie:

```cpp
if(RCC->CSR & RCC_CSR_IWDGRSTF)
{
  RCC->CSR |= RCC_CSR_RMVF;
  // ...
}
```

# UART

Konfiguracja GPIO

```cpp
RCC->IOPENR |= RCC_IOPSMENR_GPIOASMEN;
GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
GPIOA->MODER |= (2 << GPIO_MODER_MODE9_Pos) | (2 << GPIO_MODER_MODE10_Pos);
GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk);
GPIOA->AFR[1] |= (1 << GPIO_AFRH_AFSEL9_Pos) | (1 << GPIO_AFRH_AFSEL10_Pos);
```

Konfiguracja UART

```cpp
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
```

Wysyłanie konejnych znaków

```cpp
uint8_t var = '0';
while(1) {
  USART1->TDR = var;
  var++;
  delay_ms(200);
}
```

Odbieranie i odsyłanie zakodowanych znaków koden cezara

```cpp
void USART1_IRQHandler(void)
{
  if(USART1->ISR & USART_ISR_RXNE_RXFNE)
  {
    uint8_t var = (uint8_t)(USART1->RDR);

    if(var >= 'a' && var <= 'z')
    {
      var += 3;
      if(var > 'z')
        var -= ('z' - 'a' + 1);
      USART1->TDR = var;
    }
  }
}
```

# PWM

Sprawdzenie diody RGB

```cpp
RCC->IOPENR |= RCC_IOPSMENR_GPIOASMEN;
GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
GPIOA->MODER |= (1 << GPIO_MODER_MODE8_Pos) | (1 << GPIO_MODER_MODE9_Pos) | (1 << GPIO_MODER_MODE10_Pos);

GPIOA->ODR |= GPIO_ODR_OD8; // Red
GPIOA->ODR &= ~GPIO_ODR_OD8;

GPIOA->ODR |= GPIO_ODR_OD9; // Green
GPIOA->ODR &= ~GPIO_ODR_OD9;

GPIOA->ODR |= GPIO_ODR_OD10; //Blue
GPIOA->ODR &= ~GPIO_ODR_OD10;
```

Konfiguracja pinów dla timera TIM1 w trybie PWM

```cpp
RCC->IOPENR |= RCC_IOPSMENR_GPIOASMEN;
GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk | GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
GPIOA->MODER |= (2 << GPIO_MODER_MODE8_Pos) | (2 << GPIO_MODER_MODE9_Pos) | (2 << GPIO_MODER_MODE10_Pos);

GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL8_Msk | GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk);
GPIOA->AFR[1] |= (2 << GPIO_AFRH_AFSEL8_Pos) |(2 << GPIO_AFRH_AFSEL9_Pos) | (2 << GPIO_AFRH_AFSEL10_Pos);
```

Konfiguracja timera TIM1 w trybie PWM

```cpp
RCC->APBENR2 |= RCC_APBENR2_TIM1EN;

TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E;
TIM1->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
TIM1->CCMR1 |= TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
TIM1->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;

TIM1->PSC = 1599;
TIM1->ARR = 100;

TIM1->CR1 = TIM_CR1_ARPE;

TIM1->DIER &= ~TIM_DIER_UIE;
TIM1->BDTR |= TIM_BDTR_MOE;
TIM1->EGR |= TIM_EGR_UG;
TIM1->CR1 |= TIM_CR1_CEN;

// TIM1->CCR1 = [0..100];
```

Aplikacja sterująca diodą RGB:

```cpp
int32_t setPWM(uint8_t *dir, int32_t *val, int32_t inc)
{
  if(!*dir) {
    *val += inc;
    if(*val >= AUTO_RELOAD) { *val = AUTO_RELOAD; *dir ^= 1; }
  }
  else {
    *val -= inc;
    if(*val <= 0) { *val = 0; *dir ^= 1; }
  }
  return *val;
}
```

```cpp
uint8_t dir[3] = { 0, 0, 0 };
int32_t val[3] = { 0, 0, 0 };

while(1)
{
  TIM1->CCR1 = (uint32_t)setPWM(&dir[0], &val[0], 2);
  TIM1->CCR2 = (uint32_t)setPWM(&dir[1], &val[1], 3);
  TIM1->CCR3 = (uint32_t)setPWM(&dir[2], &val[2], 5);
  delay_ms(1);
}
```
