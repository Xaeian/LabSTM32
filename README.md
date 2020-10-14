# Nucleo Base Project

Bazowy projekt dla procesorów `STM32` z rodziny `G0`. Zawiera on wszystkie pliki nagłówkowe

###Struktura projektu

+ Inc
  + CMSIS - pliki nagłówkowe od ARM
  + ST - pliki nagłówkowe od ST
+ **Scr** - katalog z naszymi plikami
  + `main.c` - plik główny
  + `syscalls.c`
  + `sysmem.c` (plik główny)
+ Startup
  - `startup_stm32g081rbtx.s` - plik inicjacyjny
+ `STM32G081RBTx_Flash.ld` - plik struktury pamięci

W projekcie zaimplementowane są pliki pliki dla procesora **STM32G081xx** jednak równie dobrze działają dla procesorów **STM32G071xx**. Mikrokontroler **STM32G070xx** nie posiada wszystkich peryferiów, więc niektórych definicji nie należy używać.

Projekt obsługuje wersję procesorów z **128kB** pamięci **FLASH** oraz **36kB** pamięci **RAM**. W przypadku innych konfiguracji konieczna jest podmiana pliku `STM32G081RBTx_Flash.ld`

Program startowy z częstotliowścią 400ms miga diodą podpiędą do pinu **PA5** na płytce **NUCLEO**.