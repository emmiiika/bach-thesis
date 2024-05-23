#ifndef BA_GRAPH_INVARIANTS_COLOURING_HPP
#define BA_GRAPH_INVARIANTS_COLOURING_HPP

#include <iostream>
#include <set>

#include "degree.hpp"
#include "invariants/girth.hpp"
#include "../operations/simplification.hpp"
#include "../operations/line_graph.hpp"
#include "../operations/basic.hpp"

namespace ba_graph
{

const int uncolourable = -1;
const int noColour = -1;

namespace internal
{

// encapsulates data related to a backtracking enumerating all vertex colourings
template <typename P = void>
class ColouringEnumerator
{
  public:
    const Graph &G;

    static const int noLimit = -1;

    ColouringEnumerator(const Graph &G, int k) : G(G), k(k) {
#ifdef BA_GRAPH_DEBUG
        assert(k >= 0);
#endif
    }

    // returns true iff at least one colouring was found
    // use maxColourings == 1 if you only want to know if a colouring exists
    bool enumerateColourings(
            bool (*callback)(std::map<Vertex, int>, P *param), P *callbackParam,
            std::map<Vertex, int> precolouring = std::map<Vertex, int>(),
            int maxColourings = noLimit
    )
    {
#ifdef BA_GRAPH_DEBUG
        assert(maxColourings == noLimit || maxColourings >= 0);
#endif
        if (G.order() == 0) {
            if (callback != nullptr) {
                std::map<Vertex, int> empty_colouring;
                callback(empty_colouring, callbackParam);
            }
            return true;
        }
        this->callback = callback;
        this->callbackParam = callbackParam;
        this->maxColourings = maxColourings;

        colouring = std::vector<int>(max_number(G).to_int() + 1, noColour);
        if (!computeOrdering(precolouring))
            return false;
        foundColouringsCount = 0;
        doContinue = true;

        return colour(precolouredCount); // we only colour vertices not given a colour
    }

  protected:
    int k; // number of colours

    bool (*callback)(std::map<Vertex, int>, P *param);
    P *callbackParam;
    int maxColourings;

    int precolouredCount;
    std::vector<Number> ordering;
    std::vector<bool> surrounded;
    std::vector<int> colouring;
    int foundColouringsCount;
    bool doContinue;

    // if precolouring is consistent, constructs an ordering staring with precoloured vertices
    // otherwise just returns false
    bool computeOrdering(std::map<Vertex, int> precolouring)
    {
        ordering.clear();
        ordering.reserve(G.order());

        surrounded.clear();
        surrounded.resize(max_number(G).to_int() + 1);

        std::map<Number, int> nColouredNeighbours;
        for (auto &r : G) {
            nColouredNeighbours[r.n()] = 0;
            surrounded[r.n().to_int()] = false;
        }

        std::set<Number> unused;
        for (auto &r : G) {
            if (precolouring.count(r.v()) == 0 || precolouring[r.v()] == noColour) {
                unused.insert(r.n());
            } else {
#ifdef BA_GRAPH_DEBUG
                assert(precolouring[r.v()] < k);
                assert(precolouring[r.v()] >= 0);
#endif
                colouring[r.n().to_int()] = precolouring[r.v()];
                ordering.push_back(r.n());

                for (auto &ii : G[r.n()]) {
                    if ((precolouring.count(ii.v2()) > 0) && (precolouring[ii.v2()] == precolouring[r.v()]))
                        return false;
                    nColouredNeighbours[ii.n2()]++;
                }
            }
        }
        precolouredCount = G.order() - unused.size();

        // next vertex is one that has maximum number of neighbours among those already coloured
        while (unused.size() > 0) {
            auto fewerColouredNeighbours =  [&](auto v1, auto v2) {
                return nColouredNeighbours[v1] < nColouredNeighbours[v2];
            };
            Number v = *std::max_element(unused.begin(), unused.end(), fewerColouredNeighbours);
            if (G[v].degree() == nColouredNeighbours[v]) // all neighbours of v are coloured
                surrounded[v.to_int()] = true;
            ordering.push_back(v);
            unused.erase(v);
            for (auto u : G[v].neighbours())
                ++nColouredNeighbours[u];
        }
        return true;
    }

