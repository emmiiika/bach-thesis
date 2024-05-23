#ifndef BA_GRAPH_SAT_EXEC_CIRCUMFERENCE_HPP
#define BA_GRAPH_SAT_EXEC_CIRCUMFERENCE_HPP

#include "cnf_circumference.hpp"
#include "../invariants/girth.hpp"
#include "exec_solver.hpp"
#include "solver.hpp"

#include "../operations/copies.hpp"
#include "../operations/simplification.hpp"

namespace ba_graph
{

// works for k > 0
inline bool has_circuit_sat(SatSolver solver, const Graph &G, int k, Number w=-1)
{
    if (k == 1) return has_loop(G);
    if (k == 2) return has_parallel_edge(G);
    auto cnf = cnf_has_circuit(G, k, w);
    return satisfiable(solver, cnf);
}

inline bool is_hamiltonian_sat(SatSolver solver, const Graph &G)
{
    if (G.order() == 0) return true;
    if (G.order() == 1) return has_loop(G);
    if (G.order() == 2) return has_parallel_edge(G);

    auto cnf = cnf_has_circuit(G, G.order(), G.find(RP::all())->n());
    return satisfiable(solver, cnf);
}

inline int circumference_sat(SatSolver solver, const Graph &G)
{
    // it seems that testing hamiltonicity is much faster than existence of a shorter circuit
    // maybe because of the precolouring of a vertex
    // TODO this definitely needs more work, experiments and optimization
    // or perhaps the CNF formula can be derived differently?
    // it works quite poorly for cubic graphs compared to backtracking

    // deficit 0
    if (is_hamiltonian_sat(solver, G)) return G.order();

    // if not hamiltonian, then it has at least 1 vertex
    if (G.order() == 1) return true; // if there was a loop, G would be hamiltonian
    // now G has at least 2 vertices

    // deficit 1
    Number v = G.find(RP::all())->n();
    // a circuit of length n-1 either passes through v or G-v is hamiltonian
    if (has_circuit_sat(solver, G, G.order() - 1, v))
        return G.order() - 1;
    else {
        Factory f;
        Graph H(copy_other_factory(G, f));
        deleteV(H, v, f);
        if (is_hamiltonian_sat(solver, H))
            return G.order() - 1;
    }
    // no cycle of length n-1 is possible

    // deficit 2 or more
    for (int k = G.order() - 2; k > 0; --k)
        if (has_circuit_sat(solver, G, k)) return k;

    return 0;
}

}  // namespace end

#endif

