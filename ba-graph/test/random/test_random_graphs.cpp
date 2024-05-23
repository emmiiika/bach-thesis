#include "implementation.h"

#include <random/random_graphs.hpp>
#include <invariants.hpp>

#include <cassert>


using namespace ba_graph;

int main() {
    Graph g1(random_pseudograph(5, 10));
    assert(g1.order() == 5);
    assert(g1.size() == 10);

    Graph g2(random_multigraph(20, 100));
    assert(g2.order() == 20);
    assert(g2.size() == 100);
    assert(girth(g2) > 1);

    Graph g3(random_simple_graph_unreliable(20, 100));
    assert(g3.order() == 20);
    assert(g3.size() <= 100);
    assert(girth(g3) > 2);

    Graph g4(random_simple_graph_reliable(20, 100));
    assert(g4.order() == 20);
    assert(g4.size() == 100);
    assert(girth(g4) > 2);

    Graph g5(random_bipartite_regular_multigraph(20, 5));
    assert(g5.order() == 20);
    assert(max_deg(g5) == 5);
    assert(min_deg(g5) == 5);
    assert(g5.size() == 50);
    assert(girth(g5) % 2 == 0);

    int order = 100, degree = 3;
    Graph g6(random_regular_multigraph(order, degree));
    assert(g6.order() == order);
    assert(max_deg(g6) == degree);
    assert(min_deg(g6) == degree);
    assert(g6.size() == order * 3 / 2);
    assert(girth(g6) > 1);

    order = 100; degree = 8;
    Graph g7(random_regular_multigraph(order, degree));
    assert(g7.order() == order);
    assert(max_deg(g7) == degree);
    assert(min_deg(g7) == degree);
    assert(g7.size() == order * degree / 2);
    assert(girth(g7) > 1);

    order = 10; degree = 1;
    Graph g8(random_regular_multigraph(order, degree));
    assert(g8.order() == order);
    assert(max_deg(g8) == degree);
    assert(min_deg(g8) == degree);
    assert(g8.size() == order * degree / 2);
    assert(girth(g8) == -1);

    return 0;
}
