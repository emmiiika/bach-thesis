#include "implementation.h"
#include <operations.hpp>
//#include <io/print_nice.hpp>
#include <cassert>
#include <set>

using namespace ba_graph;

int main() {
    Graph G(createG());
    addMultipleV(G, 5);
    for(int i=0;i<5;i++)
        for(int j=i+1;j<5;j++)
            addE(G, Loc(i, j));
    split_vertex(G,0,5);

    auto [H, m]=copy_disjoint<EdgeMapper>(G);
    add_graph(G, H, 10);
    (void) m;

    for(int i=5;i<9;i++) {
        identify_vertices(G, i, i+10);
        suppress_vertex(G,i);
    }

    assert(G.order()==8);
    assert(G[1].degree()==4);
    assert(G[2].degree()==4);
    assert(G[3].degree()==4);
    assert(G[4].degree()==4);
    assert(G[11].degree()==4);
    assert(G[12].degree()==4);
    assert(G[13].degree()==4);
    assert(G[14].degree()==4);

    Graph H2(copy_identical(G));

    Edge e=std::find_if(G[1].begin(),G[1].end(),IP::IP(Number(2)))->e();
    subdivide_edge(G,e,0);
    add_graph(G,H2,0); //no vertices added, just edges
    assert(G[0].degree()==2);
    assert(G[1].degree()==5);
    assert(G[2].degree()==5);

    contract_edge(G,Location(11,12));
    assert(!G.contains(Number(12)));
    assert(G[11].degree()==6);

}
