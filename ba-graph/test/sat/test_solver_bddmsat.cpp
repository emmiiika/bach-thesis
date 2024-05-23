#include <cassert>
#include <iostream>

#include "implementation.h"
#include <graphs.hpp>
#include <invariants/girth.hpp>
#include <sat/cnf.hpp>
#include <sat/solver_bddmsat.hpp>
#include <sat/exec_factors.hpp>
#include <sat/exec_snarks.hpp>
#include <sat/exec_solver.hpp>

using namespace ba_graph;

bool cb2f(Graph &, int *count)
{
    (*count)++;
    return true;
}

bool cb2fPetersen(Graph &F, int *count)
{
    assert(F.order() == 10);
    assert(F.size() == 10);
    for (auto &v : F)
        assert(shortest_cycle_length(F, v.n()) == 5);
    (*count)++;
    return true;
}

bool cbCount(std::vector<bool> &, int *count)
{
    (*count)++;
    return true;
}

bool cb1f(std::vector<Edge> &perfectMatching, int *count)
{
    perfectMatching = perfectMatching;
    (*count)++;
    return true;
}

int main()
{
    Configuration cfg;
    cfg.load_from_string(R"delim({
        "storage": {
            "dir": "../../resources/graphs"
        },
        "allsat_solvers": [
            {
                "output_type": "BDD_MINISAT",
                "execute_command": "bdd_minisat_all_static"
            }
        ]
    })delim");
    AllSatSolver solver(cfg, 0);

    CNF cnf = std::pair(3, std::vector({std::vector({Lit(0, false), Lit(1, true)}),
                                        std::vector({Lit(1, false), Lit(2, true)})}));
    int c = 0;
    assert(all_sat_solutions(solver, cnf, cbCount, &c));
    assert(c == 4);

    Graph g = createG();

    g = std::move(empty_graph(4));
    addE(g, Location(0, 1));
    addE(g, Location(1, 2));
    addE(g, Location(1, 3));
    addE(g, Location(2, 3));
    c = 0;
    assert(perfect_matchings_enumerate_sat(solver, g, cb1f, &c));
    assert(c == 1);
    assert(perfect_matchings_list_sat(solver, g).size() == 1);

    g = std::move(circuit(11));
    c = 0;
    assert(!perfect_matchings_enumerate_sat(solver, g, cb1f, &c));
    assert(c == 0);

    g = std::move(circuit(8));
    c = 0;
    assert(perfect_matchings_enumerate_sat(solver, g, cb1f, &c));
    assert(c == 2);

    g = std::move(complete_graph(4));
    c = 0;
    assert(perfect_matchings_enumerate_sat(solver, g, cb1f, &c));
    assert(c == 3);

    g = std::move(create_petersen());
    c = 0;
    assert(perfect_matchings_enumerate_sat(solver, g, cb1f, &c));
    assert(c == 6);
    assert(perfect_matchings_list_sat(solver, g).size() == 6);
    deleteV(g, 0);
    assert(perfect_matchings_list_sat(solver, g).size() == 0);

    Graph g4(complete_graph(4));
    c = 0;
    assert(two_factors_enumerate_allsat_cubic(solver, g4, static_factory, cb2f, &c));
    assert(c == 3);

    Graph g5(create_petersen());
    c = 0;
    assert(two_factors_enumerate_allsat_cubic(solver, g5, static_factory, cb2fPetersen, &c));
    assert(c == 6);
    assert(oddness_allsat(solver, g5) == 2);
    assert(two_factors_list_allsat_cubic(solver, g5, static_factory).size() == 6);

    for (int i = 4; i <= 14; i += 2) {
        std::string filename = "../../resources/graphs/"
                               + internal::stored_cubic_path(1, 3, i);
        Factory f;
        auto graphs = read_graph6_file(filename, f).graphs();
        for (Graph &G : graphs)
            assert(oddness_allsat(solver, G) <= 2);
    }

    for (int i = 10; i <= 10; i += 2) {
        std::string filename = "../../resources/graphs/"
                               + internal::stored_cubic_path(1, 3, i);
        Factory f;
        auto graphs = read_graph6_file(filename, f).graphs();
        int snarkCount = 0;
        for (Graph &G : graphs) {
            if (oddness_allsat(solver, G) > 0)
                snarkCount++;
        }
        assert(snarkCount == 2);
    }

    return 0;
}
