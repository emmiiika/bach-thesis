#ifndef BA_GRAPH_SNARKS_DOT_PRODUCT_HPP
#define BA_GRAPH_SNARKS_DOT_PRODUCT_HPP

#include "../operations/copies.hpp"
#include "../operations/add_graph.hpp"
#include "../operations/basic.hpp"

namespace ba_graph
{

inline Graph &two_product(Graph &G1, const Location &l1,
                   const Graph &G2, const Location &l2,
                   int g2offset, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G1, f));
    assert(check(G2, f));
    assert(G1.contains(l1));
    assert(G2.contains(l2));
#endif

    add_graph(G1, G2, g2offset, f);
    deleteE(G1, l1, f);
    Number u2 = l2.n1() + g2offset;
    Number v2 = l2.n2() + g2offset;
    deleteE(G1, Location(u2, v2, l2.index()), f);
    addE(G1, Location(l1.n1(), u2), f);
    addE(G1, Location(l1.n2(), v2), f);

    return G1;
}

inline Graph &three_product(Graph &G1, Number v1, std::vector<Number> neighbours1,
                    const Graph &G2, Number v2, std::vector<Number> neighbours2,
                    int g2offset, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G1, f));
    assert(check(G2, f));
    assert(neighbours1.size() == 3);
    assert(neighbours2.size() == 3);
    assert(G1.contains(v1));
    assert(G1.contains(Location(v1, neighbours1[0])));
    assert(G1.contains(Location(v1, neighbours1[1])));
    assert(G1.contains(Location(v1, neighbours1[2])));
    assert(G2.contains(v2));
    assert(G2.contains(Location(v2, neighbours2[0])));
    assert(G2.contains(Location(v2, neighbours2[1])));
    assert(G2.contains(Location(v2, neighbours2[2])));
#endif

    add_graph(G1, G2, g2offset, f);
    deleteV(G1, v1, f);
    deleteV(G1, v2 + g2offset, f);
    for (std::vector<Number>::size_type i = 0; i < 3; ++i)
        addE(G1, Location(neighbours1[i], neighbours2[i] + g2offset), f);

    return G1;
}

inline Graph &dot_product(Graph &G, const Location &l21, const Location &l22,
                   const Graph &g1, const Location &l1,
                   int g2offset, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(check(g1, f));
    assert(g1.contains(l1));
    assert(g1[l1.n1()].degree() == 3);
    assert(g1[l1.n2()].degree() == 3);
    assert(G.contains(l21));
    assert(G.contains(l22));
    assert(G.find(l21)->e() != G.find(l22)->e());
    for (const auto &rot : g1)
        assert(!G.contains(rot.v())); //test disjoint
#endif
    Number n21 = l21.n1();
    Number n22 = l21.n2();
    Number n23 = l22.n1();
    Number n24 = l22.n2();
    deleteE(G, Location(l21), f);
    deleteE(G, Location(l22), f);

    add_graph(G, g1, g2offset, f);
    Number n11 = l1.n1().to_int() + g2offset;
    Number n12 = l1.n2().to_int() + g2offset;
    deleteE(G, Location(n11, n12, l1.index()), f);
    auto vec = G[n11].list(IP::all());
    Number n111 = vec[0]->n2();
    Number n112 = vec[1]->n2();
    if (n112 < n111)
        std::swap(n111, n112);
    vec = G[n12].list(IP::all());
    Number n121 = vec[0]->n2();
    Number n122 = vec[1]->n2();
    if (n122 < n121)
        std::swap(n121, n122);
#ifdef BA_GRAPH_DEBUG
    assert(n11 != n12);
    std::set s = {n111, n112, n121, n122};
    assert(s.count(n11) == 0);
    assert(s.count(n12) == 0);
#endif
    deleteV(G, n11, f);
    deleteV(G, n12, f);

    addE(G, Location(n111, n21), f);
    addE(G, Location(n112, n22), f);
    addE(G, Location(n121, n23), f);
    addE(G, Location(n122, n24), f);

    return G;
}

// remove the given 5-circuit from each of the given graphs and join
// the resulting multipoles as in the Petersen graph
inline Graph &star_product(Graph &G, std::vector<Number> v,
                    const Graph &g1, std::vector<Number> v1,
                    int g2offset, Factory &f = static_factory)
{
    v.push_back(v[0]);
    v1.push_back(v1[0]);
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(check(g1, f));
    assert(v.size() == 6);
    assert(v1.size() == 6);
    for (int i = 0; i < 5; i++) {
        assert(G.contains(v[i]));
        assert(g1.contains(v1[i]));
        assert(G[v[i]].count(v[i + 1]) == 1);
        assert(g1[v1[i]].count(v1[i + 1]) == 1);
    }
#endif
    for (int i = 0; i < 5; i++)
        deleteE(G, Location(v[i], v[i + 1]));

    add_graph(G, g1, g2offset, f);
    for (int i = 0; i < 6; i++)
        v1[i] = v1[i]+g2offset;
    for (int i = 0; i < 5; i++)
        deleteE(G, Location(v1[i], v1[i + 1]));
    for (int i = 0; i < 5; i++) {
        identify_vertices(G, v[i], v1[(2 * i) % 5], f);
        suppress_vertex(G, v[i], f);
    }

    return G;
}

} // namespace ba_graph

#endif
