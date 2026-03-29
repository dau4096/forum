CC = g++
CFLAGS = -std=c++23 -Wall -Wextra -static-libstdc++ -static-libgcc
LDFLAGS = -I/usr/include -I/usr/local/include

LIBS = -lm -ldl -pthread -static-libstdc++ -static-libgcc

SOURCES = main.cpp src/font.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: prgm

debug: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -g -DBUFFER_2x -o prgm

prgm: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -O2 -ffast-math -o prgm

%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) prgm


