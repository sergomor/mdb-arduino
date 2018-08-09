#include "utils.h"

//namespace utils {

uint16_t freeRam()
{
  extern uint16_t __heap_start, *__brkval;
  uint16_t v;
  return (int) &v - (__brkval == 0 ? (uint16_t) &__heap_start : (uint16_t) __brkval);
}

//};