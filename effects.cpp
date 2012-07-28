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

uint32_t Wheel(WS2801& strip, uint16_t WheelPos)
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

void rainbow(WS2801& strip, time_t t, unsigned wait) 
{
  unsigned i;
  unsigned cycles = 384;
  unsigned period = wait * cycles;
  unsigned ticks = t % period;
  unsigned j = rint((double)(cycles*ticks) / (double)(period));
   
  //std::cout << j << "/" << period << " t=" << t << " wait=" << wait << " " << ticks << "\n";
  for (i=0; i < strip.get_length(); i++) {
    strip.setPixelColor(i, Wheel(strip, (i + j) % 384));
  }  
  strip.refresh();   // write all the pixels out
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(WS2801& strip, time_t t, unsigned wait) 
{
  unsigned i;
  unsigned cycles = 384 * 5;
  unsigned period = wait * cycles;
  unsigned ticks = t % period;
  unsigned j = rint((double)(cycles*ticks) / (double)(period));
  
  for (i=0; i < strip.get_length(); i++) {
    strip.setPixelColor(i, Wheel(strip, ((i * 384 / strip.get_length()) + j) % 384) );
  }  
  strip.refresh();   // write all the pixels out
}

// fill the dots one after the other with said color
void colorWipe(WS2801& strip, time_t t, uint32_t c, unsigned wait) 
{
  unsigned i;
  unsigned cycles = strip.get_length();
  unsigned period = wait * cycles;
  unsigned ticks = t % period;
  unsigned j = rint((double)(cycles*ticks) / (double)(period));
  
  for (i=0; i < j; i++) {
    strip.setPixelColor(i, c);
  }
  strip.refresh();
}

// Chase a dot down the strip
void colorChase(WS2801& strip, time_t t, uint32_t c, unsigned wait) 
{
  unsigned i;
  unsigned cycles = strip.get_length();
  unsigned period = wait * cycles;
  unsigned ticks = t % period;
  unsigned j = rint((double)(cycles*ticks) / (double)(period));
  
  for (i=0; i < strip.get_length(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  } 
  
  strip.setPixelColor(j, c);
  strip.refresh();
}

void blocking_rainbowCycle(WS2801& strip, unsigned wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.get_length(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.get_length() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( strip, ((i * 384 / strip.get_length()) + j) % 384) );
    }  
    strip.refresh();   // write all the pixels out
    usleep(wait * 1000);
  }
}

void blocking_colorWipe(WS2801& strip, uint32_t c, unsigned wait) {
  int i;
  
  for (i=0; i < strip.get_length(); i++) {
      strip.setPixelColor(i, c);
      strip.refresh();
      usleep(wait * 10000);
  }
}

void blocking_colorChase(WS2801& strip, uint32_t c, unsigned wait) {
  int i;
  
  for (i=0; i < strip.get_length(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  } 
  
  for (i=0; i < strip.get_length(); i++) {
      strip.setPixelColor(i, c);
      if (i == 0) { 
        strip.setPixelColor(strip.get_length()-1, 0);
      } else {
        strip.setPixelColor(i-1, 0);
      }
      strip.refresh();
      usleep(wait * 100);
  }
}

