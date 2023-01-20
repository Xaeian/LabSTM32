#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32g0xx.h"
#include "hc595.h"

#define delay_ms(ms) for(int _i = 1592 * ms; _i; _i--);

int main(void)
{
  HC595_Begin();
  HC595_Dec(12345);
  HC595_Send();

  while(1) {

  }
}
