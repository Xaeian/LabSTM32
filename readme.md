## ⚓ Content

- [IDE, Dioda, Przycisk](#ide-dioda-przycisk-)
- [Liczniki](#liczniki-)
- [Biblioteka GPIO](#biblioteka-gpio-)
- [Biblioteka DELAY](#biblioteka-delay-)

Podczas tego kursu będziemy pracowali z procesorem **STM32G071RB** na deb-board'zie **nucleo**. Pracując takimy płytkami dobrze jest mieć pod ręką rysunek z oznaczonymi wyprowadzeniamu. Nie ma za  co ;)

![](images/nucleo.png)

# IDE, Dioda, Przycisk [➥](#-content)

Moim zdaniem w nauce programowania, _potem w sumie też_, ważne jest żeby osiągać jakieś efekty szybko. Praca z systemami wbudowanymi posiada niekończące się zagadanienia poboczne, jak konfiguracja środowiska, hardware itd. Zatem pominiemy to wszystko, ściągniemy środowisko [**STM32CubeIDE**](https://www.st.com/en/development-tools/stm32cubeide.html), dostarczane **ST**, klonujemy repozutorium **nucleo** i dodajemy [template](./template) jako projekt:

Project Exploler `⟶` Right click `⟶` **Import** `⟶` Existing Project into Workspace

```cpp
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

W nowszych procesorach, które dodatkowo są zoptymalizowane pod kontem aplikacji energooszczędnych wszystkie peryferia trzeba włączyć. Tak samo jest tutaj. Na początku programu wpisujemy 1 do rejestru [`RCC->IOPENR`](./docs/datasheet.pdf#page=147) na pozycji 0. Tą operacją doprowadzamy sygnał zegarowy do peryferium **GPIOA**.

W kolejnych liniach ustawiamy wyprowadzenie **PA5** na wyjście przez ustawienie wartości `01` do odpowiedniego pola rejestru [`GPIOA->MODER`](./docs/datasheet.pdf#page=205).

W pętli głównej na przemian ustawiamy i kasujemy bit z rejestru [`GPIOA->ODR`](./docs/datasheet.pdf#page=207). Te operacje na przemian ustawiają na wyjściu stan wysoki i niski, co skutkuje zapalaniem i gaszeniem diody. Oczywiście efekt ten jest zauważalny dzięki funkcji delay_ms.

```cpp
#define delay_ms(ms) for(int __i = 1592 * ms; __i; __i--)
```

W procesorach STM32 nie mamy wbudowanej funkcji `delay_ms` tak jak to działało w mikrokontrolerach **AVR**. Po prostu nie ma takeij potrzeby, ponieważ w procesorze Atmega328P mieliśmy 3 timery z czego w **Arduino** jeden był wykorzystywany do _nie wiem czego_. Procesory **STM32** z rodziny **G0** mamy do dyspozycji 12 timerów (nie licząc watchdoga) i jeden z nich możemy wykorzystać w celu oprogramowania funkcji `delay_ms`. Takie rozwiązanie będzie znacznie bardziej dokładne i precyzyjne. Nie mniej takie rozwiązanie będzie wystarczające w wielu przypadkach.

Na tym etapie warto usunąć linijki zawierające funkcje `delay_ms` oraz odpalić kod w debugerze 🐞.

Wracając do samego kodu. W przypadku niektórych rejestrów dobrą praktyką może okazać się zastąpienie przesunięć bitowych definicjami, które są zawartę w plikach nagówkowych dostarczomchy przez ST.

```cpp
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

```cpp
while(1)
{
  GPIOA->ODR ^= GPIO_ODR_OD5;
  delay_ms(200);
}
```

Najlepszym jednak sposobem jest na zmianę stanu wyjścia jest skorzystanie z rejestru [`GPIOA->BSRR`](./docs/datasheet.pdf#page=208). Pozwala on _atomowo_ wstawić do rejestru `GPIOA->ODR` 1 lub 0. Instrukcja `GPIOA->ODR |= GPIO_ODR_OD5` w praktyce wykonuje 3 operacje.

- Pobranie wartości z rejestru `GPIOA->ODR`
- Wykonanie sumy bitowej z wartością `GPIO_ODR_OD5`
- Wpisanie do rejestru `GPIOA->ODR` wyniku operacji

W bardziej skomplikowanych aplikacjach na skutek przerwania jakaś instrukcja mogłaby się wcisnąć w środek naszej operacji, co może wprowadzać komplikacje. Korzystając z rejestru `GPIOA->BSRR` procesor wykonuje jedną operacją, co czynie tą operację niepodzielną - _atomową_.

```cpp
while(1)
{
  GPIOA->BSRR |= GPIO_BSRR_BS5;
  delay_ms(200);
  GPIOA->BSRR |= GPIO_BSRR_BR5;
  delay_ms(200);
}
```

Migająca dioda już za nami - teraz pora na przycisk. Zanim jednak zabierzemy się za jego konfigurację warto przejrzeć się wszystkim [konfiguracją **GPIO**](./docs/datasheet.pdf#page=197)

Przycisk na płytce **nucleo** jest od razu podciągnięty do zasilania _(pull-up)_, więc wystarczy ustawić na jako wejście i odczytywać jego stan. Niech nasz program świeci diodą w przypadku wciśnięcia przycisku, a w przeciwnym razie niech dioda pozostanie zgaszona.

```cpp
RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
GPIOC->MODER &= ~GPIO_MODER_MODE13;

while(1)
{
  if(!(GPIOC->IDR & (1 << 13))) GPIOA->BSRR |= GPIO_BSRR_BS5;
  else GPIOA->BSRR |= GPIO_BSRR_BR5;
}
```

## Shields [➥](#-content)

Powyższe kody będą działać na _"gołej"_ płytce nucleo. Dalej będą wykorzystywane **shield**'y, czyli nakładki, które można połączyć na kanapkę z naszą płytką bazową

| Nakładka Input-Output     | Nakładka Measurement       |
| ------------------------- | -------------------------- |
| ![](images/shield-io.png) | ![](images/shield-msm.png) |

Oczywiście bez problemu można pracować bez tych płytek, ale jeśli preferujemy rozpocząć zabawę od _copy-paste_ to polecam. Warto wówczas na płytce nucleo dokonać dwóch drobnych modyfikacji...

## Świecąca linijka LED [➥](#-content)

Wyprowadzenia `PC0`-`PC15` są podłączone do kolejnych diod.
W programie jedynka z rejestru `GPIOC->ODR` jest przesuwana na lewo (`<<`).
Gdy dochodzi do diody `PC15` kierunek zmienia się na prawo (`>>`).
Całość sprawia wrażenie, że święcący punk przesuwa się na lewo i prawo.

```cpp
#define delay_ms(ms) for(int _i = 1592 * ms; _i; _i--);

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

## Przycisk i dioda [➥](#-content)

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

# Liczniki [➥](#-content)

Nasz procek procek jest owocem pracy dwóch firm. **ARM**, który zaprojektował rdzeń i sprzedał licencję firmie **ST Microelectronic**, które tworzy peryferia i produkuje układy. Zatem nie powinno nas dziwić, że nie wszystkie informacje znajdziemy w dokumencie Reference Manual. Takim układem jest licznik licznik **SysTick**, którego nazwa sugeruje, żeby wykorzystać go jako zegar systemowy. Pozostałe liczniki ogólnego przeznaczenia określane są w dokumentacji skrótem **TIMx**, gdzie **x** jest numerem timer'a.

## Licznik SysTick [➥](#-content)

SysTick jest bardzo prostym licznikiem 24-bitowym licznikiem, który jedyne co może robić to zliczać w górę takt sygnału zegarowego. Podczas konfiguracji `SysTick_Config` określamy, do jakiej wartości zliczanie ma się odbywać. Gdy to nastąpi, wartość rejestru zliczającego jest zerowana, a rdzeń przestaje wykonywać bieżące zadanie i przechodzi do funkcji `SysTick_Handler`. Taką zagrywkę ze strony mikrokontrolera nazywamy przerwaniem, a funkcję jego obsługą.

W przykładzie licznik ten jest skonfigurowany, aby przerwania występowało co `100ms`, a w obsłudze przetwania dioda podłączona do `PD5` zmienia stan na przeciwny.

```cpp
int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
  GPIOC->MODER &= ~(3 << (2 * 5));
  GPIOC->MODER |= (1 << (2 * 5));
  SysTick_Config(SystemCoreClock / 10); // T = 1s / 10 = 100ms
  while(1) __NOP();
}

void SysTick_Handler(void)
{
  GPIOC->ODR ^= (1 << 5);
}
```

Jeżeli z jakiegoś powodu rozdzielczość naszego licznika nie będzie wystarczająca

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

## Liczniki TIM [➥](#-content)

W procesorach STM32 liczników mamy naprawdę sporo. STM32G071RB posiada" TIM**1**, TIM**2**, TIM**3**, TIM**6**, TIM**7**, TIM**14**, TIM**15**, TIM**16**, TIM**17**. Różnią one się nieco między sobą. Np. **TIM1** jest najbardziej rozbudowany jeśli chodzi o peryferia i nalepiej sprawdza się podczas genrowania i zliczania sygnałów. **TIM2** ma zwiększoną rozdzielność, bo aż **32-bitów**. Pozostałe mają jedynie **16-bitów**. Jednak funkcjonalność jaką chcemy uzyskać _(czyli skok do przerwania z odpowiednim interwałem)_ uzyskamy wykorzystując nawet najbardziej ograniczone liczniki jakimi są **TIM6** i **TIM7**.

Konfiguracja tego licznika nie będzie tak prosta jak w przypadku SystTick'a. Jak to w STM32 trzeba włączyć peryterium poprzez podanie sygnału zegarowego. Najważniejsymy rejestrami w licznikach są `PSC`, w którym określamy wartość prescalera oraz `ARR`, po której osiągnięciu licznik się zeruję (auto-reload). Wartości tych rejestrów wpłyną na czas zerowania.

    T = (PSC - 1) * ARR / 16MHz (SystemCoreClock)

Trzeba wskazać, aby to zdarzenie poinformowało rdzeń o swoim wystąpieniu `event`'u. W tym celu trzeba ustawić bit `UIE` w rejestrze `DIER`. Aby całość zadziałała rdzeń musi spodziewać się sygnału `event`. Z poziemu programu należy wywołać funckję `NVIC_EnableIRQ` z odpowiednią flagą `TIM6_DAC_LPTIM1_IRQn`. Można także ustawić priorytet przerwania od `0` do `3` funkcją `NVIC_SetPriority`, gdzie przerwania z **niższymi** wartościami będą wykonywane w pierwszej kolejności. Na końcu wystarczy włączyć licznik i wszystko powinno śmigać.

```cpp
int main(void)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
  GPIOC->MODER &= ~(3 << (2 * 5));
  GPIOC->MODER |= (1 << (2 * 5)); // gpio init

  RCC->APBENR1 |= RCC_APBENR1_TIM6EN; // turn on clock signal on TIM6
  TIM6->PSC = 15999; // prescaler register
  TIM6->ARR = 10; // auto-reload register
  // T = (PSC - 1) * ARR / 16MHz (SystemCoreClock)
  TIM6->DIER |= TIM_DIER_UIE; // interupt enable (peryfery)
  NVIC_SetPriority(TIM6_DAC_LPTIM1_IRQn, 0); // interupt prioryty: 0 (arm-core)
  NVIC_EnableIRQ(TIM6_DAC_LPTIM1_IRQn); // interupt enable (arm-core)
  TIM6->CR1 |= TIM_CR1_CEN; // TIM6 enable

  while(1)
  {
    __NOP(); // do nothing
  }
}
```

To znaczy, program główny powinien być przerywany co `100ms` i powinien _"przeskakiwać"_ do funkcji `TIM6_DAC_LPTIM1_IRQHandler`, gdzie powinniśmy upewnić się, czy na 100% źródłem przerwania jest TIM6, zrobić co trzeba _(w naszym przypadku mrugnąć diodą)_ oraz wyzerować flagę licznika.

```cpp
void TIM6_DAC_LPTIM1_IRQHandler(void)
{
  if(TIM6->SR & TIM_SR_UIF) { // if TIM6 auto-reload event
    GPIOC->ODR ^= (1 << 5); // blink led
    TIM6->SR &= ~TIM_SR_UIF; // clear TIM6 auto-reload flag
  }
}
```

Odradzam szukanie nazw przerwań w dokumentacji i internecie. Najlepiej odwiedzić plik [startup](./template/Src/startup.c). Wówczas mamy pewność, że przerwanie nam zadziała, gdyż te nazwy potrafią się różnić nawet w obrębie jednej rodziny mikroprocków :(

Na koniec dodam, że nie możemy rejestrów `PSC` i `ARR` nie można traktować równorzędnie (choć ten przykład może to sugerować). Prescaler modyfikuje sygnał zegarowy hardware'owo, natomiast auto-reload jest jedynie wartością w rejestrze, do której licznik liczy. Znając tą różnicę, można się domyślić, że rejestr `ARR` możemy modyfikować _"w locie"_, wydłużając mignięcia diody z każdym przerwaniem.

```cpp
void TIM6_DAC_LPTIM1_IRQHandler(void)
{
  if(TIM6->SR & TIM_SR_UIF) {
    GPIOC->ODR ^= (1 << 5);
    TIM6->ARR++;
    TIM6->SR &= ~TIM_SR_UIF;
  }
}
```

# Biblioteka GPIO [➥](#-content)

W niemal każdym projekckie gdziej jest mikrokontroler, wykorzystuje się standardowe wyjścia/wejścia **GPIO**. Zatem miło jest mieć do tego bibliotekę, która na dłuższą metę usprawni naszą pracę.

Gdy tworzy się większe projekty, szybko można dojść do wniosku że pojedynczy plik `main.c` to zdecydowanie za mało. W tym celu dołaczamy dodatkowe inne pliki. W języku `c` zwykle robi się to w parach załączając pliki z rozszerzeniem `.c` _(od języka **c**)_ i `.h` _(od **header**)_. Taką parę możemy nazwać biblioteką. W pliku nagłówkowym `.h` dobrze umieścić poniższą składnie:

```cpp
#ifndef GPIO_H_
#define GPIO_H_
//...
#endif
```

Dzięki niej, gdy będziemy wykorzystywali bibliotekę wielokrotnie, zostanie ona dodana tylko jeden raz. Analizując... Gdy nie zdefiniowano zmiennej preprocesora `#ifndef GPIO_H_`, to zdefiniuj zmienną `#define GPIO_H_` i załącz zawartość `...`. W językach "wyższo-poziomowych" tego typu mechanizm jest zaszyty, więc nie musimu przejmować się wielokrotnym załączaniem bibliotek.

Nasza biblioteka załącz inne bibioteki:

```cpp
#include <stdbool.h>
#include <stdint.h>
#include "stm32g0xx.h"
```

Utartą konwencją jest, że biblioteki standardowe dostępne w języku **c** umieszczamy pomiędzy `<...>`
natomiast dodatkowe między `"..."`. Załączamy jedynie pliki `.h`.

Nasza biblioteka będzie stanowić interfejs pomiędzy hadware'm, a programistą aplikacyjnym _(który nie koniecznie musi wiedzieć, co w rejestrach piszczy)_. Zatem warto postarać się, aby nasz interfejs bym jak najbardziej przyjazny użytkownikowi _(user-frendly)_, ale nie można do końca zapominać o strukturze rejestrów, ponieważ może to znacznie utrudnić nam pisanie takiej biblioteki. Warto stosować zmienne wyliczeniowe `enum`, które mogą stanowić most pomiędzy sprzętem, a użytkownikiem.

```cpp
typedef enum {
  GPIO_Mode_Input = 0,
  GPIO_Mode_Output = 1,
  GPIO_Mode_Alternate = 2,
  GPIO_Mode_Analog = 3,
} GPIO_Mode_e;

typedef enum {
  GPIO_Pull_Floating = 0,
  GPIO_Pull_PullUp = 1,
  GPIO_Pull_PullDown = 2
} GPIO_Pull_e;
```

Po lewej stronie znajduje się nazwa, która będzie wykorzystywana z wyższego poziomu, a my podczas pisania biblioteki będziemy mieli dostęp do liczby, która kryje się pod tą nazwą. Warto pamiętać, że `enum`'y zajmują tyle miejsca co zmienna typy `int`.

Wszystkie, dostępne dla użytkownika, zmienne dobrze umieścić w jednej strukturze. Jeżeli będziemy wykorzystywali zmienne pomocnicze, warto rozpocząć je od znaku `_`. Będzie stanowiło, to podpowiedź, aby tych zmiennych nie ustawiać.

```cpp
typedef struct {
  GPIO_TypeDef *gpio_typedef;
  uint8_t pin_no;
  GPIO_Mode_e mode;
  GPIO_Pull_e pull;
  bool revers;
  uint8_t alternate;
} GPIO_t;
```

Funkcje, które możemy wykonać z przekazaniem struktury, umieszczamy w [pliku `.c`](./template/Src/gpio.c), który musi mieć dostęp do pliku nagłówkowego, więc musi rozpoczynać się od dołączenia własnego nagłówka `#include "gpio.h"`. W pliku `.h` umieszczamy jedynie nazwy funkcji:

```cpp
void GPIO_Init(GPIO_t *gpio);
void GPIO_Set(GPIO_t *gpio);
void GPIO_Rst(GPIO_t *gpio);
void GPIO_Tgl(GPIO_t *gpio);
bool GPIO_In(GPIO_t *gpio);
```

Zawartość tej biblioteki _(oczywiście z obsługą funkcji zawartą w pliku `.c`)_ stanowi pseudo-**klasę**. Powyższe funkcje będą wykonywane zawsze na rzecz stworzonego wcześniej _"**obiektu**"_, zatem na standardy programowania w języku `.c`, możemy śmiało je nazywać **metodami**.

## Używanie biblioteki GPIO [➥](#-content)

Oczywiste jest, że aby korzystać z naszej biblioteki musimy ją dołączyć. O ile będzie ona umieszczona w tym samym folderze, co plik ją wykorzystujący, wystarczy na początku dodać `#include "gpio.h"`.

Weźmy nasz program zapalający diodę w przypadku wciśnięcia przycisku:

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

...i przepiszmy go z wykorzystaniem naszej biblioteki **GPIO**.

```cpp
GPIO_t app_led = { .gpio_typedef = GPIOC, .pin_no = 8, .mode = GPIO_Mode_Output };
GPIO_t app_sw = { .gpio_typedef = GPIOA, .pin_no = 12, .pull = GPIO_Pull_PullUp, .revers = true };

int main(void)
{
  GPIO_Init(&app_led);
  GPIO_Init(&app_sw);

  while(1)
  {
    if(GPIO_In(&app_sw)) {
      GPIO_Set(&app_led);
    }
    else {
      GPIO_Rst(&app_led);
    }
  }
}
```

Taki kod, nawet bez komentarzy, jest znacznie prostrzy do ogarnięcia niż odnosząc się bezpośrednio do rejestrów. Ale dyskusja czy jest ono słuszne to już temat na inną opowieść ;)

Warto pamiętać, że dla struktury zadeklarowanej globalnie _(nie w funkcji)_ wszystkie niezadeklarowane zmienne będą przyjmowały wartość `0`, więc wartość pola `mode` zmiennej `GPIO_t app_sw` jest ustawiona na `GPIO_Mode_Input`.

# Biblioteka DELAY [➥](#-content)

Pisząc proste programy jednowątkowe wygodnie wyposażyć się w funkcję `delay(time)`, która spowoduje odczekanie czasu `time`, ponieważ pracując na **STM**'ach nie dostajemy gotowych funkcji jak w przypadku mikrokontrolerów **AVR**. Aby uzyskać odpowiednią precyzję, wykorzystamy sprzętowy **TIM**'er. TIM'erów mamy do dyspozycji całkiem sporo, ale na początek damy do wyboru liczniki `6` i `7`. W zależności od wybranej opcji zostaną wykorzystane inne struktury, definicje i funkcje:

```cpp
#if(DELAY_TIM == 6)
  #define _DELAY_TIM TIM6
  #define _DELAY_HANDLER TIM6_DAC_LPTIM1_IRQHandler
  #define _DELAY_N TIM6_DAC_LPTIM1_IRQn
#elif(DELAY_TIM == 7)
  #define _DELAY_TIM TIM7
  #define _DELAY_HANDLER TIM7_LPTIM2_IRQHandler
  #define _DELAY_N TIM7_LPTIM2_IRQn
#endif
```

Dobrą praktyką jest ustawienie domyślnego timera, tak na wypadek, gdyby programista nie miał ochoty się tym zajmować. Wówczas, gdy tego nie zrobi zmienna `DELAY_TIM` zostanie ustawiona na `6`.

```cpp
#ifndef DELAY_TIM
  #define DELAY_TIM 6
#endif
```

Przydałoby się jednak obsłużyć sytuacje, gdy nie chcemy _"marnować"_ sprzętowego licznika, a poznana już niedokładna metoda czekania zadanego czasu jest wystarczająca dla pisanej aplikacji.

```cpp
#ifndef DELAY_HARDWARE
  #define DELAY_HARDWARE 1
#endif

#if(DELAY_HARDWARE)
  // ...
  // hardware delay defines
  // ...
  void delay_init(void);
  void delay_ms(uint16_t ms);
  void delay_us(uint16_t us);
#else
  #define delay_ms(ms) for(int _i = 1592 * ms; _i; _i--)
#endif

#endif
```

Trzeba jednak pamiętać, że TIM'ery wymagają inicjacji, stąd funkcja `delay_init`. Obsługa funkcji jak wiadomo w [pliku `delay.c`](./template/Src/delay.c)
