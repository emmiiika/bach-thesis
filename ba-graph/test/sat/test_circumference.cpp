#include <cassert>

#include "implementation.h"
#include <graphs.hpp>
#include <invariants/girth.hpp>
#include <sat/solver_cmsat.hpp>
#include <sat/exec_circumference.hpp>

using namespace ba_graph;


int main()
{
    CMSatSolver solver;

    Graph G(empty_graph(3));
    assert(!is_hamiltonian_sat(solver, G));
    assert(circumference_sat(solver, G) == 0);
    addE(G, Location(0, 1));
    assert(!has_circuit_sat(solver, G, 3));
    assert(!is_hamiltonian_sat(solver, G));
    assert(circumference_sat(solver, G) == 0);
    addE(G, Location(0, 0));
    assert(circumference_sat(solver, G) == 1);
    addE(G, Location(0, 1));
    assert(!is_hamiltonian_sat(solver, G));
    assert(circumference_sat(solver, G) == 2);
    addE(G, Location(1, 2));
    assert(circumference_sat(solver, G) == 2);
    addE(G, Location(2, 0));
    assert(is_hamiltonian_sat(solver, G));
    assert(circumference_sat(solver, G) == 3);

    G = std::move(complete_graph(4));
    assert(is_hamiltonian_sat(solver, G));

    G = std::move(circuit(5));
    assert(is_hamiltonian_sat(solver, G));
    assert(circumference_sat(solver, G) == 5);

    G = std::move(circuit(38));
    assert(is_hamiltonian_sat(solver, G));
    assert(circumference_sat(solver, G) == 38);

    G = std::move(create_petersen());
    assert(!is_hamiltonian_sat(solver, G));
    assert(circumference_sat(solver, G) == 9);

    auto graphs = read_graph6_file("../../resources/graphs/"
                                   + internal::stored_snark_path(4, 5, 20)).graphs();
    for (auto &H : graphs) {
        assert(!is_hamiltonian_sat(solver, H));
        assert(circumference_sat(solver, H) == H.order() - 1);
    }
    return 0;
}
