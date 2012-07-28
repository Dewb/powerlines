#include <unistd.h>
#include "WS2801.h"

uint32_t Wheel(WS2801& strip, uint16_t WheelPos);
void rainbow(WS2801& strip, time_t t, unsigned wait);
void rainbowCycle(WS2801& strip, time_t t, unsigned wait);
void colorWipe(WS2801& strip, time_t t, uint32_t c, unsigned wait);
void colorChase(WS2801& strip, time_t t, uint32_t c, unsigned wait);
void blocking_rainbowCycle(WS2801& strip, unsigned wait);
void blocking_colorWipe(WS2801& strip, uint32_t c, unsigned wait);
void blocking_colorChase(WS2801& strip, uint32_t c, unsigned wait);

