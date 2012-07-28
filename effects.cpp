#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "WS2801.h"

#include <iostream>
#include <cstring>
#include <cstdlib>

// Following effects adapted from Adafruit WS2801 library

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(WS2801 strip, uint16_t WheelPos)
{
  byte r=0, g=0, b=0;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}

void rainbow(WS2801 strip, time_t t, unsigned wait) 
{
  unsigned i;
  unsigned cycles = 384;
  unsigned period = wait * cycles;
  unsigned ticks = t % period;
  unsigned j = rint((double)(cycles*ticks) / (double)(period));
   
  std::cout << j << "/" << period << " t=" << t << " wait=" << wait << " " << ticks << "\n";
  for (i=0; i < strip.get_length(); i++) {
    strip.setPixelColor(i, Wheel(strip, (i + j) % 384));
  }  
  strip.refresh();   // write all the pixels out
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(WS2801 strip, time_t t, unsigned wait) 
{
  int i;
  int cycles = 384*5;
  int period = wait * cycles;
  int j = cycles*floor((t % period) * (1.0/period));
  
  for (i=0; i < strip.get_length(); i++) {
    strip.setPixelColor(i, Wheel(strip, ((i * 384 / strip.get_length()) + j) % 384) );
  }  
  strip.refresh();   // write all the pixels out
}

// fill the dots one after the other with said color
void colorWipe(WS2801 strip, time_t t, uint32_t c, unsigned wait) 
{
  int i;
  int cycles = strip.get_length();
  int period = wait * cycles;
  int j = cycles*floor((t % period) * (1.0/period));
  
  for (i=0; i < j; i++) {
    strip.setPixelColor(i, c);
  }
  strip.refresh();
}

// Chase a dot down the strip
void colorChase(WS2801 strip, time_t t, uint32_t c, unsigned wait) 
{
  int i;
  int cycles = strip.get_length();
  int period = wait * cycles;
  int j = cycles*floor((t % period) * (1.0/period));
  
  for (i=0; i < strip.get_length(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  } 
  
  strip.setPixelColor(j, c);
  strip.refresh();
}
