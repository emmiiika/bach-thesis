#ifndef BA_GRAPH_OPERATIONS_SIMPLIFICATION_HPP
#define BA_GRAPH_OPERATIONS_SIMPLIFICATION_HPP

#include "basic.hpp"

namespace ba_graph
{


inline Graph &remove_loops(Graph &G, Number v, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(v));
#endif
    int loopsCount = G[v].count(v) / 2;
    for (int i = 0; i < loopsCount; i++)
        deleteE(G, Location(v, v), f);
    return G;
}

inline Graph &remove_loops(Graph &G, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
#endif
    for (auto &r : G) {
        int loopsCount = r.count(r.n()) / 2;
        for (int i = 0; i < loopsCount; i++)
            deleteE(G, Location(r.n(), r.n()), f);
    }
    return G;
}

inline Graph &remove_parallel_edges(Graph &G, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
#endif
    for (auto &r : G) {
        std::vector<Location> edgesToRemove;
        std::set<Number> neighbours;
        for (auto &i : r) {
            if (!i.is_loop()) {
                if (neighbours.find(i.n2()) != neighbours.end())
                    edgesToRemove.push_back(Location(r.n(), i.n2()));
                else
                    neighbours.insert(i.n2());
            }
        }
        for (auto loc : edgesToRemove)
            deleteE(G, loc, f);
    }
    return G;
}

inline Graph &simplify(Graph &G, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
#endif
    remove_loops(G, f);
    remove_parallel_edges(G, f);
    return G;
}

} // namespace ba_graph

#endif
