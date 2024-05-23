#ifndef BA_GRAPH_OPERATIONS_UNDO_HPP
#define BA_GRAPH_OPERATIONS_UNDO_HPP

#include <unordered_set>
#include <vector>

namespace ba_graph
{

typedef std::tuple<Edge, Rotation *, Rotation *> ClearedEdge;

inline std::vector<ClearedEdge> clear_edges(Graph &G, std::vector<Location> edges, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    for (Location l : edges)
        assert(G.contains(l));
#endif
    std::vector<ClearedEdge> clearedEdges;
    std::unordered_set<Edge> ce;
    for (Location l : edges) {
        auto ii = G[l.n1()].find(l);
        Edge e = ii->e();
        Rotation *r1 = &ii->r1(), *r2 = &ii->r2();
        if (!ii->is_primary())
            std::swap(r1, r2);
        if (std::find(ce.begin(), ce.end(), e) == ce.end()) {
            clearedEdges.push_back(std::tuple(e, r1, r2));
            ce.insert(e);
        }
    }
    for (auto x : clearedEdges)
        deleteE(G, *std::get<1>(x), std::get<0>(x), f);
    return clearedEdges;
}

inline std::vector<ClearedEdge> clear_edges(Graph &G, std::vector<Edge> edges, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
#endif
    std::vector<Location> locations;
    for (auto e : edges)
        locations.push_back(G.find(e.v1(), e).second->l());
    return clear_edges(G, locations, f);
}

inline std::vector<ClearedEdge> clear_edges(Graph &G, std::set<Edge> edges, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
#endif
    std::vector<Location> locations;
    for (auto e : edges)
        locations.push_back(G.find(e.v1(), e).second->l());
    return clear_edges(G, locations, f);
}

inline std::vector<ClearedEdge> clear_edges(Graph &G, std::vector<Number> vertices, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    for (Number v : vertices)
        assert(G.contains(v));
#endif
    std::vector<ClearedEdge> clearedEdges;
    std::unordered_set<Edge> ce;
    for (Number v : vertices) {
        for (auto &neii : G[v].list(IP::all())) {
            if (std::find(ce.begin(), ce.end(), neii->e()) == ce.end()) {
                if (neii->e().v1() == neii->r1().v())
                    clearedEdges.push_back(std::tuple(neii->e(), &neii->r1(), &neii->r2()));
                else
                    clearedEdges.push_back(std::tuple(neii->e(), &neii->r2(), &neii->r1()));
                ce.insert(neii->e());
            }
        }
    }
    for (auto x : clearedEdges)
        deleteE(G, *std::get<1>(x), std::get<0>(x), f);
    return clearedEdges;
}

inline void restore_edges(Graph &G, std::vector<ClearedEdge> clearedEdges, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
#endif
    for (auto x : clearedEdges)
        addE(G, *std::get<1>(x), *std::get<2>(x), std::get<0>(x), f);
}

} // namespace ba_graph

#endif
