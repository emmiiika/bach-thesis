#ifndef BA_GRAPH_GRAPHS_SNARKS_HPP
#define BA_GRAPH_GRAPHS_SNARKS_HPP

#include "basic.hpp"

namespace ba_graph
{

inline Graph create_petersen(Factory &f = static_factory)
{
    Graph g(empty_graph(10, f));
    //outer cycle
    addE(g, Location(0, 1), f);
    addE(g, Location(1, 2), f);
    addE(g, Location(2, 3), f);
    addE(g, Location(3, 4), f);
    addE(g, Location(4, 0), f);
    //inner cycle
    addE(g, Location(5, 7), f);
    addE(g, Location(7, 9), f);
    addE(g, Location(9, 6), f);
    addE(g, Location(6, 8), f);
    addE(g, Location(8, 5), f);
    //other edges
    addE(g, Location(0, 5), f);
    addE(g, Location(1, 6), f);
    addE(g, Location(2, 7), f);
    addE(g, Location(3, 8), f);
    addE(g, Location(4, 9), f);
    return g;
}

inline Graph create_isaacs(int k, Factory &f = static_factory)
{
    Graph g(empty_graph(4 * k));
    for (int i = 0; i < k; ++i) {
        addE(g, Location(i, i + k), f);
        addE(g, Location(i + k, i + 2 * k), f);
        addE(g, Location(i + k, i + 3 * k), f);
        addE(g, Location(i, (i + 1) % k), f);
        addE(g, Location(i + 2 * k, (i + 1) % k + 3 * k), f);
        addE(g, Location(i + 3 * k, (i + 1) % k + 2 * k), f);
    }
    return g;
}

} // namespace ba_graph

#endif
