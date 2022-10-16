#include "hc595.h"

//---------------------------------------------------------------------------------------------------------------------

void SPI_Software_Init(SPI_Software_t *spi)
{
  spi->cs->mode = GPIO_Mode_Output;
  spi->sck->mode = GPIO_Mode_Output;
  spi->miso->mode = GPIO_Mode_Input;
  spi->mosi->mode = GPIO_Mode_Output;
  GPIO_Init(spi->cs);
  GPIO_Init(spi->sck);
  GPIO_Init(spi->miso);
  GPIO_Init(spi->mosi);
  #if(SPI_SOFTWARE_CPOL)
    GPIO_Set(spi->sck);
  #else
    GPIO_Rst(spi->sck);
  #endif
  GPIO_Rst(spi->cs);
}

static void SPI_Software_Write(GPIO_t *mosi, uint8_t *byte_tx)
{
  #if(SPI_SOFTWARE_LBS)
    if(*byte_tx & 1) GPIO_Set(mosi);
    else GPIO_Rst(mosi);
    *byte_tx >>= 1;
  #else
    if(*byte_tx & (1 << 7)) GPIO_Set(mosi);
    else GPIO_Rst(mosi);
    *byte_tx <<= 1;
  #endif
}

static void SPI_Software_Read(GPIO_t *miso, uint8_t *byte_rx)
{
  #if(SPI_SOFTWARE_LBS)
    *byte_rx >>= 1;
    if(GPIO_In(miso)) *byte_rx += (1 << 7);
  #else
    *byte_rx <<= 1;
    if(GPIO_In(miso)) *byte_rx += 1;
  #endif
}

void SPI_Software_Run(SPI_Software_t *spi, uint8_t *rx_ary, uint8_t *tx_ary, uint16_t n)
{
  #if(SPI_SOFTWARE_CPOL)
    GPIO_Set(spi->sck);
  #else
    GPIO_Rst(spi->sck);
  #endif
  GPIO_Set(spi->cs);
  for(uint32_t i = 0; i < spi->delay; i++) {}
  uint8_t byte_tx, byte_rx;
  for(uint16_t i = 0; i < n; i++) {
    byte_tx = tx_ary[i];
    byte_rx = 0;
    for(uint8_t j = 0; j < 8; j++) {
      GPIO_Tgl(spi->sck); __NOP();
      #if(!SPI_SOFTWARE_CPHA)
        SPI_Software_Write(spi->mosi, &byte_tx);
        SPI_Software_Read(spi->mosi, &byte_rx);
      #endif
      GPIO_Tgl(spi->sck); __NOP();
      #if(SPI_SOFTWARE_CPHA)
        SPI_Software_Write(spi->mosi, &byte_tx);
        SPI_Software_Read(spi->mosi, &byte_rx);
      #endif
    }
    rx_ary[i] = byte_rx;
  }
  #if(SPI_SOFTWARE_CPOL)
    GPIO_Rst(spi->sck);
  #else
    GPIO_Set(spi->sck);
  #endif
  GPIO_Rst(spi->cs);
}

//---------------------------------------------------------------------------------------------------------------------

GPIO_t hc595_cs = { .gpio_typedef = GPIOB, .pin_no = 0, .revers = true };

GPIO_t hc595_sck = { .gpio_typedef = GPIOA, .pin_no = 5 };
GPIO_t hc595_miso = { .gpio_typedef = GPIOA, .pin_no = 6 };
GPIO_t hc595_mosi = { .gpio_typedef = GPIOA, .pin_no = 7 };

SPI_Software_t hc595_spi = {
  .cs = &hc595_cs,
  .sck = &hc595_sck,
  .miso = &hc595_miso,
  .mosi = &hc595_mosi,
  .delay = 200
};

HC595_t hc595 = { .spi = &hc595_spi };

void HC595_Begin(void)
{
  SPI_Software_Init(hc595.spi);
}

void HC595_Send(void)
{
  SPI_Software_Run(hc595.spi, hc595.rx_ary, hc595.tx_ary, HC595_BUFFER_SIZE);
}

//--------------------------------------------------------------------------------------------------------------------- <--- APP


void HC595_Sign(uint8_t sign, uint8_t pos)
{

}

