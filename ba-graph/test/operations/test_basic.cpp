#include "implementation.h"

#include <cassert>
#include <set>

#include <operations/basic.hpp>

using namespace ba_graph;

void test_identify_vertices_1() {
    Graph G(createG());
    Vertex v0 = addV(G, 0).v();
    Vertex v1 = addV(G, 1).v();
    Vertex v2 = addV(G, 2).v();
    Vertex v3 = addV(G, 3).v();
    Edge e0 = addE(G, Location(0, 1)).e();
    Edge e1 = addE(G, Location(0, 1)).e();
    Edge e2 = addE(G, Location(3, 2)).e();
    Edge e3 = addE(G, Location(0, 2)).e();
    Edge e4 = addE(G, Location(3, 3)).e();

    auto m = identify_vertices<NumberMapper>(G, 1, 3);
    assert(G.order() == 3);
    assert(G[0].degree() == 3);
    assert(G[1].degree() == 5);
    assert(G[2].degree() == 2);
    assert(m.get(0) == 0);
    assert(m.get(1) == 1);
    assert(m.get(2) == 2);
    assert(m.get(3) == 1);
}

void test_identify_vertices_2() {
    Graph G(createG());
    Vertex v0 = addV(G, 0).v();
    Vertex v1 = addV(G, 1).v();
    Vertex v2 = addV(G, 2).v();
    Vertex v3 = addV(G, 3).v();
    Edge e0 = addE(G, Location(0, 1)).e();
    Edge e1 = addE(G, Location(0, 1)).e();
    Edge e2 = addE(G, Location(3, 2)).e();
    Edge e3 = addE(G, Location(0, 2)).e();
    Edge e4 = addE(G, Location(3, 3)).e();

    auto m = identify_vertices<EdgeMapper>(G, 1, 3);
    assert(G.order() == 3);
    assert(G[0].degree() == 3);
    assert(G[1].degree() == 5);
    assert(G[2].degree() == 2);
    assert(m.get(e0) == e0);
    assert(m.get(e1) == e1);
    assert(m.get(e3) == e3);
    assert(m.get(e2) == G[1].find(Number(2))->e());
    assert(m.get(e4) == G[1].find(Number(1))->e());
}

void test_identify_vertices()
{
    test_identify_vertices_1();
    test_identify_vertices_2();
}

void test_split_vertex()
{
    Graph G(createG());
    Vertex v0 = addV(G, 0).v();
    Vertex v1 = addV(G, 1).v();
    Vertex v2 = addV(G, 2).v();
    Vertex v3 = addV(G, 3).v();
    Edge e0 = addE(G, Location(0, 1)).e();
    Edge e1 = addE(G, Location(0, 1)).e();
    Edge e2 = addE(G, Location(3, 2)).e();
    Edge e3 = addE(G, Location(0, 2)).e();
    Edge e4 = addE(G, Location(3, 3)).e();

    auto m = split_vertex<EdgeMapper>(G, 0, 4);

    assert(G.order() == 6);
    assert(G.size() == 5);
    assert(G[1].degree() == 2);
    assert(G[2].degree() == 2);
    assert(G[3].degree() == 3);
    assert(G[4].degree() == 1);
    assert(G[5].degree() == 1);
    assert(G[6].degree() == 1);

    assert(e1 != m.get(e1) && e1 != m.get(e2) && e1 != m.get(e3));
    Number n1 = G.find(m.get(e0).v1())->n();
    assert(G.find(m.get(e0).v2())->n() == 1);
    Number n2 = G.find(m.get(e1).v1())->n();
    assert(G.find(m.get(e1).v2())->n() == 1);
    Number n3 = G.find(m.get(e3).v1())->n();
    assert(G.find(m.get(e3).v2())->n() == 2);
    std::set<Number> s1 = {n1, n2, n3};
    std::set<Number> s2 = {4, 5, 6};
    assert(s1 == s2);

    Graph G1 = createG();
    addV(G1, 0);
    split_vertex(G1, 0, 1);
    assert(G1.order() == 0);

    Graph G2 = createG();
    addV(G2, 0);
    addV(G2, 1);
    addE(G2, Location(0, 1));
    split_vertex(G2, 0, 2);
    assert(G2.order() == 2);
    assert(G2.size() == 1);
}

