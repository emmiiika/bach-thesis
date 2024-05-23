#ifndef BA_GRAPH_GRAPHS_BASIC_HPP
#define BA_GRAPH_GRAPHS_BASIC_HPP

namespace ba_graph
{

inline Graph empty_graph(int cnt, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(cnt >= 0);
#endif
    Graph G(createG(f));
    addMultipleV(G, cnt, 0, f);
    return G;
}

inline Graph complete_graph(int cnt, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(cnt >= 0);
#endif
    Graph G(empty_graph(cnt, f));
    for (int i = 0; i < cnt; i++)
        for (int j = i + 1; j < cnt; j++)
            addE(G, Location(i, j), f);
    return G;
}

inline Graph path(int length, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(length >= 0);
#endif
    Graph G(empty_graph(length + 1));
    for (int i = 0; i < length; i++)
        addE(G, Location(i, i + 1), f);
    return G;
}

inline Graph circuit(int length, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(length >= 0);
#endif
    Graph G(empty_graph(length));
    for (int i = 0; i < length; i++)
        addE(G, Location(i, (i + 1) % length), f);
    return G;
}

} // namespace ba_graph

#endif
