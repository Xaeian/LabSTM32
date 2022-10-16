#ifndef HC595_H_
#define HC595_H_

#include <string.h>
#include <stdint.h>
#include "gpio.h"
#include "stm32g0xx.h"

//---------------------------------------------------------------------------------------------------------------------

#ifndef SPI_SOFTWARE_LBS
  #define SPI_SOFTWARE_LBS 0
#endif
#ifndef SPI_SOFTWARE_CPOL
  #define SPI_SOFTWARE_CPOL 0
#endif
#ifndef SPI_SOFTWARE_CPHA
  #define SPI_SOFTWARE_CPHA 1
#endif

typedef struct {
  GPIO_t *cs;
  GPIO_t *sck;
  GPIO_t *miso;
  GPIO_t *mosi;
  uint32_t delay;
} SPI_Software_t;

void SPI_Software_Init(SPI_Software_t *spi);
void SPI_Software_Run(SPI_Software_t *spi, uint8_t *rx_ary, uint8_t *tx_ary, uint16_t n);

//---------------------------------------------------------------------------------------------------------------------

#define HC595_BUFFER_SIZE 6

typedef struct {
  SPI_Software_t *spi;
  uint8_t tx_ary[HC595_BUFFER_SIZE];
  uint8_t rx_ary[HC595_BUFFER_SIZE];
} HC595_t;

void HC595_Begin(void);
void HC595_Send(void);
void HC595_Sign(uint8_t sign, uint8_t pos);
extern SPI_Software_t hc595_spi;

//---------------------------------------------------------------------------------------------------------------------
#endif
