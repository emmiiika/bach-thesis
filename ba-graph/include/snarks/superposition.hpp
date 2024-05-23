#ifndef BA_GRAPH_SNARKS_SUPERPOSITION_HPP
#define BA_GRAPH_SNARKS_SUPERPOSITION_HPP

#include "../operations/copies.hpp"
#include "../operations/add_graph.hpp"
#include "../operations/basic.hpp"
#include "../multipoles/join.hpp"

#include <vector>
#include <functional>

namespace ba_graph
{

inline Graph &superposition(Graph &G, const std::vector<Number> &vs,
                     const std::vector<const Graph *> &hs,
                     const std::vector<Multipole> &ms,
                     int offset, int offset2, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(vs.size() > 1);
    assert(std::set<Number>(vs.begin(), vs.end()).size() == vs.size());
    if (vs.size() == 2)
        assert(G[vs[0]].count(vs[1]) == 2);
    assert(vs.size() == hs.size());
    assert(ms.size() == hs.size());
    for (unsigned int i = 0; i < vs.size(); i++) {
        assert(G.contains(vs[i]));
        ms[i].check_in(*hs[i]);
        assert(ms[i].size() == 2);
        assert(ms[i][0].size() == 3);
        assert(ms[i][1].size() == 3);
    }
    for (unsigned int i = 0; i < vs.size() - 1; i++) {
        assert(G[vs[i]].contains(vs[i + 1]));
        assert(G[vs[i]].degree() == 3);
    }
    assert(G[vs[vs.size() - 1]].contains(vs[0]));
    assert(G[vs[vs.size() - 1]].degree() == 3);
#endif
    for (unsigned int i = 0; i < vs.size() - 1; i++)
        deleteE(G, Location(vs[i], vs[i + 1]), f);
    deleteE(G, Location(vs[vs.size() - 1], vs[0]), f);

    auto ms2 = ms;
    for (unsigned int i = 0; i < vs.size(); i++)
        for (unsigned int j = 0; j < 2; j++)
            for (unsigned int k = 0; k < 3; k++)
                ms2[i][j][k]=ms2[i][j][k]+ offset + i * offset2;

    add_disjoint(G, *hs[0], offset, f);
    for (unsigned int i = 1; i < vs.size(); i++) {
        add_disjoint(G, *hs[i], offset + (int)i * offset2, f);
        identify_vertices(G, ms2[i - 1][1][0], ms2[i][0][0], f);
        suppress_vertex(G, ms2[i - 1][1][0], f);
        identify_vertices(G, ms2[i - 1][1][2], ms2[i][0][2], f);
        suppress_vertex(G, ms2[i - 1][1][2], f);
        identify_vertices(G, vs[i], ms2[i][0][1], f);
        identify_vertices(G, vs[i], ms2[i - 1][1][1], f);
    }
    identify_vertices(G, ms2[vs.size() - 1][1][0], ms2[0][0][0], f);
    suppress_vertex(G, ms2[vs.size() - 1][1][0], f);
    identify_vertices(G, ms2[vs.size() - 1][1][2], ms2[0][0][2], f);
    suppress_vertex(G, ms2[vs.size() - 1][1][2], f);
    identify_vertices(G, vs[0], ms2[0][0][1], f);
    identify_vertices(G, vs[0], ms2[vs.size() - 1][1][1], f);

    return G;
}

} // namespace ba_graph

#endif
