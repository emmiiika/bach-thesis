#ifndef BA_GRAPH_RANDOM_RANDOM_GRAPHS_HPP
#define BA_GRAPH_RANDOM_RANDOM_GRAPHS_HPP

#include "../graphs/basic.hpp"

#include <cstdlib>
#include <iostream>

namespace ba_graph {


inline Graph random_pseudograph(int order, int size, Factory &f=static_factory) {
#ifdef BA_GRAPH_DEBUG
    assert(order >= 0); assert(size >= 0);
#endif
    Graph G(empty_graph(order, f));
    for (; size > 0; --size) {
        int u = rand() % order;
        int v = rand() % order;
        addE(G, Location(u, v), f);
    }
    return G;
}

inline Graph random_multigraph(int order, int size, Factory &f=static_factory) {
#ifdef BA_GRAPH_DEBUG
    assert(order >= 0); assert(size >= 0);
#endif
    Graph G(empty_graph(order, f));
    for (; size > 0; --size) {
        int u = rand() % order;
        int v = rand() % (order - 1);
        if (v >= u) ++v;
        addE(G, Location(u, v), f);
    }
    return G;
}

// attempts to add maxSize edges, but may fail (always finishes)
inline Graph random_simple_graph_unreliable(int order, int maxSize, Factory &f=static_factory) {
#ifdef BA_GRAPH_DEBUG
    assert(order >= 0); assert(maxSize >= 0);
#endif
    Graph G(empty_graph(order, f));
    for (int i = 0; i < maxSize; i++) {
        int u = rand() % order;
        int v = rand() % order;
        if ((u != v) && !G.contains(Location(u, v)))
            addE(G, Location(u, v), f);
    }
    return G;
}

namespace internal {

inline int randomAvailableInt(std::vector<bool> a) {
    int aCount = 0;
    for (std::vector<bool>::size_type i = 0; i < a.size(); i++) {
        if (a[i]) aCount++;
    }
    if (aCount == 0)
        throw std::runtime_error("no integers available");
    int v = rand() % aCount;
    for (int i = 0; i <= v; i++) {
        if (!a[i]) v++;
    }
    return v;
}

}

inline Graph random_simple_graph_reliable(int order, int size, Factory &f=static_factory) {
#ifdef BA_GRAPH_DEBUG
    assert(order >= 0); assert(size >= 0);
#endif
    Graph G(empty_graph(order, f));
    for (; size > 0; --size) {
        std::vector<bool> au; // available for u
        for (int i = 0; i < order; i++)
            au.push_back(G[i].degree() < order - 1);
        int u = internal::randomAvailableInt(au);

        std::vector<bool> av; // available for v
        for (int i = 0; i < order; i++)
            av.push_back(true);
        av[u] = false;
        for (auto &i : G[u])
            av[i.n2().to_int()] = false;
        int v = internal::randomAvailableInt(av);
        addE(G, Location(u, v), f);
    }
    return G;
}

inline Graph random_regular_multigraph(int order, int degree, Factory &f=static_factory) {
#ifdef BA_GRAPH_DEBUG
    assert(order >= 0); assert((degree >= 0) && (degree < order));
    if (degree % 2 == 1) assert(order % 2 == 0);
#endif
    Graph G(empty_graph(order, f));
    std::vector<int> candidates;
    for (int u = 0; u < order; u++)
        candidates.push_back(u);
    for (int j = 0; j < degree; ++j) { // we avoid loops by this
    // (near the end of generation, there are no vertices of degree 1 that could result in a loop)
        for (int u = 0; u < order; ++u) {
            if (G[u].degree() == degree)
                continue;
            int v = -1;
            int index = rand() % candidates.size();
            while (v == -1) {
                if (G[candidates[index]].degree() == degree) {
                    candidates[index] = candidates.back();
                    candidates.pop_back();
                    index = rand() % candidates.size();
                } else if (candidates[index] == u) {
                    index = rand() % (candidates.size() - 1);
                    if (candidates[index] == u) index = candidates.size() - 1;
                } else {
                    v = candidates[index];
                    break;
                }
            }
            addE(G, Location(u, v), f);
        }
    }
    return G;
}

inline Graph random_bipartite_regular_multigraph(int order, int degree, Factory &f=static_factory) {
#ifdef BA_GRAPH_DEBUG
    assert((order >= 0) && (order % 2 == 0)); assert((degree >= 0) && (degree < order));
#endif
    Graph G(empty_graph(order, f));
    for (int u = 0; u < order / 2; ++u) {
        for (int j = 0; j < degree; ++j) {
            std::vector<bool> av;
            for (int k = 0; k < order/2; ++k) {
                int v = k + order/2;
                av.push_back(G[v].degree() < degree);
            }
            int v = internal::randomAvailableInt(av) + order/2;
            addE(G, Location(u, v), f);
        }
    }
    return G;
}


} // namespace end

#endif
