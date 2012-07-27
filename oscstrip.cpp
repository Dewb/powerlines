#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "WS2801.h"
#include "effects.h"

#include <iostream>
#include <cstring>
#include <cstdlib>

#include "ip/UdpSocket.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"

#include <boost/thread/thread.hpp>


WS2801 strip = WS2801("/dev/spidev2.0", 192);


void setStripColor(uint8_t r, uint8_t g, uint8_t b) {
    for (int i=0; i < strip.get_length(); i++) {
        strip.setPixelColor(i, r, g, b);
    }
    strip.refresh();
}

time_t getMillisecondClock()
{
  timespec tp;
  clock_gettime(CLOCK_MONOTONIC, &tp);
  return tp.tv_sec * 1000 + tp.tv_nsec / 1000;
}

class StripOscListener : public osc::OscPacketListener 
{
public:
    StripOscListener()
    : m_lastEvent(0)
    {
    }

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
               update = true;
            }
        }
        catch(...)
        {
        }
        
        if (update)
        {
          m_lastEvent = getMillisecondClock();
        }
    }  
    
    time_t m_lastEvent;

    volatile uint8_t m_r;
    volatile uint8_t m_g;
    volatile uint8_t m_b;

    uint8_t m_lastR;
    uint8_t m_lastG;
    uint8_t m_lastB;

public:

    void do_work()
    {  
        time_t now = getMillisecondClock();
        
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
            else if (now - m_lastEvent > 1000*5)
            {
                rainbow(strip, now, 100);
            }
        }
    }   
};


int main(int argc, char* argv[]) 
{
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

