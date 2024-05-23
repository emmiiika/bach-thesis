#include "implementation.h"

#include <graphs/trees.hpp>
#include <invariants/girth.hpp>
#include <invariants/distance.hpp>

#include <cassert>
#include <set>

using namespace ba_graph;

int main()
{
    Factory f;

    Graph G1 = tree(1, 0, f);
    assert(G1.order() == 1);
    assert(G1.size() == 0);
    assert(girth(G1) == -1);
    assert(distance(G1, 0, G1.order() - 1) == 0);

    Graph G2 = tree(100, 0, f);
    assert(G2.order() == 1);
    assert(G2.size() == 0);
    assert(girth(G2) == -1);
    assert(distance(G2, 0, G2.order() - 1) == 0);

    Graph G3 = tree(5, 1, f);
    assert(G3.order() == 6);
    assert(G3.size() == 5);
    assert(girth(G3) == -1);
    assert(distance(G3, 0, G3.order() - 1) == 1);

    Graph G4 = tree(3, 2, f);
    assert(G4.order() == 13);
    assert(G4.size() == 12);    
    assert(girth(G4) == -1);
    assert(distance(G4, 0, G4.order() - 1) == 2);

    Graph G5 = tree(2, 9, f);
    assert(G5.order() == 1023);
    assert(G5.size() == 1022);    
    assert(girth(G5) == -1);
    assert(distance(G5, 0, G5.order() - 1) == 9);

    Graph G6 = regular_tree(3, 0, f);
    assert(G6.order() == 1);
    assert(G6.size() == 0);    
    assert(girth(G6) == -1);
    assert(distance(G6, 0, G6.order() - 1) == 0);

    Graph G7 = regular_tree(3, 1, f);
    assert(G7.order() == 4);
    assert(G7.size() == 3);    
    assert(girth(G7) == -1);
    assert(distance(G7, 0, G7.order() - 1) == 1);

    Graph G8 = regular_tree(3, 2, f);
    assert(G8.order() == 10);
    assert(G8.size() == 9);    
    assert(girth(G8) == -1);
    assert(distance(G8, 0, G8.order() - 1) == 2);

    return 0;
}