void test_split_edge()
{
    Graph G(createG());
    Vertex v0 = addV(G, 0).v();
    Vertex v1 = addV(G, 1).v();
    Vertex v2 = addV(G, 2).v();
    Vertex v3 = addV(G, 3).v();
    Edge e0 = addE(G, Location(0, 1)).e();
    Edge e1 = addE(G, Location(0, 1)).e();
    Edge e2 = addE(G, Location(3, 2)).e();
    Edge e3 = addE(G, Location(0, 2)).e();
    Edge e4 = addE(G, Location(3, 3)).e();

    auto m = split_edge<HalfEdgeMapper>(G, e0, 4);

    assert(G.order() == 6);
    assert(G.size() == 6);
    assert(G[0].degree() == 3);
    assert(G[1].degree() == 2);
    assert(G[2].degree() == 2);
    assert(G[3].degree() == 3);
    assert(G[4].degree() == 1);
    assert(G[5].degree() == 1);

    assert(G[4].find(IP::n2(0))->h1() == m.get(e0.h1()));
    assert(G[5].find(IP::n2(1))->h1() == m.get(e0.h2()));
}

void test_subdivide_edge()
{
    Graph G(createG());
    Vertex v0 = addV(G, 0).v();
    Vertex v1 = addV(G, 1).v();
    Vertex v2 = addV(G, 2).v();
    Vertex v3 = addV(G, 3).v();
    Edge e0 = addE(G, Location(0, 1)).e();
    Edge e1 = addE(G, Location(0, 1)).e();
    Edge e2 = addE(G, Location(3, 2)).e();
    Edge e3 = addE(G, Location(0, 2)).e();
    Edge e4 = addE(G, Location(3, 3)).e();

    auto m = subdivide_edge<HalfEdgeMapper>(G, e1, 4);
    assert(G.order() == 5);
    assert(G.size() == 6);
    assert(G[0].degree() == 3);
    assert(G[1].degree() == 2);
    assert(G[2].degree() == 2);
    assert(G[3].degree() == 3);
    assert(G[4].degree() == 2);
    assert(m.get(e1.h1()) == G[0].find(Number(4))->e().h1());
    assert(m.get(e1.h2()) == G[1].find(Number(4))->e().h2());
}

void test_suppress_vertex()
{
    Graph G(createG());
    Vertex v0 = addV(G, 0).v();
    Vertex v1 = addV(G, 1).v();
    Vertex v2 = addV(G, 2).v();
    Vertex v3 = addV(G, 3).v();
    Edge e0 = addE(G, Location(0, 1)).e();
    Edge e1 = addE(G, Location(0, 1)).e();
    Edge e2 = addE(G, Location(3, 2)).e();
    Edge e3 = addE(G, Location(0, 2)).e();
    Edge e4 = addE(G, Location(3, 3)).e();

    auto m = suppress_vertex<HalfEdgeMapper>(G, 1);
    assert(G.order() == 3);
    assert(G.size() == 4);
    assert(G[0].degree() == 3);
    assert(G[2].degree() == 2);
    assert(G[3].degree() == 3);
    assert(G[0].contains(Number(0)));
    assert(G[3].contains(Number(3)));
    auto ii = G[0].find(Number(0));
    std::set<HalfEdge> s1 = { m.get(e1.h1()), m.get(e0.h1()) };
    std::set<HalfEdge> s2 = { ii->h1(), ii->h2() };
    assert(s1 == s2);
}

void test_contract_edge()
{
    Factory f;
    Graph G = createG(f);
    Vertex v0 = addV(G, 0, f).v();
    Vertex v1 = addV(G, 1, f).v();
    Vertex v2 = addV(G, 2, f).v();
    Vertex v3 = addV(G, 3, f).v();
    Edge e0 = addE(G, Location(0, 1), f).e();
    Edge e1 = addE(G, Location(0, 1), f).e();
    Edge e2 = addE(G, Location(3, 2), f).e();
    Edge e3 = addE(G, Location(0, 2), f).e();
    Edge e4 = addE(G, Location(3, 3), f).e();

    auto m = contract_edge<NumberMapper>(G, e1, f);
    assert(G.order() == 3);
    assert(G.size() == 4);
    assert(G[0].degree() == 3);
    assert(G[2].degree() == 2);
    assert(G[3].degree() == 3);
    assert(m.get(0) == 0);
    assert(m.get(1) == 0);
}

int main()
{
    test_identify_vertices();
    test_split_vertex();
    test_split_edge();
    test_subdivide_edge();
    test_suppress_vertex();
    test_contract_edge();
}
