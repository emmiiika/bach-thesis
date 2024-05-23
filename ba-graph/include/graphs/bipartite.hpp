#ifndef BA_GRAPH_GRAPHS_BIPARTITE_HPP
#define BA_GRAPH_GRAPHS_BIPARTITE_HPP

#include "basic.hpp"

namespace ba_graph
{

inline Graph complete_bipartite_graph(int a, int b, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert((a >= 1) && (b >= 1));
#endif
    Graph G(empty_graph(a + b, f));
    for (int i = 0; i < a; i++)
        for (int j = a; j < a + b; j++)
            addE(G, Location(i, j), f);
    return G;
}

inline Graph square_grid(int m, int n, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert((m >= 0) && (n >= 0));
#endif
    Graph G(empty_graph(m * n, f));
    if (m == 0 || n == 0)
        return G;
    for (int i = 0; i < m - 1; ++i) {
        for (int j = 0; j < n; ++j) {
            int v = i + j * m;
            addE(G, Location(v, v + 1), f);
            if (v + m < G.order())
                addE(G, Location(v, v + m), f);
        }
    }
    for (int j = 0; j < n - 1; ++j)
        addE(G, Location(m - 1 + j*m, m - 1 + (j+1) * m), f);
    return G;
}

inline Graph wrapped_square_grid(int m, int n, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert((m >= 0) && (n >= 0));
#endif
    Graph G(square_grid(m, n, f));
    if (m == 0 || n == 0)
        return G;
    for (int i = 0; i < m; ++i)
        addE(G, Location(i, i + m * (n - 1)), f);
    for (int j = 0; j < n; ++j)
        addE(G, Location(j * m, j * m + m - 1), f);
    return G;
}

} // namespace ba_graph

#endif
