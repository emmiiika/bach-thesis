#ifndef BA_GRAPH_GRAPHS_CPG_HPP
#define BA_GRAPH_GRAPHS_CPG_HPP

#include <array>


#include "basic.hpp"
#include "../operations/add_graph.hpp"
#include "../structures/permutation.hpp"

namespace ba_graph {

/**
 * Given a permutation `p` of degree `N` it creates a cycle permutation graph.
 *
 * From the definition, the permutation `p` (acting on {0, 1, ..., N-1}) have to fix the point 0.
 */
    template<unsigned N>
    Graph cycle_permutation_graph(permutation<N> const &p, Factory &f = static_factory) {
#ifdef BA_GRAPH_DEBUG  // precondition tests
        assert(p(0) == 0); // p must fix 0
#endif

        Graph G(empty_graph(2 * N, f));
        for (unsigned i = 0; i != N; ++i) {
            addE(G, Location(i, i + 1 == N ? 0 : i + 1), f);
            addE(G, Location(i + N, i + 1 == N ? N : N + i + 1), f);
            addE(G, Location(i, p(i) == 0 ? N : p(i) + N), f);
        }
#ifdef BA_GRAPH_DEBUG  // postcondition tests
        for (auto &v: G) {
            assert(v.degree() == 3); // we shall create a cubic graph
        }
#endif

        return G;
    }

}
#endif // !BA_GRAPH_GRAPHS_CPG_HPP
