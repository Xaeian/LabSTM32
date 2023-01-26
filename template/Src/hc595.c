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

const uint8_t HC595_SIGN_ARRAY[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
const uint8_t HC595_POS_ARRAY[] = { 5, 4, 3, 2, 1};

static uint8_t HC595_Convert(uint8_t input)
{
  if(input >= '0' && input <= '9') input -= '0';
  else if(input >= 'A' && input <= 'F') input -= 'A' - 10;
  else if(input >= 'a' && input <= 'f') input -= 'a' - 10;

  switch(input)
  {
    case '-': return HC595_MINUS;
    case 'o': return 0x5C;
    case 'r': return 0x50;
    case ' ': return HC595_SPACE;
    default: return HC595_SIGN_ARRAY[input];
  }
}

void HC595_Sign(uint8_t sign, uint8_t pos)
{
  hc595.tx_ary[HC595_POS_ARRAY[pos]] = HC595_Convert(sign);
}

void HC595_Signs(uint8_t s4, uint8_t s3, uint8_t s2, uint8_t s1, uint8_t s0)
{
  hc595.tx_ary[1] = HC595_Convert(s4);
  hc595.tx_ary[2] = HC595_Convert(s3);
  hc595.tx_ary[3] = HC595_Convert(s2);
  hc595.tx_ary[4] = HC595_Convert(s1);
  hc595.tx_ary[5] = HC595_Convert(s0);
}

void HC595_Dot(uint8_t pos)
{
  for(uint8_t i = 1; i < 6; i++) hc595.tx_ary[i] &= ~(1 << 7);
  hc595.tx_ary[HC595_POS_ARRAY[pos]] |= (1 << 7);
}

static void HC595_Int(int32_t nbr, uint8_t base)
{
  char *str = itoa_int(nbr, base, true, 1, 5);
  if(strlen(str) != 5) str = "Error";
  HC595_Signs(str[0], str[1], str[2], str[3], str[4]);
}

void HC595_Dec(int32_t nbr)
{
  HC595_Int(nbr, 10);
}

void HC595_Hex(int32_t nbr)
{
  HC595_Int(nbr, 16);
}

void HC595_Float(float nbr, uint8_t precision)
{
  for(uint8_t i = 0; i < precision; i++) nbr *= 10;
  HC595_Int((int32_t)nbr, 10);
  if(precision) HC595_Dot(precision);
}

void HC595_FloatAutoPrecision(float nbr)
{
  uint8_t precision;
  if(nbr <= -10000) precision = 0;
  else if(nbr <= -1000) precision = 1;
  else if(nbr <= -10) precision = 2;
  else if(nbr < 0) precision = 3;
  else if(nbr < 10) precision = 4;
  else if(nbr < 100) precision = 3;
  else if(nbr < 1000) precision = 2;
  else if(nbr < 10000) precision = 1;
  else precision = 0;
  HC595_Float(nbr, precision);
}

void HC595_LedOne(uint8_t pos)
{
  hc595.tx_ary[0] = (1 << pos);
}

void HC595_LedSet(uint8_t pos)
{
  hc595.tx_ary[0] |= (1 << pos);
}

void HC595_LedRst(uint8_t pos)
{
  hc595.tx_ary[0] &= ~(1 << pos);
}

void HC595_LedTgl(uint8_t pos)
{
  hc595.tx_ary[0] &= ~(1 << pos);
}

void HC595_LedClear(uint8_t pos)
{
  hc595.tx_ary[0] = 0;
}

//---------------------------------------------------------------------------------------------------------------------


