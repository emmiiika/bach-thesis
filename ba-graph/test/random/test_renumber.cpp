#include "implementation.h"

#include <operations/copies.hpp>
#include <random/random_graphs.hpp>
#include <random/renumber.hpp>

#include <cassert>

using namespace ba_graph;

int main() {
    Graph g1(random_pseudograph(5, 10));
    Graph g1c(copy_disjoint(g1));
	auto m = randomize_vertices(g1, static_factory);
    assert(g1.order() == 5);
    assert(g1.size() == 10);
    for (auto &v : g1c)
        assert(v.degree() == g1[m.get(v.n())].degree());

    return 0;
}
