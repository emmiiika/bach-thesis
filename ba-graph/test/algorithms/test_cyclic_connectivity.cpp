#include "implementation.h"

#include <algorithms/cyclic_connectivity.hpp>
#include <io/graph6.hpp>
#include <graphs.hpp>
#include <multipoles.hpp>
#include <operations/basic.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    Graph g1(create_petersen());
    assert(cyclic_connectivity(g1) == 5);
    assert(is_cyclically_connected(g1, 1));
    assert(is_cyclically_connected(g1, 2));
    assert(is_cyclically_connected(g1, 3));
    assert(is_cyclically_connected(g1, 4));
    assert(is_cyclically_connected(g1, 5));
    assert(!is_cyclically_connected(g1, 6));
    assert(cyclic_connectivity_with_cuts(g1).second.size() == 0); // no non-trivial cycle-separating cuts

    const std::string graphsDir = "../../resources/graphs/";

    auto graphs = read_graph6_file(graphsDir + internal::stored_snark_path(4, 5, 18)).graphs();
    for (Graph &G : graphs)
        assert(cyclic_connectivity(G) == 4);
    assert(cyclic_connectivity_with_cuts(graphs[0]).second.size() == 2);
    assert(cyclic_connectivity_with_cuts(graphs[1]).second.size() == 1);

    graphs = read_graph6_file(graphsDir + internal::stored_snark_path(4, 5, 20)).graphs();
    int c4 = 0, c5 = 0, c6 = 0;
    int j5index = -1;
    int count = 0;
    for (Graph &G : graphs) {
        if (cyclic_connectivity(G) == 4) {
            ++c4;
            assert(is_cyclically_connected(G, 4));
            assert(!is_cyclically_connected(G, 5));
        } else if (cyclic_connectivity(G) == 5) {
            j5index = count;
            ++c5;
            assert(is_cyclically_connected(G, 5));
            assert(!is_cyclically_connected(G, 6));
        } else if (cyclic_connectivity(G) == 6) {
            ++c6;
            assert(is_cyclically_connected(G, 6));
            assert(!is_cyclically_connected(G, 7));
        }
        count++;
    }
    assert((c4 == 5) && (c5 == 1) && (c6 == 0));

    Graph j5 = copy_identical(graphs[j5index]);
    Number u = -1, v = -1;
    for (auto &r : j5) {
        if (shortest_cycle_length(j5, r.n()) == 5 && u == -1)
            u = r.n();
    }
    for (auto &r : j5) {
        if (shortest_cycle_length(j5, r.n()) == 6 && v == -1 && distance(j5, u, r.n()) > 2)
            v = r.n();
    }
    auto m = multipole_by_removing(j5, {u, v}, min_offset(j5), static_factory);
    assert(!contains_cc_obstacle(j5, m, 6));

/*
    FIXME takes too long

    graphs = read_graph6_file(graphsDir + internal::stored_snark_path(4, 5, 26)).graphs();
    c4 = 0, c5 = 0, c6 = 0;
    for (Graph &G : graphs) {
        if (cyclic_connectivity(G) == 4) {
            ++c4;
            assert(is_cyclically_connected(G, 4));
            assert(!is_cyclically_connected(G, 5));
        } else if (cyclic_connectivity(G) == 5) {
            ++c5;
            assert(is_cyclically_connected(G, 5));
            assert(!is_cyclically_connected(G, 6));
        } else if (cyclic_connectivity(G) == 6) {
            ++c6;
            assert(is_cyclically_connected(G, 6));
            assert(!is_cyclically_connected(G, 7));
        }
    }
    assert((c4 == 280-10) && (c5 == 10) && (c6 == 0));
*/
}
