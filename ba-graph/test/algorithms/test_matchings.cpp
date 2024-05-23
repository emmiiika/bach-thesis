#include "implementation.h"

#include <algorithms/matchings.hpp>
#include <graphs.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    Graph G(empty_graph(4));
    assert(all_matchings(G).size() == 1);

    G = std::move(complete_graph(4));
    assert(all_matchings(G, 1).size() == 6);
    assert(all_matchings(G, 2).size() == 3);
    assert(all_matchings(G, 3).size() == 0);
    assert(all_matchings(G, 4).size() == 0);

    G = std::move(square_grid(3, 3));
    assert(all_matchings(G, 1).size() == unsigned(G.size()));

    G = std::move(create_petersen());
    assert(all_matchings(G, 5).size() == 6);

    return 0;
}
