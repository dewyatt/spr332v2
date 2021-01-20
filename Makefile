CXX=g++

CXXFLAGS=-g -Wall -std=c++17 `pkg-config --cflags libpcsclite`
LDFLAGS=`pkg-config --libs libpcsclite`

TARGETS=spr332v2.o buzzer leds nvram

all: $(TARGETS)

spr332v2.o: spr332v2.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c spr332v2.cpp

buzzer: buzzer.cpp spr332v2.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) spr332v2.o -o buzzer buzzer.cpp

leds: leds.cpp spr332v2.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) spr332v2.o -o leds leds.cpp

nvram: nvram.cpp spr332v2.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) spr332v2.o -o nvram nvram.cpp

clean:
	$(RM) $(TARGETS)

