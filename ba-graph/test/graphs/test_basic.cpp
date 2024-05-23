#include "implementation.h"

#include <graphs/basic.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    Graph G(empty_graph(5));
    assert(G.order() == 5);
    assert(G.size() == 0);

    G = std::move(complete_graph(5));
    assert(G.order() == 5);
    assert(G.size() == 10);
}
