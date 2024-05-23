#include "implementation.h"

#include <graphs/snarks.hpp>
#include <snarks/superposition.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    Graph G(create_petersen());
    Graph H(create_petersen());
    split_vertex(H, 0, 10);
    split_vertex(H, 2, 13);
    Multipole m({Connector({10, 11, 12}), Connector({13, 14, 15})});
    superposition(G, {0, 1, 2, 3, 4}, {&H, &H, &H, &H, &H}, {m, m, m, m, m}, 20, 20);

    for (const auto &rot : G)
        assert(rot.degree() == 3);
    assert(G.order() == 50);
}
