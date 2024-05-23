#include "implementation.h"

#include <invariants/girth.hpp>
#include <io/graph6.hpp>
#include <graphs.hpp>

#include <cassert>
#include <cmath>
#include <iostream>

using namespace ba_graph;

int main()
{
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

    assert(has_loop(g3));
    assert(has_parallel_edge(g3));
    assert(has_short_cycle(g3, 1));
    assert(has_short_cycle(g3, 2));

    addE(g3, Location(0, 0));
    addE(g3, Location(0, 0));
    auto pe = parallel_edges(g3);
    assert(pe.size() == 2);
    assert(pe[0].size() + pe[1].size() == 5);
    assert(std::min(pe[0].size(), pe[1].size()) == 2);

    const int n = 11;
    Graph g(empty_graph(n));
    assert(girth(g) == -1);
    assert(!has_loop(g));
    assert(!has_parallel_edge(g));

    const int a = 5, b = 6;
    g = std::move(complete_bipartite_graph(a, b));
    assert(girth(g) == 4);
    assert(!has_loop(g));
    assert(!has_parallel_edge(g));
    assert(!has_short_cycle(g, 3));
    assert(has_short_cycle(g, 4));

    g = std::move(empty_graph(4));
    addE(g, Location(0, 1));
    addE(g, Location(0, 2));
    addE(g, Location(0, 3));
    addE(g, Location(1, 2));
    assert(shortest_cycle_length(g, 0) == 3);
    assert(shortest_cycle_length(g, 1) == 3);
    assert(shortest_cycle_length(g, 2) == 3);
    assert(shortest_cycle_length(g, 3) == -1);
    assert(girth(g) == 3);
    addE(g, Location(1, 2));
    assert(shortest_cycle_length(g, 0) == 3);
    assert(shortest_cycle_length(g, 1) == 2);
    assert(shortest_cycle_length(g, 2) == 2);
    assert(shortest_cycle_length(g, 3) == -1);
    assert(girth(g) == 2);
    addE(g, Location(1, 2));
    assert(shortest_cycle_length(g, 0) == 3);
    assert(shortest_cycle_length(g, 1) == 2);
    assert(shortest_cycle_length(g, 2) == 2);
    assert(shortest_cycle_length(g, 3) == -1);
    assert(girth(g) == 2);
    addE(g, Location(1, 1));
    assert(shortest_cycle_length(g, 0) == 3);
    assert(shortest_cycle_length(g, 1) == 1);
    assert(shortest_cycle_length(g, 2) == 2);
    assert(shortest_cycle_length(g, 3) == -1);
    assert(girth(g) == 1);

    g = std::move(create_petersen());
    assert(girth(g) == 5);
    assert(!has_loop(g));
    assert(!has_parallel_edge(g));
    assert(!has_short_cycle(g, 4));
    assert(has_short_cycle(g, 5));
    assert(has_short_cycle(g, 6));

    const std::string graphsDir = "../../resources/graphs/cubic/cages/";
    auto graphs = read_graph6_file(graphsDir + "cub0024_g07_cage.s6").graphs();
    assert(girth(graphs[0]) == 7);
    assert(girth(graphs[0], 1) == -1);
    assert(girth(graphs[0], 2) == -1);
    assert(girth(graphs[0], 3) == -1);
    assert(girth(graphs[0], 4) == -1);
    assert(girth(graphs[0], 5) == -1);
    assert(girth(graphs[0], 6) == -1);
    assert(girth(graphs[0], 7) == 7);
    assert(girth(graphs[0], 8) == 7);

    graphs = read_graph6_file(graphsDir + "cub0126_g12_cage.s6").graphs();
    assert(girth(graphs[0]) == 12);

    graphs = read_graph6_file(graphsDir + "cub0620_g15_cage.s6").graphs();
    assert(girth(graphs[0]) == 15);

    g = std::move(empty_graph(2));
    addE(g, Location(0, 1));
    addE(g, Location(0, 1));
    addE(g, Location(0, 1));
    addE(g, Location(0, 1));
    assert(shortest_cycle(g, 0).size() == 2);
    assert(shortest_cycle(g, 0, 1).size() == 0);
    assert(shortest_cycle(g, 0, 2).size() == 2);
    assert(!has_short_cycle(g, 1));
    assert(has_short_cycle(g, 2));
}
