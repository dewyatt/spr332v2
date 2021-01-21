CXX=g++

CXXFLAGS=-g -Wall -std=c++17 `pkg-config --cflags libpcsclite`
LDFLAGS=`pkg-config --libs libpcsclite`

TARGETS=spr332v2.o spr-buzzer spr-leds spr-nvram

all: $(TARGETS)

spr332v2.o: spr332v2.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c spr332v2.cpp

spr-buzzer: buzzer.cpp spr332v2.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) spr332v2.o -o spr-buzzer buzzer.cpp

spr-leds: leds.cpp spr332v2.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) spr332v2.o -o spr-leds leds.cpp

spr-nvram: nvram.cpp spr332v2.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) spr332v2.o -o spr-nvram nvram.cpp

clean:
	$(RM) $(TARGETS)

