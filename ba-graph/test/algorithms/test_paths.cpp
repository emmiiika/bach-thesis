#include "implementation.h"

#include <algorithms/paths.hpp>
#include <graphs.hpp>
#include <io/graph6.hpp>
#include <operations/basic.hpp>

#include <cassert>

#include <io/print_nice.hpp>

using namespace ba_graph;

int main()
{
    Graph G(empty_graph(4));
    assert(minimum_uncovered_vertices(G, {{0, 1}}) == 4);
    assert(!is_hypohamiltonian_basic(G));
    assert(all_paths(G).size() == 4);
    assert(all_circuits(G).size() == 0);

    G = std::move(circuit(7));
    assert(all_paths(G, 0).size() == 7);
    assert(all_paths(G, 1).size() == 7);
    assert(all_paths(G, 2).size() == 7);
    assert(all_paths(G, 4, 7).size() == 21);
    assert(all_paths(G, 8).size() == 0);
    assert(all_paths(G).size() == 49);
    assert(all_circuits(G).size() == 1);

    G = std::move(complete_graph(4));
    assert(all_paths(G).size() == 34); 
    assert(all_circuits(G).size() == 7);
    deleteE(G, Location(0, 1));
    assert(all_paths(G).size() == 4 + 5 + 8 + 6); 
    assert(all_circuits(G).size() == 3);

    G = std::move(square_grid(4, 4));
    assert(is_hamiltonian_basic(G));
    assert(!is_hypohamiltonian_basic(G));

    G = std::move(square_grid(3, 3));
    assert(minimum_uncovered_vertices(G, {{0, 8}}) == 0);
    assert(minimum_uncovered_vertices(G, {{0, 5}}) == 1);
    assert(minimum_uncovered_vertices(G, {{3, 5}}) == 2);
    assert(minimum_uncovered_vertices(G, {{4, 8}}) == 0);
    assert(minimum_uncovered_vertices(G, {{4, 5}}) == 1);
    assert(minimum_uncovered_vertices(G, {{0, 2}, {3, 5}}) == 1);
    assert(minimum_uncovered_vertices(G, {{0, 2}, {6, 8}}) == 1);
    assert(minimum_uncovered_vertices(G, {{0, 2}, {5, 6}}) == 0);
    assert(minimum_uncovered_vertices(G, {{0, 8}, {2, 6}}) == 9);
    assert(minimum_uncovered_vertices(G, {{0, 8}, {6, 7}}) == 0);
    assert(minimum_uncovered_vertices(G, {{0, 4}, {6, 7}}) == 2);
    assert(!is_hamiltonian_basic(G));
    assert(is_hypohamiltonian_basic(G));

    G = std::move(create_petersen());
    assert(is_hypohamiltonian_basic(G));
    split_vertex(G, 0, 10);
    // dangling edges are 1--10, 4--11, 5--12
    deleteE(G, Location(2, 3));
    assert(minimum_uncovered_vertices(G, {{10, 11}}, {12}) == 1);
    assert(minimum_uncovered_vertices(G, {{2, 3}, {10, 11}}, {12}) == 0);

	std::string s = ":S___``aDbCbdjEfgKhJLiJKcagh";
    G = std::move(read_graph6_line(s));
    assert(minimum_uncovered_vertices(G, {{10, 17}, {16, 18}}, {19}) == 0);

	s = ":U___``aCbCbdeGlGJhJhKLiKMfkdaj";
    G = std::move(read_graph6_line(s));
    assert(minimum_uncovered_vertices(G, {{17, 19}, {18, 21}}, {20}) == 0);

	s = "]???????@???A?O???G?@?_C?g_AK?????_??A?S?I??M??CC?C?@AG??O??G_?@?OCO???@__";
    G = std::move(read_graph6_line(s));
    assert(minimum_uncovered_vertices(G, {{0, 4}, {1, 5}, {2, 6}}, {3}) == 0);

/*
    // long with -O0
    G = std::move(read_graph6_line(":ga?GGQMBBOGGGPJ@@HgiTOEBh{oZN_@gWUYPfCy`UkVKOjS[vJMOOsKPILvbyAWYff[QNHOGhYPLJ"));
    assert(!is_hamiltonian_basic(G));
    assert(is_hypohamiltonian_basic(G));
*/

    /* takes more than a minute on this snark with oddness 4 and 44 vertices
    G = std::move(read_graph6_line("ks?GO????????@??_???Q?CG?O??OG?D??CG??O_??g??c??E???????C??GA??A?G??c@??C_J???????A????W????W??C?????O???G@????AA?????G?????q????AOC?????O_????@?_????_H????@?O"));
    assert(!is_hypohamiltonian_basic(G)); */

    return 0;
}
