#ifndef BA_GRAPH_SAT_EXEC_FACTORS_HPP
#define BA_GRAPH_SAT_EXEC_FACTORS_HPP

#include "cnf_factors.hpp"
#include "exec_solver.hpp"
#include "solver.hpp"

#include "../invariants/degree.hpp"
#include "../operations/copies.hpp"

namespace ba_graph
{

inline bool has_perfect_matching_sat(const SatSolver &solver, const Graph &G,
    const std::vector<Number> &possiblyNotCoveredVertices)
{
#ifdef BA_GRAPH_DEBUG
    for (auto n : possiblyNotCoveredVertices)
        assert(G.contains(n));
#endif
    auto cnf = cnf_perfect_matching(G, possiblyNotCoveredVertices);
    return satisfiable(solver, cnf);
}

inline bool has_perfect_matching_sat(const SatSolver &solver, const Graph &G,
    const std::vector<Vertex> &possiblyNotCoveredVertices)
{
    return has_perfect_matching_sat(solver, G, vertex_vector_to_number_vector(G, possiblyNotCoveredVertices));
}

inline bool has_perfect_matching_sat(const SatSolver &solver, const Graph &G)
{
    return has_perfect_matching_sat(solver, G, std::vector<Number>{});
}

template <typename P>
bool perfect_matchings_enumerate_sat(const AllSatSolver &solver, const Graph &G,
        bool (*callback)(std::vector<Edge> &perfectMatching, P *param), P *callbackParam,
        const std::vector<Number> &possiblyNotCoveredVertices)
{
    auto cnf = cnf_perfect_matching(G, possiblyNotCoveredVertices);
    return all_sat_solutions(solver, cnf, callback, callbackParam,
        internal::transform_to_perfect_matching, &G);
}

template <typename P>
bool perfect_matchings_enumerate_sat(const AllSatSolver &solver, const Graph &G,
        bool (*callback)(std::vector<Edge> &perfectMatching, P *param), P *callbackParam)
{
    return perfect_matchings_enumerate_sat(solver, G, callback, callbackParam, std::vector<Number>{});
}

inline std::vector<std::vector<Edge>>
perfect_matchings_list_sat(const AllSatSolver &solver, const Graph &G,
    const std::vector<Number> &possiblyNotCoveredVertices)
{
#ifdef BA_GRAPH_DEBUG
    for (auto n : possiblyNotCoveredVertices)
        assert(G.contains(n));
#endif
    std::vector<std::vector<Edge>> list;
    perfect_matchings_enumerate_sat(solver, G, enumerateListCallback, &list, possiblyNotCoveredVertices);
    return list;
}

inline std::vector<std::vector<Edge>>
perfect_matchings_list_sat(const AllSatSolver &solver, const Graph &G,
    const std::vector<Vertex> &possiblyNotCoveredVertices)
{
    return perfect_matchings_list_sat(solver, G, vertex_vector_to_number_vector(G, possiblyNotCoveredVertices));
}

inline std::vector<std::vector<Edge>>
perfect_matchings_list_sat(const AllSatSolver &solver, const Graph &G)
{
    return perfect_matchings_list_sat(solver, G, std::vector<Number>{});
}

inline bool has_kfactor_sat(const SatSolver &solver, const Graph &G, int k,
    const std::vector<Number> &possiblyNotCoveredVertices)
{
#ifdef BA_GRAPH_DEBUG
    for (auto n : possiblyNotCoveredVertices)
        assert(G.contains(n));
#endif
    auto cnf = cnf_kfactor(G, k, possiblyNotCoveredVertices);
    return satisfiable(solver, cnf);
}

inline bool has_kfactor_sat(const SatSolver &solver, const Graph &G, int k,
    const std::vector<Vertex> &possiblyNotCoveredVertices)
{
    return has_kfactor_sat(solver, G, k, vertex_vector_to_number_vector(G, possiblyNotCoveredVertices));
}

inline bool has_kfactor_sat(const SatSolver &solver, const Graph &G, int k)
{
    return has_kfactor_sat(solver, G, k, std::vector<Number>{});
}

template <typename P>
bool kfactors_enumerate_sat(const AllSatSolver &solver, const Graph &G, int k, Factory &f,
        bool (*callback)(Graph &kfactor, P *param), P *callbackParam,
        const std::vector<Number> &possiblyNotCoveredVertices)
{
    auto cnf = cnf_kfactor(G, k, possiblyNotCoveredVertices);
    struct internal::KFactorTransformData data;
    data.G = &G;
    data.f = &f;
    return all_sat_solutions(solver, cnf, callback, callbackParam,
        internal::transform_to_kfactor, &data);
}

template <typename P>
bool kfactors_enumerate_sat(const AllSatSolver &solver, const Graph &G, int k, Factory &f,
        bool (*callback)(Graph &kfactor, P *param), P *callbackParam)
{
    return kfactors_enumerate_sat(solver, G, k, f, callback, callbackParam, std::vector<Number>{});
}

inline std::vector<Graph>
kfactors_list_sat(const AllSatSolver &solver, const Graph &G, int k, Factory &f,
    const std::vector<Number> &possiblyNotCoveredVertices)
{
#ifdef BA_GRAPH_DEBUG
    for (auto n : possiblyNotCoveredVertices)
        assert(G.contains(n));
#endif
    std::vector<Graph> list;
    kfactors_enumerate_sat(solver, G, k, f, enumerateListCallback, &list, possiblyNotCoveredVertices);
    return list;
}

inline std::vector<Graph>
kfactors_list_sat(const AllSatSolver &solver, const Graph &G, int k, Factory &f,
    const std::vector<Vertex> &possiblyNotCoveredVertices)
{
    return kfactors_list_sat(solver, G, k, f, vertex_vector_to_number_vector(G, possiblyNotCoveredVertices));
}

inline std::vector<Graph>
kfactors_list_sat(const AllSatSolver &solver, const Graph &G, int k, Factory &f)
{
    return kfactors_list_sat(solver, G, k, f, std::vector<Number>{});
}

/*
 * ====================== 2-factors for cubic graphs via allsat =======================
 */

namespace internal
{

inline Graph transform_to_2factor_cubic(std::vector<bool> &solution, struct KFactorTransformData *data)
{
    Graph F(copy_identical(*data->G, *data->f));
    auto pmEdges = transform_to_perfect_matching(solution, data->G);
    for (auto e : pmEdges)
        deleteE(F, e, *data->f);
    return F;
}


} // namespace internal

template <typename P>
bool two_factors_enumerate_allsat_cubic(const AllSatSolver &solver, const Graph &G, Factory &f,
        bool (*callback)(Graph &F, P *param), P *callbackParam,
        const std::vector<Number> &possiblyNotCoveredVertices)
{
#ifdef BA_GRAPH_DEBUG
    assert(max_deg(G) == 3);
    assert(min_deg(G) == 3);
    for (auto n : possiblyNotCoveredVertices)
        assert(G.contains(n));
#endif
    auto cnf = cnf_perfect_matching(G, possiblyNotCoveredVertices);
    struct internal::KFactorTransformData data;
    data.G = &G;
    data.f = &f;
    return all_sat_solutions(solver, cnf, callback, callbackParam,
            internal::transform_to_2factor_cubic, &data);
}

template <typename P>
bool two_factors_enumerate_allsat_cubic(const AllSatSolver &solver, const Graph &G, Factory &f,
        bool (*callback)(Graph &F, P *param), P *callbackParam)
{
    return two_factors_enumerate_allsat_cubic(solver, G, f, callback, callbackParam, std::vector<Number>{});
}

inline std::vector<Graph> two_factors_list_allsat_cubic(const AllSatSolver &solver, const Graph &G, Factory &f,
    const std::vector<Number> &possiblyNotCoveredVertices)
{
#ifdef BA_GRAPH_DEBUG
    assert(max_deg(G) == 3);
    assert(min_deg(G) == 3);
    for (auto n : possiblyNotCoveredVertices)
        assert(G.contains(n));
#endif
    std::vector<Graph> list;
    two_factors_enumerate_allsat_cubic(solver, G, f, enumerateListCallback, &list, possiblyNotCoveredVertices);
    return list;
}

inline std::vector<Graph> two_factors_list_allsat_cubic(const AllSatSolver &solver, const Graph &G, Factory &f)
{
    return two_factors_list_allsat_cubic(solver, G, f, std::vector<Number>{});
}

}  // namespace end

#endif

