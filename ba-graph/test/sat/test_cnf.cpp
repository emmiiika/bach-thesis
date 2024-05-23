#include "implementation.h"

#include <sat/cnf.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    CNF cnf = std::pair(3, std::vector({std::vector({Lit(0, false), Lit(1, true)}),
                                        std::vector({Lit(1, false), Lit(2, true)})}));
    assert(cnf_dimacs(cnf) == "p cnf 3 2\n1 -2 0\n2 -3 0\n");

    auto f = dnf_to_cnf(cnf.second, 3);
    assert(cnf_dimacs(std::pair(5, f)) == "p cnf 5 5\n-4 1 0\n-4 -2 0\n-5 2 0\n-5 -3 0\n4 5 0\n");

    return 0;
}
