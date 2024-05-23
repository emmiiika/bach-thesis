#ifndef BA_GRAPH_SNARKS_COLOURING_HPP
#define BA_GRAPH_SNARKS_COLOURING_HPP

#include "../invariants/degree.hpp"
#include "../invariants/distance.hpp"
#include "../invariants/colouring.hpp"
#include "../operations/copies.hpp"
#include "../operations/undo.hpp"

namespace ba_graph {

/* ==============================================================
 * Concepts defining 3-edge-colourisers
 * ==============================================================
 */
template <typename C>
concept bool Colouriser = requires(C c, const Graph &G)
{
    {c.isColourable(G)} -> bool;
};

template <typename C, typename P>
concept bool EnumerativeColouriser = Colouriser<C> &&
        requires(C c, const Graph &G,
                 bool (*callback)(std::map<Edge, int>, P *param), P *callbackParam)
{
    {c.enumerateColourings(callback, callbackParam)} -> bool;
};

template <typename C>
concept bool PrecolourColouriser = Colouriser<C> &&
        requires(C c, const Graph &G, const std::map<Edge, int> &precolouring)
{
    {c.isColourable(G, precolouring)} -> bool;
};

template <typename C, typename P>
concept bool EnumerativePrecolourColouriser = PrecolourColouriser<C> &&
        requires(C c, const Graph &G,
                 bool (*callback)(std::map<Edge, int>, P *param), P *callbackParam,
                 const std::map<Edge, int> &precolouring)
{
    {c.enumerateColourings(callback, callbackParam)} -> bool;
    {c.enumerateColourings(callback, callbackParam, precolouring)} -> bool;
};


/* ==============================================================
 * Algorithms depending on colourisers
 * ==============================================================
 */

typedef BasicColouriser<void> DefaultColouriser;


template <Colouriser C>
inline bool is_colourable(const Graph &G, C &colouriser)
{
    return colouriser.isColourable(G);
}

template <Colouriser C = DefaultColouriser>
inline bool is_colourable(const Graph &G)
{
    C c;
    return is_colourable(G, c);
}


// assumes G is uncolourable
template<Colouriser C>
bool is_critical(const Graph &G, C &colouriser)
{
    Factory f;
    Graph H(copy_other_factory(G, f));
    std::vector<std::pair<Number, Number>> vertexPairs;
    for (auto &ii : H.list(RP::all(), IP::primary()))
        vertexPairs.push_back(std::pair(ii->n1(), ii->n2()));
    for (auto [n1, n2]: vertexPairs) {
        auto clearedEdges = clear_edges(H, std::vector<Number>({n1, n2}), f);
        if (!colouriser.isColourable(H))
            return false;
        restore_edges(H, clearedEdges, f);
    }
    return true;
}

template <Colouriser C = DefaultColouriser>
inline bool is_critical(const Graph &G)
{
    C c;
    return is_critical(G, c);
}


// assumes G is uncolourable
template<Colouriser C>
bool is_cocritical(const Graph &G, C &colouriser)
{
    Factory f;
    Graph H(copy_other_factory(G, f));
    for (auto &r1 : G) {
        for (auto &r2 : G) {
            if (r2.n() <= r1.n()) continue;
            if (G.contains(r1.n(), r2.n())) continue;
            auto clearedEdges = clear_edges(H, std::vector({r1.n(), r2.n()}), f);
            if (!colouriser.isColourable(H))
                return false;
            restore_edges(H, clearedEdges, f);
        }
    }
    return true;
}

template <Colouriser C = DefaultColouriser>
inline bool is_cocritical(const Graph &G)
{
    C c;
    return is_cocritical(G, c);
}


// assumes G is uncolourable
template<Colouriser C>
inline bool is_irreducible(const Graph &G, C colouriser)
{
    return is_critical(G, colouriser) && is_cocritical(G, colouriser);
}

template <Colouriser C = DefaultColouriser>
inline bool is_irreducible(const Graph &G)
{
    C c;
    return is_irreducible(G, c);
}

/* FIXME does not work
// assumes G is uncolourable
// a strong snark: any edge is suppressible
// (by removing it and suppressing its endvertices we get an uncolourable graph)
template<Colouriser C>
bool is_strong(const Graph &G, C colouriser)
{
    Factory f;
    Graph H(copy_other_factory(G, f));
    std::vector<std::pair<Number, Number>> vertexPairs;
    for (auto &ii : H.list(RP::all(), IP::primary()))
        vertexPairs.push_back(std::pair(ii->n1(), ii->n2()));
    for (auto [n1, n2]: vertexPairs) {
        // TODO co presne spravit, aby som vedel potom restore?? FIXME
        deleteE(H, Location(n1, n2), f);
        suppress_vertex(H, n1);
        suppress_vertex(H, n2);
        if (colouriser.isColourable(H))
            return false;
    }
    return true;
}

template <Colouriser C = DefaultColouriser>
inline bool is_strong(const Graph &G)
{
    C c;
    return is_strong(G, c);
}
*/

template<Colouriser C>
std::vector<std::pair<Number, Number>>
removable_pairs_vertices(const Graph &G, C colouriser)
{
    if (colouriser.isColourable(G))
        return {};
    std::vector<std::pair<Number, Number>> result;
    Factory f;
    Graph H(copy_other_factory(G, f));
    for (auto &r1 : H) {
        for (auto &r2 : H) {
            if (r2.n() <= r1.n()) continue;
            auto clearedEdges = clear_edges(H, std::vector({r1.n(), r2.n()}), f);
            if (!colouriser.isColourable(H))
                result.push_back(std::pair(r1.n(), r2.n()));
            restore_edges(H, clearedEdges, f);
        }
    }
    return result;
}

template <Colouriser C = DefaultColouriser>
inline std::vector<std::pair<Number, Number>> removable_pairs_vertices(const Graph &G)
{
    C c;
    return removable_pairs_vertices(G, c);
}


template<Colouriser C>
std::vector<std::pair<Number, Number>>
removable_pairs_adjacent_vertices(const Graph &G, C colouriser)
{
    if (colouriser.isColourable(G))
        return {};
    std::vector<std::pair<Number, Number>> result;
    Factory f;
    Graph H(copy_other_factory(G, f));
    std::vector<std::pair<Number, Number>> vertexPairs;
    for (auto &ii : H.list(RP::all(), IP::primary()))
        vertexPairs.push_back(std::pair(ii->n1(), ii->n2()));
    for (auto [n1, n2]: vertexPairs) {
        auto clearedEdges = clear_edges(H, std::vector({n1, n2}), f);
        if (!colouriser.isColourable(H))
            result.push_back(std::pair(n1, n2));
        restore_edges(H, clearedEdges, f);
    }
    return result;
}

template <Colouriser C = DefaultColouriser>
inline std::vector<std::pair<Number, Number>>
removable_pairs_adjacent_vertices(const Graph &G)
{
    C c;
    return removable_pairs_adjacent_vertices(G, c);
}


// assumes G is uncolourable
template<Colouriser C>
std::vector<std::pair<Number, Number>>
nonremovable_pairs_vertices(const Graph &G, C colouriser)
{
    std::vector<std::pair<Number, Number>> result;
    Factory f;
    Graph H(copy_other_factory(G, f));
    for (auto &r1 : H) {
        for (auto &r2 : H) {
            if (r2.n() <= r1.n()) continue;
            auto clearedEdges = clear_edges(H, std::vector({r1.n(), r2.n()}), f);
            if (colouriser.isColourable(H))
                result.push_back(std::pair(r1.n(), r2.n()));
            restore_edges(H, clearedEdges, f);
        }
    }
    return result;
}

template <Colouriser C = DefaultColouriser>
inline std::vector<std::pair<Number, Number>>
nonremovable_pairs_vertices(const Graph &G)
{
    C c;
    return nonremovable_pairs_vertices(G, c);
}


// assumes G is uncolourable
template<Colouriser C>
std::vector<std::pair<Number, Number>>
nonremovable_pairs_adjacent_vertices(const Graph &G, C colouriser)
{
    if (colouriser.isColourable(G))
        return {};
    std::vector<std::pair<Number, Number>> result;
    Factory f;
    Graph H(copy_other_factory(G, f));
    std::vector<std::pair<Number, Number>> vertexPairs;
    for (auto &ii : H.list(RP::all(), IP::primary()))
        vertexPairs.push_back(std::pair(ii->n1(), ii->n2()));
    for (auto [n1, n2]: vertexPairs) {
        auto clearedEdges = clear_edges(H, std::vector({n1, n2}), f);
        if (colouriser.isColourable(H))
            result.push_back(std::pair(n1, n2));
        restore_edges(H, clearedEdges, f);
    }
    return result;
}

template <Colouriser C = DefaultColouriser>
inline std::vector<std::pair<Number, Number>>
nonremovable_pairs_adjacent_vertices(const Graph &G)
{
    C c;
    return nonremovable_pairs_adjacent_vertices(G, c);
}


template<Colouriser C = DefaultColouriser>
std::vector<std::pair<Location, Location>>
removable_pairs_edges(const Graph &G, C colouriser, int minDistance=0)
{
    std::vector<std::pair<Location, Location>> result;
    Factory f;
    auto [H, m] = copy_other_factory<EdgeMapper>(G, f);
    auto edgeList = G.list(RP::all(), IP::primary());
    for (std::vector<IncidenceIterator>::size_type i = 0; i < edgeList.size(); ++i) {
        for (std::vector<IncidenceIterator>::size_type j = i + 1; j < edgeList.size(); ++j) {
            Location l1 = edgeList[i]->l(), l2 = edgeList[j]->l();
            if (distance(G, l1, l2) < minDistance)
                continue;
            std::vector<Location> toRemove = {l1, l2};
            auto clearedEdges = clear_edges(H, toRemove, f);
            if (!colouriser.isColourable(H))
                result.push_back(std::pair(l1, l2));
            restore_edges(H, clearedEdges, f);
        }
    }
    return result;
}

template <Colouriser C = DefaultColouriser>
inline std::vector<std::pair<Location, Location>>
removable_pairs_edges(const Graph &G, int minDistance=0)
{
    C c;
    return removable_pairs_edges(G, c, minDistance);
}

// assumes G is uncolourable
template<Colouriser C>
std::vector<std::pair<Location, Location>>
nonremovable_pairs_edges(const Graph &G, C colouriser)
{
    std::vector<std::pair<Location, Location>> result;
    Factory f;
    auto [H, m] = copy_other_factory<EdgeMapper>(G, f);
    auto edgeList = G.list(RP::all(), IP::primary());
    for (std::vector<IncidenceIterator>::size_type i = 0; i < edgeList.size(); ++i) {
        for (std::vector<IncidenceIterator>::size_type j = i + 1; j < edgeList.size(); ++j) {
            Edge e1 = m.get(edgeList[i]->e()), e2 = m.get(edgeList[j]->e());
            std::vector<Location> toRemove = { H.find(e1.v1(), e1).second->l(), H.find(e2.v1(), e2).second->l() };
            auto clearedEdges = clear_edges(H, toRemove, f);
            if (colouriser.isColourable(H))
                result.push_back(std::pair(edgeList[i]->l(), edgeList[j]->l()));
            restore_edges(H, clearedEdges, f);
        }
    }
    return result;
}

template <Colouriser C = DefaultColouriser>
inline std::vector<std::pair<Location, Location>>
nonremovable_pairs_edges(const Graph &G)
{
    C c;
    return nonremovable_pairs_edges(G, c);
}

} // namespace ba_graph

#endif
