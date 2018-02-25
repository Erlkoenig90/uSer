CXXFLAGS=-std=c++17 -Wall -Wextra -Wconversion -pedantic -I../uSer -fsanitize=undefined -g

EX_SOURCES	= $(wildcard *.cpp)

EX_BINARIES	= $(foreach file,$(EX_SOURCES),build/$(basename $(file)))

EX_DISAS = $(foreach file,$(EX_SOURCES),build/$(basename $(file)).S)

all : $(EX_DISAS)

$(EX_DISAS) : build/%.S : build/%
	objdump -d -C $< > $@

$(EX_BINARIES) : build/% : %.cpp ../uSer/uSer.hh
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f $(EX_BINARIES) $(EX_DISAS)

