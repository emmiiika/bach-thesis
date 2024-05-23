#include "implementation.h"

#include <graphs.hpp>
#include <sat/solver_cmsat.hpp>
#include <sat/exec_circular_colouring.hpp>

#include <cassert>

using namespace ba_graph;


int main()
{
    CMSatSolver solver;

    Graph G(complete_graph(4));
    assert(is_circularly_edge_colourable_sat(solver, G, 6, 2));
    std::map<Edge, int> precolouring;
    Edge e01 = G[0].find(Location(0, 1))->e();
    Edge e02 = G[0].find(Location(0, 2))->e();
    precolouring[e01] = 2;
    precolouring[e02] = 2;
    assert(!is_circularly_edge_colourable_sat(solver, G, 3, 1, precolouring));
    precolouring[e02] = 8;
    assert(is_circularly_edge_colourable_sat(solver, G, 9, 3, precolouring));

    G = std::move(empty_graph(4));
    addE(G, Location(0, 1));
    addE(G, Location(1, 2));
    addE(G, Location(1, 3));
    addE(G, Location(2, 3));
    assert(!is_circularly_colourable_sat(solver, G, 0, 1));
    assert(!is_circularly_colourable_sat(solver, G, 1, 1));
    assert(!is_circularly_colourable_sat(solver, G, 2, 1));
    assert(is_circularly_colourable_sat(solver, G, 3, 1));
    assert(is_circularly_colourable_sat(solver, G, 7, 2));

    G = std::move(circuit(5));
    assert(is_circularly_colourable_sat(solver, G, 5, 2));
    assert(!is_circularly_colourable_sat(solver, G, 4, 2));
    assert(!is_circularly_colourable_sat(solver, G, 99, 40));
    assert(circular_chromatic_number_sat(solver, G) == std::pair(5, 2));

    G = std::move(circuit(35));
    assert(circular_chromatic_number_sat(solver, G) == std::pair(35, 17));

    G = std::move(create_petersen());
    assert(is_circularly_edge_colourable_sat(solver, G, 11, 3));
    assert(!is_circularly_edge_colourable_sat(solver, G, 7, 2));
    assert(!is_circularly_edge_colourable_sat(solver, G, 32, 9));
    assert(circular_chromatic_index_sat(solver, G) == std::pair(11, 3));

    return 0;
}
