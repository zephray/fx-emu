
TARGET = fx-emu

HOST =

CFILES = $(wildcard ./*.c)
CPPFILES = $(wildcard ./*.cpp)
OBJFILES = $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

CCFLAGS = `sdl-config --cflags` -g -Wall -O2 -fno-strict-aliasing
CXXFLAGS = $(CCFLAGS) -std=c++11
CFLAGS = $(CCFLAGS) -std=gnu99
LDFLAGS = `sdl-config --libs` -lstdc++ -lm

$(TARGET): $(OBJFILES)
	$(HOST)g++ $(OBJFILES) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(HOST)gcc $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(HOST)gcc $(CXXFLAGS) -c $< -o $@

clean:
	-rm -f $(TARGET) *.o
