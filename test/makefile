CALL_gcc = g++
CALL_clang = clang++

COMPILERS=gcc clang
VARIANTS=quick ec ex rel
BUILDDIR=build/

ifeq ($(OS),Windows_NT)
	detected_OS := Windows
else
	detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
endif

EEXT=
ifeq ($(detected_OS),Windows)
	EEXT=.exe
endif

CXXFLAGS_gcc_base=-std=c++17 -Wall -Wextra -Werror -Wconversion -Wsign-promo -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=5 -fstrict-aliasing -I../uSer
CXXFLAGS_gcc_quick=-DUSER_TEST_QUICK
CXXFLAGS_gcc_ec=-fsanitize=undefined -g
CXXFLAGS_gcc_ex=-DUSER_EXCEPTIONS -fsanitize=undefined -g
CXXFLAGS_gcc_rel=-O2 -flto


CXXFLAGS_clang_base=-std=c++1z -Wall -Wextra -Werror -Wconversion -Wsign-promo -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=5 -fstrict-aliasing -I../uSer
CXXFLAGS_clang_quick=-DUSER_TEST_QUICK
CXXFLAGS_clang_ec=-fsanitize=undefined -g
CXXFLAGS_clang_ex=-DUSER_EXCEPTIONS -fsanitize=undefined -g
CXXFLAGS_clang_rel=-O2

PRODUCTS=$(foreach cl,$(COMPILERS),$(foreach v,$(VARIANTS),$(cl)-$(v)))

DEP=../uSer/uSer.hh

all : $(BUILDDIR).gitignore run

define GEN_RULE
$(BUILDDIR)test-$(1)-$(2)$(EEXT) : test.cpp $(DEP)
	$(CALL_$(1)) $$< -o $$@ $$(CXXFLAGS_$(1)_base) $$(CXXFLAGS_$(1)_$(2))

run-$(1)-$(2) : $(BUILDDIR)test-$(1)-$(2)$(EEXT)
	./$$<
endef

define GEN_CLRULE
run-$(1) : $$(foreach v,$$(VARIANTS),run-$(1)-$$(v)) ;
endef

$(foreach prod,$(PRODUCTS),$(eval $(call GEN_RULE,$(word 1,$(subst -, ,$(prod))),$(word 2,$(subst -, ,$(prod))))))

$(foreach cl,$(COMPILERS),$(eval $(call GEN_CLRULE,$(cl))))

build : $(BUILDDIR).gitignore $(foreach prod,$(PRODUCTS),$(BUILDDIR)test-$(prod)$(EEXT))

run : $(foreach prod,$(PRODUCTS),run-$(prod))
	@echo "All variants ran successfully."

.PHONY : clean test $(foreach prod,$(PRODUCTS),run-$(prod))

clean :
	rm -f $(foreach prod,$(PRODUCTS),$(BUILDDIR)test-$(prod)$(EEXT))

