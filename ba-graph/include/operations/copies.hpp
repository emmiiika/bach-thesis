#ifndef BA_GRAPH_OPERATIONS_COPIES_HPP
#define BA_GRAPH_OPERATIONS_COPIES_HPP

namespace ba_graph
{

// preserves both numbers and vertices
inline Graph copy_identical(const Graph &G, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
#endif
    //TODO keep embeding
    Graph H(createG(f));
    for (auto &r : G)
        addV(H, r.v(), r.n(), f);
    for (auto ii : G.list(RP::all(), IP::primary()))
        addE(H, ii->e(), f);
    return H;
}

// preserves only numbers (and graph structure), vertices are newly created
template <typename T = NoMapper>
typename OneExtraReturnValue<Graph, T>::ReturnType copy_disjoint(const Graph &G, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
#endif
    //TODO keep embeding
    Graph H(createG(f));
    typename T::MappingType l;
    for (auto &r : G)
        addV(H, r.n(), f);
    for (auto ii : G.list(RP::all(), IP::primary())) {
        Edge e = addE(H, Location(ii->n1(), ii->n2()), f).e();
        T::addE(l, ii->e(), e);
    }
    return OneExtraReturnValue<Graph, T>::ret(H, l);
}

template <typename T = NoMapper>
typename OneExtraReturnValue<Graph, T>::ReturnType copy_disjoint(const Graph &G)
{
    return copy_disjoint<T>(G, static_factory);
}

// preserves only numbers (and graph structure), vertices and edges come from another factory
template <typename T = NoMapper>
typename OneExtraReturnValue<Graph, T>::ReturnType copy_other_factory(const Graph &G, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(!check(G, f));
#endif
    Graph H(createG(f));
    typename T::MappingType l;
    for (auto &r : G)
        addV(H, r.n(), f);
    for (auto ii : G.list(RP::all(), IP::primary())) {
        Edge e = addE(H, Location(ii->n1(), ii->n2()), f).e();
        T::addE(l, ii->e(), e);
    }
    return OneExtraReturnValue<Graph, T>::ret(H, l);
}

} // namespace ba_graph

#endif
