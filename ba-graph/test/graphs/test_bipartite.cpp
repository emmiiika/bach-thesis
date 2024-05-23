#include "implementation.h"

#include <graphs/bipartite.hpp>
#include <invariants/degree.hpp>
#include <invariants/girth.hpp>

#include <cassert>

using namespace ba_graph;

int main() {
    Graph G(complete_bipartite_graph(3, 3));
    assert(G.order() == 6);
    assert(G.size() == 9);
    assert(max_deg(G) == 3);
    assert(min_deg(G) == 3);
    assert(girth(G) == 4);

    G = std::move(complete_bipartite_graph(1, 10));
    assert(G.order() == 11);
    assert(G.size() == 10);
    assert(max_deg(G) == 10);
    assert(min_deg(G) == 1);
    assert(girth(G) == -1);

    for (auto [m, n] : std::vector({
            std::pair(0, 0), std::pair(0, 8), std::pair(1, 1),
            std::pair(1, 8), std::pair(2, 3), std::pair(3, 4)})) {
        G = std::move(square_grid(m, n));
        assert(G.order() == m * n);
        int size = (m == 0 || n == 0) ? 0 : ((m-1) * n + (n-1) * m);
        assert(G.size() == size);
        assert(girth(G) == ((m > 1 && n > 1) ? 4 : -1));
    }

    for (auto [m, n] : std::vector({
            std::pair(0, 0), std::pair(0, 8), std::pair(1, 1),
            std::pair(3, 4), std::pair(2, 3), std::pair(1, 8)})) {
        G = std::move(wrapped_square_grid(m, n));
        if (m == 0 || n == 0) {
            assert(G.order() == 0);
            continue;
        }
        int min = std::min(m, n);
        int g = std::min(min, 4);
        assert(G.order() == m * n);
        assert(G.size() == 2 * m * n);
        assert(max_deg(G) == 4);
        assert(min_deg(G) == 4);
        assert(girth(G) == g);
    }
}
