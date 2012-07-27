# should be either OSC_HOST_BIG_ENDIAN or OSC_HOST_LITTLE_ENDIAN
# Apple Mac OS X: OSC_HOST_BIG_ENDIAN
# Win32: OSC_HOST_LITTLE_ENDIAN
# i386 GNU/Linux: OSC_HOST_LITTLE_ENDIAN
ENDIANESS=OSC_HOST_LITTLE_ENDIAN

CXX = g++
INCLUDES = -I./
COPTS  = -Wall -O3
CDEBUG = -Wall -g 
CXXFLAGS = $(COPTS) $(INCLUDES) -D$(ENDIANESS)

PREFIX = /usr/local
INSTALL = /usr/bin/install -c

SOURCES = oscstrip.cpp effects.cpp WS2801.cpp ./osc/OscTypes.cpp ./osc/OscReceivedElements.cpp ./osc/OscPrintReceivedElements.cpp ./ip/posix/NetworkingUtils.cpp ./ip/posix/UdpSocket.cpp 
OBJECTS = $(SOURCES:.cpp=.o)
LIBS = -lboost_thread

all: oscstrip	

oscstrip : $(OBJECTS)
	$(CXX) -o oscstrip $+ $(LIBS) 

clean:
	rm -rf $(OBJECTS) 


