#include "implementation.h"

#include <invariants/girth.hpp>
#include <io/print_nice.hpp>
#include <operations/copies.hpp>
#include <operations/simplification.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
	Graph g1(createG());
	addV(g1, 0);
	addE(g1, Location(0, 0));
	simplify(g1);
	assert(g1.order() == 1);
	assert(print_nice(g1) == "0:\n");

	Graph g3(createG());
	addV(g3, 0);
	addV(g3, 1);
	addV(g3, 2);
	addE(g3, Location(0, 0));
	addE(g3, Location(0, 1));
	addE(g3, Location(0, 1));
	addE(g3, Location(0, 2));
	addE(g3, Location(1, 2));
	addE(g3, Location(2, 2));
	Graph g4(copy_identical(g3));

	simplify(g3);
	assert(g3.order() == 3);
	assert(print_nice(g3) == "0: 1 2\n1: 0 2\n2: 0 1\n");
	assert(!has_loop(g3));
	assert(!has_parallel_edge(g3));

	assert(has_loop(g4));
	assert(has_parallel_edge(g4));

	remove_loops(g4, 1);
	assert(print_nice(g4) == "0: 0 0 1 1 2\n1: 0 0 2\n2: 0 1 2 2\n");
	remove_loops(g4, 2);
	assert(print_nice(g4) == "0: 0 0 1 1 2\n1: 0 0 2\n2: 0 1\n");
	remove_loops(g4, 0);
	assert(print_nice(g4) == "0: 1 1 2\n1: 0 0 2\n2: 0 1\n");
}
