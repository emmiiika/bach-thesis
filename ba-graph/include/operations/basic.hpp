#ifndef BA_GRAPH_OPERATIONS_BASIC_HPP
#define BA_GRAPH_OPERATIONS_BASIC_HPP

namespace ba_graph
{

inline Number max_number(const Graph &G)
{
    Number maxN = -1;
    for (auto &r : G) {
        if (r.n() > maxN)
            maxN = r.n();
    }
    return maxN;
}

inline int min_offset(const Graph &G)
{
    return max_number(G).to_int() + 1;
}

// ======================== identify vertices ===================================

template <typename T = NoMapper>
typename T::MappingType identify_vertices(Graph &G, Number vertex_target, Number vertex_source, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(vertex_target));
    assert(G.contains(vertex_source));
#endif
    typename T::MappingType m;
    for (auto &i : G[vertex_source]) {
        Edge ne;
        if (i.is_loop() && i.is_primary())
            ne = addE(G, Location(vertex_target, vertex_target), f).e();
        if (!i.is_loop() && i.is_primary()) //to preserve orientation
            ne = addE(G, Location(vertex_target, i.n2()), f).e();
        if (!i.is_loop() && !i.is_primary()) //to preserve orientation
            ne = addE(G, Location(i.n2(), vertex_target), f).e();
        if (!ne.is_null())
            T::addE(m, i.e(), ne);
    }
    T::addNV(m, vertex_source, vertex_target, G[vertex_source].v(), G[vertex_target].v());
    deleteV(G, vertex_source, f);
    return m;
}

template <typename T = NoMapper>
typename T::MappingType identify_vertices(Graph &G, Number vertex_target, Number vertex_source)
{
    return identify_vertices<T>(G, vertex_target, vertex_source, static_factory);
}

// ======================== split vertex ===================================

template <typename T = NoMapper>
typename T::MappingType split_vertex(Graph &G, Number vertex_to_split, Number first_new, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(vertex_to_split));
    assert(!G.contains(first_new));
#endif
    typename T::MappingType m;
    HalfEdgeLabeling<Number> v;
    //we create new vertices and map them with halfedges
    for (auto &i : G[vertex_to_split]) {
        addV(G, first_new, f);
        v.set(i.h1(), first_new); 
        first_new=first_new+1;
    }
    for (auto &i : G[vertex_to_split]) {
        Edge ne;
        if (i.is_loop() && i.is_primary())
            ne = addE(G, Location(v[i.h1()], v[i.h1().h2()]), f).e();
        if (!i.is_loop() && i.is_primary()) //to preserve orientation
            ne = addE(G, Location(v[i.h1()], i.n2()), f).e();
        if (!i.is_loop() && !i.is_primary()) //to preserve orientation
            ne = addE(G, Location(i.n2(), v[i.h1()]), f).e();
        if (!ne.is_null())
            T::addE(m, i.e(), ne);
    }
    deleteV(G, vertex_to_split, f);
    return m;
}

template <typename T = NoMapper>
typename T::MappingType split_vertex(Graph &G, Number vertex_to_split, Number first_new)
{
    return split_vertex<T>(G, vertex_to_split, first_new, static_factory);
}

// ======================== suppress vertex ===================================

template <typename T = NoMapper>
typename T::MappingType suppress_vertex(Graph &G, Number vertex_to_suppress, Factory &f)
{
    typename T::MappingType m;
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(vertex_to_suppress));
    assert(G[vertex_to_suppress].degree() == 2);
#endif
    auto it = G[vertex_to_suppress].begin();
    Incidence &i1 = *(it++);
    Incidence &i2 = *it;
#ifdef BA_GRAPH_DEBUG
    assert(!i1.is_loop());
    assert(!i2.is_loop());
#endif
    Edge e = addE(G, Location(i1.n2(), i2.n2()), f).e();
    T::addH(m, i1.h1().h2(), e.h1());
    T::addH(m, i2.h1().h2(), e.h2());
    deleteV(G, vertex_to_suppress, f);
    return m;
}

template <typename T = NoMapper>
typename T::MappingType suppress_vertex(Graph &G, Number vertex_to_supress)
{
    return suppress_vertex<T>(G, vertex_to_supress, static_factory);
}

// ======================== subdivide edge ===================================

template <typename T = NoMapper>
typename T::MappingType subdivide_edge(Graph &G, const Location &l, Number newVertex, Factory &f)
{
    typename T::MappingType m;
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(l));
    assert(!G.contains(newVertex));
