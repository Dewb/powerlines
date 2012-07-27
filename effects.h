#include <unistd.h>
#include "WS2801.h"

uint32_t Wheel(WS2801 strip, uint16_t WheelPos);
void rainbow(WS2801 strip, int t, uint8_t wait);
void rainbowCycle(WS2801 strip, int t, uint8_t wait);
void colorWipe(WS2801 strip, int t, uint32_t c, uint8_t wait);
void colorChase(WS2801 strip, int t, uint32_t c, uint8_t wait);
