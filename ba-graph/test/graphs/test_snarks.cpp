#include "implementation.h"

#include <algorithms/cyclic_connectivity.hpp>
#include <graphs/snarks.hpp>
#include <invariants/girth.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    Graph G(create_petersen());
    assert(G.order() == 10);
    assert(G.size() == 15);
    for (auto &r : G)
        assert(r.degree() == 3);
    assert(girth(G) == 5);
    assert(cyclic_connectivity(G) == 5);

    G = std::move(create_isaacs(3));
    assert(G.order() == 12);
    assert(G.size() == 18);
    for (auto &r : G)
        assert(r.degree() == 3);
    assert(girth(G) == 3);
    assert(cyclic_connectivity(G) == 3);

    G = std::move(create_isaacs(5));
    assert(G.order() == 20);
    assert(G.size() == 30);
    for (auto &r : G)
        assert(r.degree() == 3);
    assert(girth(G) == 5);
    assert(cyclic_connectivity(G) == 5);

    G = std::move(create_isaacs(6));
    assert(G.order() == 24);
    assert(G.size() == 36);
    for (auto &r : G)
        assert(r.degree() == 3);
    assert(girth(G) == 6);
    assert(cyclic_connectivity(G) == 6);
}
