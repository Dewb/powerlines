#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "WS2801.h"

#include <iostream>
#include <cstring>
#include <math.h>
#include <cstdlib>

#include "osc/OscReceivedElements.h"

#include "ip/UdpSocket.h"
#include "osc/OscPacketListener.h"

#include <boost/thread/thread.hpp>

WS2801 strip = WS2801("/dev/spidev2.0", 192);


void setStripColor(uint8_t r, uint8_t g, uint8_t b) {
    for (int i=0; i < strip.get_length(); i++) {
        strip.setPixelColor(i, r, g, b);
    }
    strip.refresh();
}


class StripOscListener : public osc::OscPacketListener 
{
protected:

    void ProcessMessage( const osc::ReceivedMessage& m, 
                         const IpEndpointName& remoteEndpoint )
    {
        bool update = false;
        try {
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();

            std::cout << "MSG: [" << m.AddressPattern() << "]\n";

            if(strcmp(m.AddressPattern(), "/RGB/r") == 0) {
                m_r = (uint8_t)floor(arg->AsFloat()*255); update = true;
            } else if (strcmp(m.AddressPattern(), "/RGB/g") == 0) {
                m_g = (uint8_t)floor(arg->AsFloat()*255); update = true;
            } else if (strcmp(m.AddressPattern(), "/RGB/b") == 0) {
                m_b = (uint8_t)floor(arg->AsFloat()*255); update = true;
            }

            if(strncmp(m.AddressPattern(), "/user/", 6) == 0) {
               m_r = (uint8_t)floor(arg->AsFloat()*128) % 128;
               m_g = 128 - m_r;
               ++arg;
               ++arg;
               m_b = (uint8_t)floor(arg->AsFloat()*17) % 128;
            }
        }
        catch(...)
        {
        }

        //if (update) {
        //    setStripColor(m_r, m_g, m_b);
        //}
    }  

    volatile uint8_t m_r;
    volatile uint8_t m_g;
    volatile uint8_t m_b;

    uint8_t m_lastR;
    uint8_t m_lastG;
    uint8_t m_lastB;

public:

    void do_work()
    {  
        bool send = false;
        while(1)
        {
            if (m_r != m_lastR) { send = true; m_lastR = m_r; }
            if (m_g != m_lastG) { send = true; m_lastG = m_g; }
            if (m_b != m_lastB) { send = true; m_lastB = m_b; }
            if (send)
            {
                setStripColor(m_lastR, m_lastG, m_lastB);
                send = false;
            }
        }
    }   
};

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos)
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

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < strip.get_length(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }  
    strip.refresh();   // write all the pixels out
    usleep(wait * 10000);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.get_length(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.get_length() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.get_length()) + j) % 384) );
    }  
    strip.refresh();   // write all the pixels out
    usleep(wait * 1000);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.get_length(); i++) {
      strip.setPixelColor(i, c);
      strip.refresh();
      usleep(wait * 10000);
  }
}

// Chase a dot down the strip
// good for testing purposes
void colorChase(uint32_t c, uint8_t wait) {
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

int main(int argc, char* argv[]) 
{
    colorChase(strip.Color(255,0,0), 10); 
    setStripColor(32, 32, 32);

    StripOscListener listener;
    int port = 7000;

    UdpListeningReceiveSocket s(
        IpEndpointName( IpEndpointName::ANY_ADDRESS, port),
        &listener );

    std::cout << "listening for input on port " << port << "...\n";
    std::cout << "press ctrl-c to end\n";

    boost::thread spiThread(boost::bind(&StripOscListener::do_work, &listener));

    s.RunUntilSigInt();
    std::cout << "finishing.\n";
}

