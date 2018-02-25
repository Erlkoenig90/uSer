GXX = g++
CLANGXX = clang++

FLAGS1=-std=c++1z -Wall -Wextra -Werror -Wconversion -Wsign-promo -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=5 -fstrict-aliasing -I../uSer
FLAGS2=-fsanitize=undefined -g
FLAGS3=-O2 -flto
BUILDDIR=build/
GCC_BUILDS = $(BUILDDIR)test-gcc-quick $(BUILDDIR)test-gcc-ex $(BUILDDIR)test-gcc-ec $(BUILDDIR)test-gcc-rel
CLANG_BUILDS = $(BUILDDIR)test-clang-quick $(BUILDDIR)test-clang-ex $(BUILDDIR)test-clang-ec $(BUILDDIR)test-clang-rel

DEP=../uSer/uSer.hh

all : test-clang-all test-gcc-all

$(BUILDDIR)test-gcc-quick : test.cpp $(DEP)
	$(GXX) -DUSER_TEST_QUICK -o $@ $< $(FLAGS1)
	./$@

$(BUILDDIR)test-gcc-ex : test.cpp $(DEP)
	$(GXX) -DUSER_EXCEPTIONS -o $@ $< $(FLAGS1) $(FLAGS2)
	./$@

$(BUILDDIR)test-gcc-ec : test.cpp $(DEP)
	$(GXX) -o $@ $< $(FLAGS1) $(FLAGS2)
	./$@

$(BUILDDIR)test-gcc-rel : test.cpp $(DEP)
	$(GXX) -o $@ $< $(FLAGS1) $(FLAGS3)
	./$@

test-gcc-quick-run : $(BUILDDIR)test-gcc-quick
	$<

test-gcc-ex-run : $(BUILDDIR)test-gcc-ex
	$<

test-gcc-ec-run : $(BUILDDIR)test-gcc-ec
	$<

test-gcc-rel-run : $(BUILDDIR)test-gcc-rel
	$<

test-gcc-all : test-gcc-quick-run test-gcc-ex-run test-gcc-ec-run test-gcc-rel-run

$(BUILDDIR)test-clang-quick : test.cpp $(DEP)
	$(CLANGXX) -DUSER_TEST_QUICK -o $@ $< $(FLAGS1)
	./$@

$(BUILDDIR)test-clang-ex : test.cpp $(DEP)
	$(CLANGXX) -DUSER_EXCEPTIONS -o $@ $< $(FLAGS1) $(FLAGS2)
	./$@

$(BUILDDIR)test-clang-ec : test.cpp $(DEP)
	$(CLANGXX) -o $@ $< $(FLAGS1) $(FLAGS2)
	./$@

$(BUILDDIR)test-clang-rel : test.cpp $(DEP)
	$(CLANGXX) -o $@ $< $(FLAGS1) $(FLAGS3)
	./$@

test-clang-quick-run : $(BUILDDIR)test-clang-quick
	$<

test-clang-ex-run : $(BUILDDIR)test-clang-ex
	$<

test-clang-ec-run : $(BUILDDIR)test-clang-ec
	$<

test-clang-rel-run : $(BUILDDIR)test-clang-rel
	$<

test-clang-all : test-clang-quick-run test-clang-ex-run test-clang-ec-run test-clang-rel-run

.PHONY : clean

clean :
	rm -f $(GCC_BUILDS)

