#include "implementation.h"

#include <graphs.hpp>
#include <sat/solver.hpp>
#include <sat/exec_solver.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    Configuration cfg;
    cfg.load_from_string(R"delim({
        "storage": {
            "dir": "../../resources/graphs"
        },
        "sat_solvers": [
            {
                "output_type": "SAT_COMPETITION",
                "execute_command": "cryptominisat5 -t 2 --reconfat 0 --reconf 4 --verb 0"
            }
        ],
        "allsat_solvers": [
            {
                "output_type": "BDD_MINISAT",
                "execute_command": "bddminisat_static"
            }
        ]
    })delim");
    SatSolver solver(cfg, 0);
    AllSatSolver allSolver(cfg, 0);

    assert(satisfiable(solver, cnf_satisfiable));
    assert(!satisfiable(solver, cnf_unsatisfiable));

    CNF cnf = std::pair(3, std::vector({
        std::vector({Lit(0, false), Lit(1, true)}),
        std::vector({Lit(1, false), Lit(2, true)})
    }));
    assert(satisfiable(solver, cnf));
    cnf.second.push_back({Lit(0, false), Lit(1, false)});
    cnf.second.push_back({Lit(0, true), Lit(1, false)});
    cnf.second.push_back({Lit(0, true), Lit(1, true)});
    assert(!satisfiable(solver, cnf));
}
