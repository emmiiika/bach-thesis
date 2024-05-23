PREFIX = ..

include $(PREFIX)/makerc

CXXFLAGS = $(DEFAULT_CXXFLAGS) -pthread
DBGFLAGS = $(DEFAULT_DBGFLAGS)
LDFLAGS = $(DEFAULT_LDFLAGS) -L$(GOOGLETEST_LIB_PATH)
LDLIBS = $(DEFAULT_LDLIBS) $(GOOGLETEST_LD_OPTIONS)

COMPILE_DBG = $(CXX) $(CXXFLAGS) $(DBGFLAGS) $(LDFLAGS)



all: implementation_basic test_all

implementation_basic:
	cp implementation_basic.h implementation.h

test_all:
	$(COMPILE_DBG) all.cpp -o all.out $(LDLIBS)
	./all.out

clean:
	rm -rf *.out

	

.PHONY: clean all compile test1 compile1 test2 compile2 test3 compile3 test4 compile4 test5 compile5 test6 compile6 test7 compile7
