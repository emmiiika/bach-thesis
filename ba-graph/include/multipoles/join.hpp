#ifndef BA_GRAPH_MULTIPOLES_JOIN_HPP
#define BA_GRAPH_MULTIPOLES_JOIN_HPP

#include "multipoles.hpp"
#include "../operations/add_graph.hpp"
#include "../operations/basic.hpp"

namespace ba_graph
{

inline void join(Graph &G, Multipole &c, int con, const Graph &g1, const Multipole &c1, int con1,
          int offset, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(check(g1, f));
    assert(c.size() > con);
    assert(c1.size() > con1);
    assert(c[con].size() == c1[con1].size());
    assert(c[con].check_in(G));
    assert(c1[con1].check_in(g1));
#endif
    add_disjoint(G, g1, offset, f);
    for (int i = 0; i < c[con].size(); i++) {
        identify_vertices(G, c[con][i], c1[con1][i] + offset, f);
        suppress_vertex(G, c[con][i], f);
    }
    int old_c = c.size();
    c.connectors.insert(c.connectors.end(), c1.connectors.begin(), c1.connectors.end());
    c.connectors.erase(c.connectors.begin() + old_c + con1);
    c.connectors.erase(c.connectors.begin() + con);
    for (auto it = c.connectors.begin() + (old_c - 1); it != c.connectors.end(); it++)
        for (Number &n : it->numbers)
            n = n+offset;
}

inline void join(Graph &G, Multipole &c, int con, int con1, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(con != con1);
    assert(c.size() > con);
    assert(c.size() > con1);
    assert(c[con].size() == c[con1].size());
    assert(c[con].check_in(G));
    assert(c[con1].check_in(G));
#endif
    for (int i = 0; i < c[con].size(); i++) {
        identify_vertices(G, c[con][i], c[con1][i], f);
        suppress_vertex(G, c[con][i], f);
    }
    if (con > con1)
        std::swap(con, con1);
    c.connectors.erase(c.connectors.begin() + con1);
    c.connectors.erase(c.connectors.begin() + con);
}

} // namespace ba_graph

#endif
