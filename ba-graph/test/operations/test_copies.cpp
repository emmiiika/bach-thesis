#include "implementation.h"

#include <operations/copies.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    Graph G(createG());
    Vertex v0 = addV(G, 0).v();
    Vertex v1 = addV(G, 1).v();
    Edge e1 = addE(G, Location(0, 1)).e();
    Edge e2 = addE(G, Location(0, 1)).e();

    Graph H(copy_identical(G));
    assert(H.order() == 2);
    assert(H.size() == 2);
    assert(H[0].v() == v0);
    assert(std::find_if(H[0].begin(), H[0].end(), IP::IP(e2))->e() == e2);

    H = std::move(copy_disjoint(G, static_factory));
    assert(H.order() == 2);
    assert(H.size() == 2);
    assert(H[0].v() != v0);
    assert(std::find_if(H[0].begin(), H[0].end(), IP::IP(e2)) == H[0].end());

	Factory f2;
    H = std::move(copy_other_factory(G, f2));
    assert(H.order() == 2);
    assert(H.size() == 2);

    //we may also obtain labelings using template arguments
    Graph G4 = copy_disjoint<NoMapper>(G); //we obtain nothing this way
    assert(G4.order() == 2);
    assert(G4.size() == 2);

    auto [G5, m1] = copy_disjoint<EdgeMapper>(G, static_factory); //we obtain Edge mapping
    auto [G6, m2] = copy_disjoint<HalfEdgeMapper>(G);             //we obtain HalfEdge mapping

    Edge eG5 = std::find_if(G5[0].begin(), G5[0].end(), IP::IP(Number(1)))->e();
    assert(eG5 != e1 && eG5 != e2);
    assert(m1.get(e1) == eG5 || m1.get(e2) == eG5);

    HalfEdge hG6 = std::find_if(G6[0].begin(), G6[0].end(), IP::IP(Number(1)))->h1();
    assert(hG6 != e1.h1() && hG6 != e2.h1());
    assert(m2.get(e1.h1()) == hG6 || m2.get(e2.h1()) == hG6);

    Factory f;
    auto [H1, mH1] = copy_other_factory<EdgeMapper>(G, f);
    assert(H1.order() == 2);
    assert(H1.size() == 2);
    assert(&H1[0].v() != &v0);
    assert(std::find_if(H1[0].begin(), H1[0].end(), IP::IP(mH1.get(e2))) != H1[0].end());
    auto [H2, mH2] = copy_other_factory<HalfEdgeMapper>(G, f);
    assert(H2.order() == 2);
    assert(H2.size() == 2);
    assert(H2[0].v() != v0);
    assert(std::find_if(H2[0].begin(), H2[0].end(), IP::IP(mH2.get(e2.h1()))) != H2[0].end());
}
