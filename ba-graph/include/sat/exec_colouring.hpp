#ifndef BA_GRAPH_SAT_EXEC_COLOURING_HPP
#define BA_GRAPH_SAT_EXEC_COLOURING_HPP

#include "cnf_colouring.hpp"
#include "cnf_circular_colouring.hpp"
#include "invariants/girth.hpp"
#include "exec_solver.hpp"
#include "solver.hpp"

namespace ba_graph {

/*
 * ==================== regular vertex- and edge-colourings =========================
 */

// determines if G has a k-colouring
inline bool is_colourable_sat(const SatSolver &solver, const Graph &G, int k, std::map<Vertex, int> precolouring)
{
    auto cnf = cnf_vertex_colouring(G, k, precolouring);
    return satisfiable(solver, cnf);
}

// determines if G has a k-colouring
inline bool is_colourable_sat(const SatSolver &solver, const Graph &G, int k)
{
    auto cnf = cnf_vertex_colouring(G, k, true);
    return satisfiable(solver, cnf);
}

// determines if G has a k-edge-colouring
inline bool is_edge_colourable_sat(const SatSolver &solver, const Graph &G, int k, std::map<Edge, int> precolouring)
{
    auto cnf = cnf_edge_colouring(G, k, precolouring);
    return satisfiable(solver, cnf);
}

// determines if G has a k-edge-colouring
inline bool is_edge_colourable_sat(const SatSolver &solver, const Graph &G, int k)
{
    auto cnf = cnf_edge_colouring(G, k, true);
    return satisfiable(solver, cnf);
}

inline int chromatic_number_sat(const SatSolver &solver, const Graph &G)
{
    for (int k = 0; k <= G.order(); ++k)
    {
        if (is_colourable_sat(solver, G, k))
            return k;
    }
    return -1;
}

inline int chromatic_index_sat(const SatSolver &solver, const Graph &G)
{
    if (has_loop(G))
        return -1;
    for (int k = 0; k <= G.size(); ++k)
    {
        if (is_edge_colourable_sat(solver, G, k))
            return k;
    }
    return -1;
}

}  // namespace end

#endif

