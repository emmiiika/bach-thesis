#include "implementation.h"
#include <operations/add_graph.hpp>
#include <cassert>

using namespace ba_graph;

int main() {
    Vertex v1=createV();
    Vertex v2=createV();
    Vertex v3=createV();
    Vertex v4=createV();
    Vertex v5=createV();
    Edge e1=createE(v1,v2);
    Edge e2=createE(v1,v2);
    Edge e3=createE(v1,v3);
    Edge e4=createE(v1,v4);
    Edge e5=createE(v1,v5);
    Edge e6=createE(v4,v5);
    Graph G1(createG());
    Graph G2(createG());
    addV(G1, v1, 1);
    addV(G1, v2, 2);
    addV(G1, v3, 3);
    addE(G1, e1);
    addE(G1, e3);

    addV(G2, v1, 1);
    addV(G2, v2, 3);
    addV(G2, v4, 2);
    addV(G2, v5, 4);
    addE(G2,e1);
    addE(G2,e2);
    addE(G2,e4);
    addE(G2,e5);
    addE(G2,e6);


    auto nm=add_graph<NumberMapper>(G1, G2, 5);

    assert(G1.order()==5);
    assert(G1.size()==6);
    assert(G1[3].v()==v3);
    assert(G1[7].v()==v4);
    assert(std::find_if(G1[7].begin(),G1[7].end(),IP::IP(v5))->r2().n()==9);

    assert(nm[Number(1)]==1);
    assert(nm[Number(3)]==2);
    assert(nm[Number(2)]==7);

    auto mp=add_disjoint<MapperPair<VertexMapper, EdgeMapper>>(G1, G2, 15);
    assert(G1.order()==9);
    assert(G1.size()==11);
    assert(mp.first[v1]==G1[16].v());
    assert(mp.first[v2]==G1[18].v());
    assert(mp.first[v4]==G1[17].v());
    assert(v1!=mp.first[v1]);
    assert(v2!=mp.first[v2]);
    assert(v4!=mp.first[v4]);
    assert(G1[16].find(mp.second[e2])->v2()==mp.first[v2]);



}
