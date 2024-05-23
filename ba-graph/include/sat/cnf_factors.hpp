#ifndef BA_GRAPH_SAT_CNF_FACTORS_HPP
#define BA_GRAPH_SAT_CNF_FACTORS_HPP

#include "cnf.hpp"
#include "../util/math.hpp"

#include <cassert>
#include <map>
#include <set>
#include <vector>

namespace ba_graph
{

inline std::vector<Edge> cnf_kfactor_vars_to_edge(const Graph &G)
{
    return G.list(RP::all(), IP::primary(), IT::e());
}

inline std::vector<Number> vertex_vector_to_number_vector(
    const Graph &G,
    const std::vector<Vertex> &vertexVector)
{
    auto numberVector = std::vector<Number>{};
    for (Vertex v : vertexVector)
    {
        numberVector.push_back(G.find(v)->n());
    }
    return numberVector;
}

inline CNF cnf_kfactor(const Graph &G, int k,
    const std::vector<Number>& possiblyNotCoveredVertices)
{
#ifdef BA_GRAPH_DEBUG
    assert(k >= 1);
    for (auto n : possiblyNotCoveredVertices)
        assert(G.contains(n));
#endif
    std::map<Edge, int> vars;
    int varsCount = 0;
    for (auto ii : G.list(RP::all(), IP::primary()))
        vars[ii->e()] = varsCount++;

    std::vector<Clause> result;
    for (auto &r : G) {
        // at least k edges at a vertex are included
        std::vector<Clause> dnf;
        auto edges = r.list(IP::all(), IT::e());
        auto edgesSet = std::set<Edge>(edges.begin(), edges.end());
        bool shouldPossiblyNotCoverVertex = std::find(possiblyNotCoveredVertices.begin(), possiblyNotCoveredVertices.end(), r.n()) 
            != possiblyNotCoveredVertices.end();
        if (!shouldPossiblyNotCoverVertex)
        {
            auto subsets = all_subsets(edgesSet, k);
            for (auto set : subsets) {
                std::vector<Lit> clause;
                for (Edge e : set)
                    clause.push_back(Lit(vars[e], false));
                dnf.push_back(clause);
            }

            auto cnf = dnf_to_cnf(dnf, varsCount);
            varsCount += dnf.size();
            result.insert(result.end(), cnf.begin(), cnf.end());
        }
    
        // at most k edges at a vertex are included
        auto subsets = all_subsets(edgesSet, k + 1);
        for (auto set : subsets) {
            std::vector<Lit> clause;
            for (Edge e : set)
                clause.push_back(Lit(vars[e], true));
            result.push_back(clause);
        }
    }
    return std::pair(varsCount, result);
}

inline CNF cnf_kfactor(const Graph &G, int k,
    const std::vector<Vertex> &possiblyNotCoveredVertices)
{
    return cnf_kfactor(G, k, vertex_vector_to_number_vector(G, possiblyNotCoveredVertices));
}

inline CNF cnf_kfactor(const Graph &G, int k)
{
    return cnf_kfactor(G, k, std::vector<Number>{});
}

inline std::vector<Edge> cnf_perfect_matching_vars_to_edge(const Graph &G)
{
    return G.list(RP::all(), IP::primary(), IT::e());
}

// TODO pridat moznost predpisat / zakazat hrany, asi v podobe Location aj Edge
inline CNF cnf_perfect_matching(const Graph &G,
    const std::vector<Number>& possiblyNotCoveredVertices)
{
#ifdef BA_GRAPH_DEBUG
    for (auto n : possiblyNotCoveredVertices)
        assert(G.contains(n));
#endif
    std::map<Edge, int> vars;
    int varsCount = 0;
    for (auto ii : G.list(RP::all(), IP::primary()))
        vars[ii->e()] = varsCount++;

    std::vector<std::vector<Lit>> result;
    result.reserve(G.order() + G.size()); // not enough but a good start
    for (auto &r : G) {
        // at least one edge at a vertex is included
        bool shouldPossiblyNotCoverVertex = std::find(possiblyNotCoveredVertices.begin(), possiblyNotCoveredVertices.end(), r.n()) 
            != possiblyNotCoveredVertices.end();
        if (!shouldPossiblyNotCoverVertex)
        {
            std::vector<Lit> clause1;
            for (auto e : r.list(IP::all(), IT::e()))
                clause1.push_back(Lit(vars[e], false));
            result.push_back(clause1);
        }

        // at most one edge at a vertex is included
        auto edges = r.list(IP::all(), IT::e());
        for (std::vector<Edge>::size_type i = 0; i < edges.size(); ++i) {
            for (std::vector<Edge>::size_type j = i + 1; j < edges.size(); ++j) {
                result.push_back( {
                        Lit(vars[edges[i]], true),
                        Lit(vars[edges[j]], true)
                });
            }
        }
    }
    return std::pair(varsCount, result);
}

inline CNF cnf_perfect_matching(const Graph &G,
    const std::vector<Vertex>& possiblyNotCoveredVertices)
{
    return cnf_perfect_matching(G, vertex_vector_to_number_vector(G, possiblyNotCoveredVertices));
}

inline CNF cnf_perfect_matching(const Graph &G)
{
    return cnf_perfect_matching(G, std::vector<Number>{});
}

namespace internal
{

inline std::vector<Edge> transform_to_perfect_matching(std::vector<bool> &solution, const Graph *G)
{
    std::vector<Edge> pm;
    pm.reserve(G->order() / 2);
    auto vars_to_edge = cnf_perfect_matching_vars_to_edge(*G);
    for (std::vector<bool>::size_type i = 0; i < solution.size(); ++i) {
        if (solution[i])
            pm.push_back(vars_to_edge[i]);
    }
    return pm;
}

struct KFactorTransformData
{
    const Graph *G;
    Factory *f;
};

inline Graph transform_to_kfactor(std::vector<bool> &solution, struct KFactorTransformData *data)
{
    Graph F = createG(*data->f);
    for (auto &r : *data->G)
        addV(F, r.v(), r.n(), *data->f);
    auto vars_to_edge = cnf_kfactor_vars_to_edge(*data->G);
    for (std::vector<bool>::size_type i = 0; i < solution.size(); ++i) {
        if (solution[i] && i < vars_to_edge.size())
            addE(F, vars_to_edge[i], *data->f);
    }
    return F;
}

} // namespace internal

} // namespace ba_graph

#endif
