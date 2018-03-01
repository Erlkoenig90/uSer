ifeq ($(OS),Windows_NT)
	detected_OS := Windows
else
	detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
endif

EEXT=
ifeq ($(detected_OS),Windows)
	EEXT=.exe
endif

CC=gcc
CXX=g++

CFLAGS=-std=c99 -Wall -Wextra -Wconversion -pedantic -I../../uSer -fsanitize=undefined -g
CXXFLAGS=-std=c++17 -Wall -Wextra -Wconversion -pedantic -I../../uSer -fsanitize=undefined -g
LDFLAGS=-fsanitize=undefined -g

build/example-c$(EEXT) : build/main.o build/serialize.o
	$(CXX) -o $@ $^ $(LDFLAGS)

build/main.o : main.c packet.h ../../uSer/uSer.hh
	$(CC) -c -o $@ $< $(CFLAGS)

build/serialize.o : serialize.cpp packet.h ../../uSer/uSer.hh
	$(CXX) -c -o $@ $< $(CXXFLAGS)

clean :
	rm -f build/example-c$(EEXT) build/main.o build/serialize.o