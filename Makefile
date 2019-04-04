TARGET = sv
LIBS = -lm -lpthread
CC = g++
CFLAGS = -g -Wall -std=c++14 -fno-strict-aliasing -g0 -fthreadsafe-statics -fno-rtti -fomit-frame-pointer -std=c11 -ffast-math

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
