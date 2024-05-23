#ifndef BA_GRAPH_SAT_EXEC_CIRCULAR_COLOURING_HPP
#define BA_GRAPH_SAT_EXEC_CIRCULAR_COLOURING_HPP

#include "cnf_circular_colouring.hpp"
#include "exec_colouring.hpp"
#include "exec_solver.hpp"
#include "solver.hpp"

namespace ba_graph {

/*
 * ==================== circular vertex- and edge-colourings =========================
 */

// determines if G has a k-colouring
inline bool is_circularly_colourable_sat(const SatSolver &solver, const Graph &G, int p, int q,
                                    std::map<Vertex, int> precolouring)
{
    auto cnf = cnf_circular_vertex_colouring(G, p, q, precolouring);
    return satisfiable(solver, cnf);
}

// determines if G has a k-colouring
inline bool is_circularly_colourable_sat(const SatSolver &solver, const Graph &G, int p, int q)
{
    auto cnf = cnf_circular_vertex_colouring(G, p, q, true);
    return satisfiable(solver, cnf);
}

// determines if G has a k-edge-colouring
inline bool is_circularly_edge_colourable_sat(const SatSolver &solver, const Graph &G, int p, int q,
                                         std::map<Edge, int> precolouring)
{
    auto cnf = cnf_circular_edge_colouring(G, p, q, precolouring);
    return satisfiable(solver, cnf);
}

// determines if G has a k-edge-colouring
inline bool is_circularly_edge_colourable_sat(const SatSolver &solver, const Graph &G, int p, int q)
{
    auto cnf = cnf_circular_edge_colouring(G, p, q, true);
    return satisfiable(solver, cnf);
}

inline std::pair<int, int> circular_chromatic_number_sat(const SatSolver &solver, const Graph &G,
        std::ostream *reportProgressStream = NULL)
{
    int chn = chromatic_number_sat(solver, G);
    auto sortedFractions = internal::computeCircularColouringFractions(G, chn);
    int minI = 0, maxI = sortedFractions.size() - 1;
    while (minI < maxI) {
        int smallestNumeratorI = minI;
        for (int j = minI; j < maxI; ++j) {
            if (sortedFractions[j].first < sortedFractions[smallestNumeratorI].first)
                smallestNumeratorI = j;
        }
        auto [p, q] =  sortedFractions[smallestNumeratorI];
        if (reportProgressStream)
            *reportProgressStream << "trying " << p << "/" << q;
        bool r = is_circularly_colourable_sat(solver, G, p, q);
        if (reportProgressStream) {
            if (r) *reportProgressStream << " <="; else *reportProgressStream << " >";
            *reportProgressStream << std::endl;
        }
        if (r)
            maxI = smallestNumeratorI;
        else
            minI = smallestNumeratorI + 1;
    }
    return sortedFractions[minI];
}

inline std::pair<int, int> circular_chromatic_index_sat(const SatSolver &solver, const Graph &G,
        std::ostream *reportProgressStream = NULL)
{
    Graph H(line_graph(G));
    return circular_chromatic_number_sat(solver, H, reportProgressStream);
}

}  // namespace end

#endif

