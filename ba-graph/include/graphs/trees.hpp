#ifndef BA_GRAPH_GRAPHS_TREES_HPP
#define BA_GRAPH_GRAPHS_TREES_HPP

#include "../operations/add_graph.hpp"

namespace ba_graph
{

// the tree has root numbered 0
inline Graph tree(int branching_factor, int depth, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(branching_factor >= 1);
    assert(depth >= 0);
#endif
    Graph G = createG(f);
    addV(G, 0, f);
    std::vector<Number> previousLevel = {0};
    Number next = 1;
    for (int d = 1; d <= depth; ++d) {
        std::vector<Number> level;
        for (auto v : previousLevel) {
            for (int i = 0; i < branching_factor; ++i) {
                addV(G, next, f);
                addE(G, Location(v, next), f);                
                level.push_back(next);
                next = next + 1;
            }
        }
        previousLevel = level;
    }
    return G;
}

// a tree as if taken from a regular graph of large enough girth
// leaves have degree 1
inline Graph regular_tree(int degree, int depth, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(degree >= 1);
    assert(depth >= 0);
#endif
    Graph G = createG(f);
    addV(G, 0, f);
    if (depth == 0)
        return G;
    Graph H = tree(degree - 1, depth - 1, f);
    for (int i = 0; i < degree; ++i) {
        add_disjoint(G, H, G.order(), f);
        addE(G, Location(0, 1 + i * H.order()), f);
    }
    return G;
} // namespace ba_graph

}

#endif
