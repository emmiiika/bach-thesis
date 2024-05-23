#ifndef BA_GRAPH_INVARIANTS_CONNECTIVITY_HPP
#define BA_GRAPH_INVARIANTS_CONNECTIVITY_HPP

#include "distance.hpp"
#include "../operations/undo.hpp"
#include "../operations/copies.hpp"

namespace ba_graph
{

inline bool is_connected(const Graph &G)
{
    if (G.order() == 0)
        return true;
    Number v = G.find(RP::all())->n();
    auto d = distances(G, v);
    for (auto &r : G) {
        if (d[r.n()] == -1)
            return false;
    }
    return true;
}

inline bool has_cut_vertex(const Graph &G)
{
    Factory f;
    auto H = copy_other_factory(G, f);
    for (auto &rv : H) {
        Number v = rv.n();
        auto cleared = clear_edges(H, {v}, f);
        auto cs = components(H);
        if (cs.size() > 2)
            return true;
        restore_edges(H, cleared, f);
    }
    return false;
}

inline std::vector<Number> cut_vertices(const Graph &G)
{
    std::vector<Number> result;
    Factory f;
    auto H = copy_other_factory(G, f);
    for (auto &rv : H) {
        Number v = rv.n();
        auto cleared = clear_edges(H, {v}, f);
        auto cs = components(H);
        if (cs.size() > 2)
            result.push_back(v);
        restore_edges(H, cleared, f);
    }
    return result;
}

inline bool has_cut_edge(const Graph &G)
{
    Factory f;
    auto H = copy_other_factory(G, f);
    for (auto &loc : H.list(RP::all(), IP::primary(), IT::l())) {
        auto cleared = clear_edges(H, {loc}, f);
        auto cs = components(H);
        if (cs.size() > 1)
            return true;
        restore_edges(H, cleared, f);
    }
    return false;
}

inline std::vector<Location> cut_edges(const Graph &G)
{
    std::vector<Location> result;
    Factory f;
    auto H = copy_other_factory(G, f);
    for (auto &loc : H.list(RP::all(), IP::primary(), IT::l())) {
        auto cleared = clear_edges(H, {loc}, f);
        auto cs = components(H);
        if (cs.size() > 1)
            result.push_back(loc);
        restore_edges(H, cleared, f);
    }
    return result;
}

}  // namespace end

#endif
