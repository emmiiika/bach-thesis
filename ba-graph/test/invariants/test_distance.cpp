#include "implementation.h"

#include <cassert>
#include <cmath>

#include <invariants/distance.hpp>
#include <io/graph6.hpp>
#include <graphs.hpp>
#include <operations/basic.hpp>
#include <operations/add_graph.hpp>


using namespace ba_graph;

int main()
{
    const int n = 11;
    Graph G(empty_graph(n));
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            assert(distance(G, i, j) == -1);

    const int a = 5, b = 6;
    G = std::move(complete_bipartite_graph(a, b));
    for (int i = 0; i < a; i++)
        for (int j = a; j < b; j++)
            assert(distance(G, i, j) == 1);
    for (int i = 0; i < a; i++)
        for (int j = i + 1; j < a; j++)
            assert(distance(G, i, j) == 2);
    assert(components(G).size() == 1);
    assert(components(G)[0].size() == a + b);

    G = std::move(empty_graph(4));
    assert(components(G).size() == 4);
    addE(G, Location(0, 1));
    assert(components(G).size() == 3);
    addE(G, Location(0, 2));
    assert(components(G).size() == 2);
    addE(G, Location(0, 3));
    addE(G, Location(1, 2));
    assert(components(G).size() == 1);
    assert(distance(G, 0, 0) == 0);
    assert(distance(G, 0, 1) == 1);
    assert(distance(G, 0, 2) == 1);
    assert(distance(G, 0, 3) == 1);
    assert(distance(G, 1, 2) == 1);
    assert(distance(G, 1, 3) == 2);
    assert(distance(G, 2, 3) == 2);
    auto dist = distances(G, 0);
    assert(dist[0] == 0);
    assert(dist[1] == 1);
    assert(dist[2] == 1);
    assert(dist[3] == 1);
    dist = distances(G, 1);
    assert(dist[0] == 1);
    assert(dist[1] == 0);
    assert(dist[2] == 1);
    assert(dist[3] == 2);
    dist = distances(G, {1, 2});
    assert(dist[0] == 1);
    assert(dist[1] == 0);
    assert(dist[2] == 0);
    assert(dist[3] == 2);
    assert(distance(G, Location(0, 1), Location(0, 2)) == 0);
    assert(distance(G, Location(0, 3), Location(1, 2)) == 1);

    G = std::move(empty_graph(20));
    add_graph(G, std::move(complete_bipartite_graph(3, 4)), min_offset(G));
    add_graph(G, std::move(complete_bipartite_graph(3, 4)), min_offset(G));
    assert(components(G).size() == 22);

    G = std::move(read_graph6_line(":Qb?``aaCbDbdHeHfIgJK`ace"));
    assert(components(G).size() == 1);

    G = create_petersen();
    assert(distance(G, 0, 1, 0) == -1);
    assert(distance(G, 0, 1, 1) == 1);
    assert(distance(G, 0, 1, 2) == 1);
    assert(distance(G, 0, 3, 0) == -1);
    assert(distance(G, 0, 3, 1) == -1);
    assert(distance(G, 0, 3, 2) == 2);
    assert(distance(G, 0, 3, 3) == 2);
}
