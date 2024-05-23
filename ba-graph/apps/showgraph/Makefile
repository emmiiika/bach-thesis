CFLAGS = -std=c++17 -fconcepts -I../../include
LDFLAGS =
DBGFLAGS = -g -O0 -pedantic -Wall -Wextra -DBA_GRAPH_DEBUG
COMPILE_DBG = $(CXX) $(CFLAGS) $(DBGFLAGS)
COMPILE = $(CXX) $(CFLAGS) -DNDEBUG -O3

all: showgraph


showgraph:
	$(COMPILE) showgraph.cpp -o showgraph $(LDFLAGS)

showgraph_dbg:
	$(COMPILE_DBG) showgraph.cpp -o showgraph $(LDFLAGS)


clean:
	rm -rf *.out


.PHONY: clean all showgraph showgraph_dbg