    bool colour(int next)
    {
        if (!doContinue)
            return false;

        if (next == G.order()) {
            ++foundColouringsCount;
            if (callback != nullptr) {
                std::map<Vertex, int> outputColouring;
                for (auto &r : G)
                    outputColouring[r.v()] = colouring[r.n().to_int()];
                doContinue = callback(outputColouring, callbackParam);
            }
            return true;
        }

        Number v = ordering[next];
        bool found = false;
        if ((maxColourings == 1) && surrounded[v.to_int()]) {
            // we are only interested in the existence of a colouring
            std::set<int> s;
            for (auto &inc : G[v])
                s.insert(colouring[inc.n2().to_int()]);
            return ((int)s.size() < k) ? colour(next + 1) : false;
        } else {
            for (int c = 0; c < k; c++) {
                bool used = false;
                for (auto &inc : G[v]) {
                    if (colouring[inc.n2().to_int()] == c) {
                        used = true;
                        break;
                    }
                }
                if (!used) {
                    colouring[v.to_int()] = c;
                    if ((maxColourings == noLimit) || (foundColouringsCount < maxColourings)) {
                        bool r = colour(next + 1);
                        found = found || r;
                    }
                }
            }
        }
        colouring[v.to_int()] = noColour;
        return found;
    }
};

} // namespace internal

template <typename P>
bool enumerate_colourings_basic(
        const Graph &G, int k,
        bool (*callback)(std::map<Vertex, int>, P *param), P *callbackParam,
        std::map<Vertex, int> precolouring = std::map<Vertex, int>()
)
{
    internal::ColouringEnumerator<P> ce(G, k);
    return ce.enumerateColourings(callback, callbackParam, precolouring);
}

namespace internal
{

template <typename P>
struct EnumerateEdgeColouringsCallbackData
{
    const Graph *G;
    bool (*originalCallback)(std::map<Edge, int>, P *param);
    P *originalCallbackParam;
    std::map<Edge, Vertex> *edgeToVertex;
};

template <typename P>
bool enumerateEdgeColouringsCallback(
    std::map<Vertex, int> lgColouring,
    EnumerateEdgeColouringsCallbackData<P> *data
)
{
    std::map<Edge, int> edgeColouring;
    for (Edge e : data->G->list(RP::all(), IP::primary(), IT::e()))
        edgeColouring[e] = lgColouring[(*(data->edgeToVertex))[e]];
    return data->originalCallback(edgeColouring, data->originalCallbackParam);
}

} // namespace internal

template <typename P>
bool enumerate_edge_colourings_basic(
    const Graph &G, int k,
    bool (*callback)(std::map<Edge, int>, P *param), P *callbackParam,
    std::map<Edge, int> precolouring
)
{
    Factory f;
    auto [H, edgeToVertexNumber] = line_graph_with_map(G, f);
    std::map<Vertex, int> lgPrecolouring;
    for (Edge e : G.list(RP::all(), IP::primary(), IT::e())) {
        if (precolouring.count(e) > 0)
            lgPrecolouring[H[edgeToVertexNumber[e]].v()] = precolouring[e];
    }
   std::map<Edge, Vertex> edgeToVertex;
    for (Edge e : G.list(RP::all(), IP::primary(), IT::e()))
        edgeToVertex[e] = H[edgeToVertexNumber[e]].v();
    struct internal::EnumerateEdgeColouringsCallbackData<P> data;
    data.G = &G;
    data.originalCallback = callback;
    data.originalCallbackParam = callbackParam;
    data.edgeToVertex = &edgeToVertex;
    return enumerate_colourings_basic(H, k, internal::enumerateEdgeColouringsCallback, &data, lgPrecolouring);
}

inline bool is_colourable_basic(
    const Graph &G, int k,
    std::map<Vertex, int> precolouring
)
{
    internal::ColouringEnumerator<> ce(G, k);
    return ce.enumerateColourings(nullptr, nullptr, precolouring, 1);
}

inline bool is_colourable_basic(const Graph &G, int k, bool precolour = true)
{
    std::map<Vertex, int> precolouring;

    if (precolour) {
        if (G.size() >= 1) {
            if (k == 0)
                return false;
            Edge e = G.find(RP::all(), IP::all()).second->e();
            precolouring[e.v1()] = 0;
            if (k >= 2)
                precolouring[e.v2()] = 1;
            else
                return false;
        }
    }
    internal::ColouringEnumerator<> ce(G, k);
    return ce.enumerateColourings(nullptr, nullptr, precolouring, 1);
}

inline bool is_edge_colourable_basic(
    const Graph &G, int k,
    std::map<Edge, int> precolouring
)
{
    if (has_loop(G))
        return false;
    Factory f;
    auto [H, edgeToVertex] = line_graph_with_map(G, f);
    std::map<Vertex, int> lgPrecolouring;
    for (Edge e : G.list(RP::all(), IP::primary(), IT::e())) {
        if (precolouring.count(e) > 0)
            lgPrecolouring[H[edgeToVertex[e]].v()] = precolouring[e];
    }
    return is_colourable_basic(H, k, lgPrecolouring);
}

inline bool is_edge_colourable_basic(const Graph &G, int k, bool precolour = true)
{
    if (has_loop(G))
        return false;
    Factory f;
    auto [H, edgeToVertex] = line_graph_with_map(G, f);

    std::map<Vertex, int> lgPrecolouring;

    if (precolour) {
        int d = max_deg(G);
        if (k < d)
            return false;
        for (auto &r : G) {
            if (r.contains(r.n())) // there is a loop in G
                return false;
            if (r.degree() == d) {
                int c = 0;
                for (Edge e : r.list(IP::all(), IT::e()))
                    lgPrecolouring[H[edgeToVertex[e]].v()] = c++;
                break;
            }
        }
    }
    return is_colourable_basic(H, k, lgPrecolouring);
}

inline int chromatic_number_basic(const Graph &g)
{
    for (int k = g.order() - 1; k >= 0; k--) {
        if (!is_colourable_basic(g, k))
            return k + 1;
    }
    return 0;
}

inline int chromatic_index_basic(const Graph &G)
{
    if (has_loop(G))
        return uncolourable;
    for (int k = 0; k <= G.size(); ++k)
    {
        if (is_edge_colourable_basic(G, k))
            return k;
    }
    return uncolourable;
}

template <typename P = void>
class BasicColouriser
{
  public:
    bool isColourable(const Graph &G) const
    {
        return is_edge_colourable_basic(G, 3, true);
    }

    bool isColourable(const Graph &G,
                      std::map<Edge, int> precolouring) const
    {
        return is_edge_colourable_basic(G, 3, precolouring);
    }

    bool enumerateColourings(
        const Graph &G,
        bool (*callback)(std::map<Edge, int>, P *param), P *callbackParam,
        std::map<Edge, int> precolouring = std::map<Edge, int>()
    ) const
    {
        return enumerate_edge_colourings_basic(G, 3, callback, callbackParam, precolouring);
    }
};

} // namespace ba_graph

#endif
