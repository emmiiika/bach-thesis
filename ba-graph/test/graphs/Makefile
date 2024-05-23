PREFIX = ..

include $(PREFIX)/makerc

CXXFLAGS = $(DEFAULT_CXXFLAGS) -pthread
DBGFLAGS = $(DEFAULT_DBGFLAGS)
LDFLAGS = $(DEFAULT_LDFLAGS) -L$(GOOGLETEST_LIB_PATH)
LDLIBS = $(DEFAULT_LDLIBS) $(GOOGLETEST_LD_OPTIONS)

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
