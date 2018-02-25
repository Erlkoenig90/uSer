CXXFLAGS=-std=c++17 -fsanitize=undefined -Wall -Wextra -Wconversion -pedantic -I../uSer -g

EX_SOURCES	= $(wildcard *.cpp)

EX_BINARIES	= $(foreach file,$(EX_SOURCES),build/$(basename $(file)))

all : $(EX_BINARIES)

$(EX_BINARIES) : build/% : %.cpp ../uSer/uSer.hh
	$(CXX) $(CXXFLAGS) -o $@ $<

.PHONY : clean

clean :
	rm -f $(EX_BINARIES)
