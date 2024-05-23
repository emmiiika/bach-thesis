#include "implementation.h"

#include <operations/undo.hpp>

#include <cassert>

using namespace ba_graph;

int max_deg(const Graph &g) { 
    int m=-1;
    for(auto const &r:g) {if (m<r.degree()) m=r.degree();};
    return m;
}
int min_deg(const Graph &g) { 
    int m=1000;
    for(auto const &r:g) {if (m>r.degree()) m=r.degree();};
    return m;
}


int main()
{
    Factory f;
    Graph G(createG(f));
    addMultipleV(G, 5, f);
    for(int i=0;i<5;i++)
        for(int j=i+1;j<5;j++)
            addE(G, Loc(i, j), f);

    auto clearedEdges = clear_edges(G, std::vector<Number>({1, 2}), f);
    assert(G.order() == 5);
    assert(G.size() == 3);
    assert(max_deg(G) == 2);
    assert(min_deg(G) == 0);

    restore_edges(G, clearedEdges, f);
    assert(G.order() == 5);
    assert(G.size() == 10);
    assert(max_deg(G) == 4);
    assert(min_deg(G) == 4);

    clearedEdges = clear_edges(G, {Location(1, 2), Location(1, 3), Location(4, 0)}, f);
    assert(G.order() == 5);
    assert(G.size() == 7);
    assert(max_deg(G) == 3);
    assert(min_deg(G) == 2);
    restore_edges(G, clearedEdges, f);
    assert(G.order() == 5);
    assert(G.size() == 10);
    assert(max_deg(G) == 4);
    assert(min_deg(G) == 4);
}