#endif
    auto it= G[l.n1()].find(l);
    HalfEdge oldh1=it->h1();
    Vertex v=addV(G, newVertex, f).v();
    deleteE(G, l, f);

    if (oldh1.is_primary()) {
        Edge e1=createE(oldh1.v1(), v, f);
        Edge e2=createE(v, oldh1.v2(), f);
        addE(G, G[l.n1()], G[newVertex], e1, f);
        addE(G, G[newVertex], G[l.n2()], e2, f);
        T::addH(m, oldh1, e1.h1());
        T::addH(m, oldh1.h2(), e2.h2());
    }
    else {
        Edge e1=createE(oldh1.v2(), v, f);
        Edge e2=createE(v, oldh1.v1(), f);
        addE(G, G[l.n2()], G[newVertex], e1, f);
        addE(G, G[newVertex], G[l.n1()], e2, f);
        T::addH(m, oldh1.h2(), e1.h1());
        T::addH(m, oldh1, e2.h2());
    }
    return m;
}

template <typename T = NoMapper>
typename T::MappingType subdivide_edge(Graph &G, const Location &l, Number newVertex)
{
    return subdivide_edge<T>(G, l, newVertex, static_factory);
}

template <typename T = NoMapper>
typename T::MappingType subdivide_edge(Graph &G, const Edge &e, Number newVertex, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(e.v1(), e));
    assert(!G.contains(newVertex));
#endif
    Location l = G.find(e.v1(), e).second->l();
    return subdivide_edge<T>(G, l, newVertex, f);
}

template <typename T = NoMapper>
typename T::MappingType subdivide_edge(Graph &G, const Edge &e, Number newVertex)
{
    return subdivide_edge<T>(G, e, newVertex, static_factory);
}

// ======================== split edge ===================================

template <typename T = NoMapper>
typename T::MappingType split_edge(Graph &G, const Location &l, Number firstNew, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(l));
    assert(!G.contains(firstNew));
    assert(!G.contains(firstNew + 1));
#endif
    typename T::MappingType m;
    auto &i = *G.find(l);
    addV(G, firstNew, f);
    Location l1(firstNew, l.n1());
    addE(G, l1, f);
    addV(G, firstNew + 1, f);
    Location l2(firstNew + 1, l.n2());
    addE(G, l2, f);
    T::addH(m, i.h1(), G.find(l1)->h1());
    T::addH(m, i.h2(), G.find(l2)->h1());
    deleteE(G, l, f);
    return m; // TODO check if the mapper is correct
}

template <typename T = NoMapper>
typename T::MappingType split_edge(Graph &G, const Location &l, Number firstNew)
{
    return split_edge<T>(G, l, firstNew, static_factory);
}

template <typename T = NoMapper>
typename T::MappingType split_edge(Graph &G, const Edge &e, Number firstNew, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(e.v1(), e));
    assert(!G.contains(firstNew));
    assert(!G.contains(firstNew + 1));
#endif
    Location l = G.find(e.v1(), e).second->l();
    return split_edge<T>(G, l, firstNew, f);
}

template <typename T = NoMapper>
typename T::MappingType split_edge(Graph &G, const Edge &e, Number firstNew)
{
    return split_edge<T>(G, e, firstNew, static_factory);
}

// ======================== contract edge ===================================

template <typename T = NoMapper>
typename T::MappingType contract_edge(Graph &G, Location l, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(G.contains(l));
#endif
    deleteE(G, l, f);
    typename T::MappingType m;
    if (l.n1() != l.n2())
        m = identify_vertices<T>(G, l.n1(), l.n2(), f);
    return m;
}

template <typename T = NoMapper>
typename T::MappingType contract_edge(Graph &G, Location l)
{
    return contract_edge<T>(G, l, static_factory);
}

template <typename T = NoMapper>
typename T::MappingType contract_edge(Graph &G, const Edge &e, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    G.contains(e.v1(), e);
#endif
    auto &i = *G.find(e.v1(), e).second;
    if (!i.is_primary())
        i = i.jump();
    Location l = i.l();
    return contract_edge<T>(G, l, f);
}

template <typename T = NoMapper>
typename T::MappingType contract_edge(Graph &G, const Edge &e)
{
    return contract_edge<T>(G, e, static_factory);
}

} // namespace ba_graph

#endif
