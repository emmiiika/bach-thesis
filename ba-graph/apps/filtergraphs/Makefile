CFLAGS = -std=c++17 -fconcepts -fopenmp -I../../include -I/usr/include/nauty
LDFLAGS = -lstdc++fs -lnauty
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG -D_GLIBCXX_DEBUG
COMPILE_DBG = $(CXX) $(CFLAGS) $(DBGFLAGS)
COMPILE = $(CXX) $(CFLAGS) -DNDEBUG -O3

all: filtergraphs filtermultipoles

dbg: filtergraphs_dbg filtermultipoles_dbg


filtergraphs:
	$(COMPILE) filtergraphs.cpp -o filtergraphs $(LDFLAGS)

filtergraphs_dbg:
	$(COMPILE_DBG) filtergraphs.cpp -o filtergraphs_dbg.out $(LDFLAGS)

filtermultipoles:
	$(COMPILE) filtermultipoles.cpp -o filtermultipoles $(LDFLAGS)

filtermultipoles_dbg:
	$(COMPILE_DBG) filtermultipoles.cpp -o filtermultipoles_dbg.out $(LDFLAGS)


clean:
	rm -rf filtergraphs filtergraphs_dbg.out filtermultipoles filtermultipoles_dbg.out


.PHONY: filtergraph filtergraph_dbg filtermultipoles filtermultipoles_dbg
