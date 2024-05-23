#include "implementation.h"

#include <graphs.hpp>
#include <sat/cnf.hpp>
#include <sat/solver_cmsat.hpp>
#include <sat/exec_factors.hpp>
#include <sat/exec_snarks.hpp>
#include <snarks/colouring.hpp>

#include <cassert>
#include <vector>

using namespace ba_graph;

bool cb(std::vector<bool> &, int *count)
{
    (*count)++;
    return true;
}

bool cbMatching(std::vector<Edge> &, int *count)
{
    (*count)++;
    return true;
}

int main()
{
    assert(internal::satisfiable_cmsat(cnf_satisfiable));
    assert(!internal::satisfiable_cmsat(cnf_unsatisfiable));

    CNF cnf = std::pair(3, std::vector({std::vector({Lit(0, false), Lit(1, true)}),
                                        std::vector({Lit(1, false), Lit(2, true)})}));
    assert(internal::satisfiable_cmsat(cnf));
    cnf.second.push_back({Lit(0, false), Lit(1, false)});
    cnf.second.push_back({Lit(0, true), Lit(1, false)});
    cnf.second.push_back({Lit(0, true), Lit(1, true)});
    assert(!internal::satisfiable_cmsat(cnf));

    cnf = std::pair(3, std::vector({std::vector({Lit(0, false), Lit(1, true)}),
                                    std::vector({Lit(1, false), Lit(2, true)})}));
    int c = 0;
    assert(internal::all_sat_solutions_cmsat(cnf, cb, &c, internal::identity, (void *)NULL));
    assert(c == 4);

    CMSatColouriser satC;
    Graph g = createG();

    g = std::move(empty_graph(4));
    addE(g, Location(0, 1));
    addE(g, Location(1, 2));
    addE(g, Location(1, 3));
    addE(g, Location(2, 3));
    assert(satC.isColourable(g));

    g = std::move(create_petersen());
    assert(is_critical<CMSatColouriser>(g));
    assert(!satC.isColourable(g));
    deleteV(g, 0);
    assert(!satC.isColourable(g));
    deleteV(g, 7);
    assert(satC.isColourable(g));

    CMSatSolver solver;
    CMAllSatSolver allSolver;

    g = std::move(empty_graph(4));
    addE(g, Location(0, 1));
    addE(g, Location(1, 2));
    addE(g, Location(1, 3));
    addE(g, Location(2, 3));
    assert(has_perfect_matching_sat(solver, g));
    c = 0;
    assert(perfect_matchings_enumerate_sat(allSolver, g, cbMatching, &c));
    assert(c == 1);
    assert(perfect_matchings_list_sat(allSolver, g).size() == 1);
    deleteV(g, 0);
    assert(!has_perfect_matching_sat(solver, g));

    g = std::move(circuit(11));
    assert(!has_perfect_matching_sat(solver, g));
    c = 0;
    assert(!perfect_matchings_enumerate_sat(allSolver, g, cbMatching, &c));
    assert(c == 0);

    g = std::move(circuit(8));
    assert(has_perfect_matching_sat(solver, g));
    c = 0;
    assert(perfect_matchings_enumerate_sat(allSolver, g, cbMatching, &c));
    assert(c == 2);

    g = std::move(complete_graph(4));
    assert(has_perfect_matching_sat(solver, g));
    c = 0;
    assert(perfect_matchings_enumerate_sat(allSolver, g, cbMatching, &c));
    assert(c == 3);

    g = std::move(create_petersen());
    assert(has_perfect_matching_sat(solver, g));
    c = 0;
    assert(perfect_matchings_enumerate_sat(allSolver, g, cbMatching, &c));
    assert(c == 6);
    assert(perfect_matchings_list_sat(allSolver, g).size() == 6);
    deleteV(g, 0);
    assert(!has_perfect_matching_sat(solver, g));
    assert(perfect_matchings_list_sat(allSolver, g).size() == 0);

    return 0;
}
