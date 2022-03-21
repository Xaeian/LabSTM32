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
