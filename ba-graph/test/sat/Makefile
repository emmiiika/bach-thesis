PREFIX = ..

include $(PREFIX)/makerc

CXXFLAGS = $(DEFAULT_CXXFLAGS) -I$(CRYPTOMINISAT_INCLUDE_PATH)
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG
# cannot use -D_GLIBCXX_DEBUG because of cryptominisat
LDFLAGS = $(DEFAULT_LDFLAGS) -L$(CRYPTOMINISAT_LIB_PATH)
LDLIBS = $(DEFAULT_LDLIBS) -lcryptominisat5

COMPILE_DBG = $(CXX) $(CXXFLAGS) $(DBGFLAGS) $(LDFLAGS)

TESTS_TO_RUN := $(patsubst test_%.cpp, %, $(wildcard test_*.cpp))


all: implementation_basic run

implementation_basic:
	cp $(PREFIX)/implementation_basic.h implementation.h

run: $(TESTS_TO_RUN)

$(TESTS_TO_RUN): %:
	@echo "------------------------------ $* ------------------------------"
	$(COMPILE_DBG) test_$*.cpp -o $*.out $(LDLIBS)
	./$*.out

clean:
	rm -rf *.out


.PHONY: all clean run implementation_basic
