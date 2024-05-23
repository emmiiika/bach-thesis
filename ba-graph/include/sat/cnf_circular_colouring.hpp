#ifndef BA_GRAPH_SAT_CNF_CIRCULAR_COLOURING_HPP
#define BA_GRAPH_SAT_CNF_CIRCULAR_COLOURING_HPP

#include "cnf.hpp"
#include "../invariants/degree.hpp"
#include "../operations/line_graph.hpp"
#include "../operations/simplification.hpp"

#include <cassert>
#include <map>
#include <vector>

namespace ba_graph
{

// distance between colours along the circle of length p
inline int circular_distance(int p, int c1, int c2) {
    int cc1 = std::min(c1, c2), cc2 = std::max(c1, c2);
    return std::min(cc2 - cc1, cc1 + p - cc2);
}

// for each of the vertices in precolouring that has colour from {0, 1, ..., k-1}
// it gets that colour
inline CNF cnf_circular_vertex_colouring(const Graph &G, int p, int q, std::map<Vertex, int> precolouring)
{
    if (p == 0 && G.order() == 0) return cnf_satisfiable;
    if (p == 0 && G.order() > 0) return cnf_unsatisfiable;

    std::map<Number, std::vector<int>> vars;
    int nextVar = 0;
    for (auto &r : G) {
        std::vector<int> v(p);
        for (int i = 0; i < p; ++i)
            v[i] = nextVar++;
        vars[r.n()] = v;
    }

    std::vector<std::vector<Lit>> result;
    result.reserve(p * p * G.order());
    for (auto &r : G) {
        const int noColour = -1;
        int preC = (precolouring.count(r.v()) > 0) ? precolouring[r.v()] : noColour;
#ifdef BA_GRAPH_DEBUG
        if (preC != noColour) {
            assert(preC < p);
            assert(preC >= 0);
        }
#endif

        std::vector<Lit> clause1;
        clause1.reserve(p);
        // at least one colour for the vertex
        for (int c = 0; c < p; ++c) {
            // if precoloured by c, this vertex gets only colour c
            if ((preC == noColour) || (preC == c))
                clause1.push_back(Lit(vars[r.n()][c], false));
        }
        result.push_back(clause1);

        if (preC == noColour) {
            // at most one colour for the vertex
            for (int c1 = 0; c1 < p; ++c1) {
                for (int c2 = c1 + 1; c2 < p; ++c2) {
                    result.push_back({
                            Lit(vars[r.n()][c1], true),
                            Lit(vars[r.n()][c2], true)
                    });
                }
            }
        }

        // no adjacent vertices have the same colour
        for (auto &i : r) {
            if (i.n2() <= r.n())
                continue;
            int preC2 = (precolouring.count(i.v2()) > 0) ? 
                    precolouring[i.v2()] : noColour;
            int preC1 = preC;
            for (int c1 = 0; c1 < p; ++c1) {
                for (int c2 = 0; c2 < p; ++c2) {
                    if (circular_distance(p, c1, c2) >= q) continue;
                    Lit lit1 = Lit(vars[r.n()][c1], true);
                    Lit lit2 = Lit(vars[i.n2()][c2], true);
                    std::vector<Lit> clause;
                    if ((preC1 == noColour) && (preC2 == noColour)) {
                        clause = {lit1, lit2};
                    } else if ((preC1 == noColour) && (preC2 != noColour)) {
                        if (c2 == preC2) clause = {lit1};
                    } else if ((preC1 != noColour) && (preC2 == noColour)) {
                        if (c1 == preC1) clause = {lit2};
                    } else {
                        if (circular_distance(p, preC1, preC2) < q)
                            return cnf_unsatisfiable;
                    }
                    if (clause.size() > 0)
                        result.push_back(clause);
                }
            }
        }
    }
    return std::pair(G.order() * p, result);
}

// if precolour = true, precolour a vertex by 0
// perhaps could be improved by finding a bigger clique
inline CNF cnf_circular_vertex_colouring(const Graph &G, int p, int q, bool precolour = false)
{
    std::map<Vertex, int> precolouring;
    if (!precolour)
        return cnf_circular_vertex_colouring(G, p, q, precolouring);
    precolouring[G.find(RP::all())->v()] = 0; // TODO can be improved if only half of the colours were tried for a neighbour
    return cnf_circular_vertex_colouring(G, p, q, precolouring);
}

// for each of the edges in precolouring that has colour from {0, 1, ..., k-1}
// it gets that colour
inline CNF cnf_circular_edge_colouring(const Graph &G, int p, int q, std::map<Edge, int> precolouring)
{
    Factory f;
    auto [H, edgeToVertex] = line_graph_with_map(G, f);
    std::map<Vertex, int> lgPrecolouring;
    for (Edge e : G.list(RP::all(), IP::primary(), IT::e())) {
        if (precolouring.count(e) > 0)
            lgPrecolouring[H[edgeToVertex[e]].v()] = precolouring[e];
    }
    return cnf_circular_vertex_colouring(H, p, q, lgPrecolouring);
}

// if precolour = true, precolour the edges from a vertex of maximum degree by colour 0, 1, ...
inline CNF cnf_circular_edge_colouring(const Graph &G, int p, int q, bool precolour = false)
{
    Factory f;
    auto [H, edgeToVertex] = line_graph_with_map(G, f);
    std::map<Vertex, int> precolouring;
    if (!precolour)
        return cnf_circular_vertex_colouring(H, p, q, precolouring);

    int d = max_deg(G);
    if (p < q * d)
        return cnf_unsatisfiable;
    for (auto &r : G) {
        if (r.contains(r.n())) // there is a loop in G
            return cnf_unsatisfiable;
        if (r.degree() == d) {
            int c = 0;
            for (Edge e : r.list(IP::all(), IT::e())) {
                precolouring[H[edgeToVertex[e]].v()] = c;
                c += q;
            }
            break;
        }
    }
    return cnf_circular_vertex_colouring(H, p, q, precolouring);
}

namespace internal
{

inline std::vector<std::pair<int, int>> computeCircularColouringFractions(const Graph &G, int upper_bound)
{
    int ub = upper_bound;
    int lb = ub - 1;
    std::set<std::pair<int, int>> fractions;
    for (int p = 1; p <= G.order(); ++p) {
        for (int q = std::max(1, p/ub); q <= p/lb; ++q) {
            if ((lb < (double)p/q) && ((double)p/q <= ub)) {
                // reduce p/q
                int pp = p, qq = q;
                for (int d = q; d >= 2; --d) {
                    while ((pp % d == 0) && (qq % d == 0)) {
                        pp /= d; qq /= d;
                    }
                }
                fractions.insert(std::pair(pp, qq));
            }
        }
    }
    std::vector<std::pair<int, int>> sortedFractions(fractions.begin(), fractions.end());
    std::sort(sortedFractions.begin(), sortedFractions.end(),
            [](std::pair<int, int> &f1, std::pair<int, int> &f2) {
                return f1.first / (double)f1.second < f2.first / (double)f2.second;
            });
    return sortedFractions;
}

} // namespace internal

} // namespace ba_graph

#endif
