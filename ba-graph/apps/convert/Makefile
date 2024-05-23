CFLAGS = -std=c++17 -fconcepts -I../../include
LDFLAGS =
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG
COMPILE_DBG = $(CXX) $(CFLAGS) $(DBGFLAGS)
COMPILE = $(CXX) $(CFLAGS) -DNDEBUG -O3

all: convert


convert:
	$(COMPILE) convert.cpp -o convert $(LDFLAGS)

convert_dbg:
	$(COMPILE_DBG) convert.cpp -o convert $(LDFLAGS)


clean:
	rm -rf *.out


.PHONY: clean all convert convert_dbg
