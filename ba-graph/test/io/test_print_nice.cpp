#include "implementation.h"

#include <io/print_nice.hpp>

#include <cassert>
#include <sstream>

using namespace ba_graph;

int main()
{
    Graph G(createG());
    addV(G, 0);
    addV(G, 1);
    addV(G, 2);
    addE(G, Location(0, 0));
    addE(G, Location(0, 1));
    addE(G, Location(0, 1));
    Edge e02 = addE(G, Location(0, 2)).e();
    Edge e12 = addE(G, Location(1, 2)).e();

    assert(print_nice(G) == "0: 0 0 1 1 2\n1: 0 0 2\n2: 0 1\n");
    assert(print_nice_only_primary(G) == "0: 0 1 1 2\n1: 2\n2:\n");
    assert(print_nice_vertices(G) == "3 vertices: 0 1 2\n");

    Vertex v1 = createV();
    assert(print_nice(v1) == std::to_string(v1.to_int()));
    std::stringstream ss;
    ss << "(" << G[0].v().to_int() << ", " << G[0][0].e().v2().to_int() << "; " << G[0][0].e().to_int() << ")";
    assert(print_nice(G[0][0].e()) == ss.str());
    assert(print_nice(Number(5)) == "5");
    assert(print_nice(Location(5, 7, 2)) == "L(5, 7, 2)");

    assert(print_nice(std::pair(v1, e02)) == "(" + print_nice(v1) + ", " + print_nice(e02) + ")");
    assert(print_nice(std::vector({v1, v1})) == "[" + print_nice(v1) + " " + print_nice(v1) + "]");

    VertexLabeling<int> vl(-2);
    vl.set(G[1].v(), 4);
    assert(print_nice(vl, G) == "[1: -2, 2: 4, 3: -2]");

    EdgeLabeling<int> el(-2);
    el.set(G[2][1].e(), 4);
    assert(print_nice(el, G) == "[4: -2, 5: -2, 6: -2, 7: -2, 8: 4]");

    NumberLabeling<int> nl(-2);
    nl.set(1, 4);
    assert(print_nice(nl, G) == "[0: -2, 1: 4, 2: -2]");

    GraphNumberLabeling<int> gnl(-2, G);
    gnl.set(1, 4);
    assert(print_nice(gnl, G) == "[0: -2, 1: 4, 2: -2]");
}
