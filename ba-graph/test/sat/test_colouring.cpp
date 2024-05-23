#include "implementation.h"

#include <graphs.hpp>
#include <sat/solver_cmsat.hpp>
#include <sat/exec_colouring.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    CMSatSolver solver;

    Graph a1(complete_graph(4));
    assert(is_edge_colourable_sat(solver, a1, 3));
    std::map<Edge, int> precolouring;
    Edge e01 = a1[0].find(Location(0, 1))->e();
    Edge e02 = a1[0].find(Location(0, 2))->e();
    precolouring[e01] = 2;
    precolouring[e02] = 2;
    assert(!is_edge_colourable_sat(solver, a1, 3, precolouring));
    precolouring[e02] = 0;
    assert(is_edge_colourable_sat(solver, a1, 3, precolouring));

    Graph g1(empty_graph(4));
    addE(g1, Location(0, 1));
    addE(g1, Location(1, 2));
    addE(g1, Location(1, 3));
    addE(g1, Location(2, 3));
    assert(is_colourable_sat(solver, g1, 0) == false);
    assert(is_colourable_sat(solver, g1, 1) == false);
    assert(is_colourable_sat(solver, g1, 2) == false);
    assert(is_colourable_sat(solver, g1, 3) == true);
    assert(is_colourable_sat(solver, g1, 4) == true);

    Graph g2(create_petersen());
    assert(chromatic_number_sat(solver, g2) == 3);
    assert(chromatic_index_sat(solver, g2) == 4);
    deleteV(g2, 0);
    assert(chromatic_index_sat(solver, g2) == 4);
    deleteV(g2, 7);
    assert(chromatic_index_sat(solver, g2) == 3);

    return 0;
}
