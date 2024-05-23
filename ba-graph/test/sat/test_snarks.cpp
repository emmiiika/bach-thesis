#include "implementation.h"

#include <graphs.hpp>
#include <snarks/colouring.hpp>
#include <sat/solver_cmsat.hpp>
#include <sat/exec_snarks.hpp>

#include <cassert>

using namespace ba_graph;


int main()
{
    OptimisedColouriser oC;

    Graph g1(empty_graph(4));
    addE(g1, Location(0, 1));
    addE(g1, Location(1, 2));
    addE(g1, Location(1, 3));
    addE(g1, Location(2, 3));
    assert(oC.isColourable(g1));

    Graph g2(create_petersen());
    assert(is_critical<OptimisedColouriser>(g2));
    assert(!oC.isColourable(g2));

    deleteV(g2, 0);
    assert(!oC.isColourable(g2));
    deleteV(g2, 7);
    assert(oC.isColourable(g2));

    return 0;
}
