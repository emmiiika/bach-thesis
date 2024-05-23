#include "implementation.h"

#include <graphs.hpp>
#include <snarks/colouring.hpp>
#include <snarks/colouring_cvd.hpp>
#include <snarks/colouring_kowalik.hpp>
#include <snarks/colouring_nagy.hpp>
#include <snarks/products.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    Graph G(complete_graph(4));
    assert(is_colourable(G));
    for (int i = 0; i < G.size(); ++i) {
        deleteE(G, G.find(RP::all(), IP::all()).second->e());
        assert(is_colourable(G));
    }

    G = std::move(create_isaacs(3));
    assert(!is_critical(G));
    assert(is_cocritical(G));
    assert(!is_irreducible(G));
//    assert(!is_strong(G)); TODO

    Graph H1(create_petersen());
    assert(is_critical(H1));
    assert(is_critical<KowalikColouriser>(H1));
    assert(is_critical<NagyColouriser>(H1));
    assert(is_cocritical<KowalikColouriser>(H1));
    assert(is_cocritical<NagyColouriser>(H1));
    assert(is_irreducible<NagyColouriser>(H1));
    assert(is_irreducible<KowalikColouriser>(H1));
//    assert(!is_strong(H1)); TODO
//    assert(!is_strong<KowalikColouriser>(H1));
    Graph H2(create_petersen());
    dot_product(H1, Location(0, 1), Location(2, 3), H2, Location(0, 1), 13);
    assert(is_critical(H1));
    assert(is_critical<KowalikColouriser>(H1));
    assert(is_critical<NagyColouriser>(H1));
    assert(is_cocritical(H1));
    assert(is_irreducible(H1));
    assert(is_cocritical<KowalikColouriser>(H1));
    assert(is_irreducible<KowalikColouriser>(H1));

    G = std::move(create_petersen());
    int n = G.order(), m = G.size();
    assert(removable_pairs_edges<HeuristicsColouriser>(G).size() == unsigned(3 * n));
    assert(removable_pairs_edges<HeuristicsColouriser>(G, 1).size() == 0);
    assert(nonremovable_pairs_edges<HeuristicsColouriser>(G).size() == unsigned(m * (m-1) / 2 - 3 * n));
    G = std::move(create_isaacs(3));
    n = G.order(), m = G.size();
    assert(removable_pairs_adjacent_vertices<HeuristicsColouriser>(G).size() == 3);
    assert(removable_pairs_vertices<HeuristicsColouriser>(G).size() == 3);
    assert(nonremovable_pairs_adjacent_vertices<HeuristicsColouriser>(G).size() == 15);
    assert(nonremovable_pairs_vertices<HeuristicsColouriser>(G).size() == 9 * 8 / 2 + 9 * 3);
    assert(removable_pairs_edges<HeuristicsColouriser>(G).size() == 3 * 9 + 6 * 5 / 2);
    assert(nonremovable_pairs_edges<HeuristicsColouriser>(G).size() == unsigned(m * (m-1) / 2 - 42));

	/*
	FIXME takes too long, use a graph on 28 vertices with oddness 4

    auto graphs = read_graph6_file("../../resources/graphs/snarks/oddness4/oddness4_44.s6");
    G = std::move(graphs[3]);
    n = G.order(), m = G.size();
    assert(removable_pairs_adjacent_vertices<HeuristicsColouriser>(G).size() == unsigned(m));
    assert(removable_pairs_vertices<HeuristicsColouriser>(G).size() == unsigned(n * (n-1) / 2));
    assert(nonremovable_pairs_adjacent_vertices<HeuristicsColouriser>(G).size() == 0);
    assert(nonremovable_pairs_vertices<HeuristicsColouriser>(G).size() == 0);
    assert(removable_pairs_edges<HeuristicsColouriser>(G).size() == unsigned(m * (m-1) / 2));
    assert(nonremovable_pairs_edges<HeuristicsColouriser>(G).size() == 0);
	*/

    // TODO add test for a strong snark (construct at least one, perhaps we can put those snarks
    // of order 28 and oddness4 do kniznice, aj nejaky nas oddness 4 cyklicky 4-suvisly aspon jeden
}
