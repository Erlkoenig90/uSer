ifeq ($(OS),Windows_NT)
	detected_OS := Windows
else
	detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
endif

EEXT=
ifeq ($(detected_OS),Windows)
	EEXT=.exe
endif

CXXFLAGS=-std=c++17 -Wall -Wextra -Wconversion -pedantic -I../uSer -fsanitize=undefined -g

EX_SOURCES	= $(wildcard *.cpp)

EX_BINARIES	= $(foreach file,$(EX_SOURCES),build/$(basename $(file))$(EEXT))

EX_DISAS = $(foreach file,$(EX_SOURCES),build/$(basename $(file)).S)

all : $(EX_DISAS)

$(EX_DISAS) : build/%.S : build/%$(EEXT)
	objdump -d -C $< > $@

$(EX_BINARIES) : build/%$(EEXT) : %.cpp ../uSer/uSer.hh
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f $(EX_BINARIES) $(EX_DISAS)

