#include "implementation.h"

#include <invariants/degree.hpp>
#include <graphs/basic.hpp>

#include <cassert>
#include <cmath>

using namespace ba_graph;

int main()
{
    Graph G(empty_graph(11));
    assert(min_deg(G) == 0);
    assert(max_deg(G) == 0);
    assert(fabs(avg_deg(G)) < 1e-6);

    G = (empty_graph(4));
    addE(G, Location(0, 1));
    addE(G, Location(0, 2));
    addE(G, Location(0, 3));
    addE(G, Location(1, 2));
    assert(min_deg(G) == 1);
    assert(min_deg_vertex(G) == 3);
    assert(max_deg(G) == 3);
    assert(max_deg_vertex(G) == 0);
    assert(fabs(avg_deg(G) - 2) < 1e-6);

    auto ds = degree_sequence(G);
    std::vector<int> answer = {3, 2, 2, 1};
    assert(ds.size() == answer.size());
    for (std::vector<int>::size_type i = 0; i < answer.size(); ++i)
        assert(answer[i] == ds[i]);
}
