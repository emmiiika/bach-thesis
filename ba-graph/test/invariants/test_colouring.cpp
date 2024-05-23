#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "implementation.h"
#include <invariants/colouring.hpp>
#include <invariants/degree.hpp>
#include <io/graph6.hpp>
#include <graphs.hpp>

using namespace ba_graph;

const std::string correctAnswer = R"(
1
11
11111
0111111111111111110
0000111111111111111111111111111111111111111111111111111111111101111111111111111111111
00000000000000000000000111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000011111111111111001111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111101111111111101111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
)";

bool countColourings(std::map<Vertex, int>, int *c)
{
    ++*c;
    return true;
}

bool countEdgeColourings(std::map<Edge, int>, int *c)
{
    ++*c;
    return true;
}

bool test_removal(const Graph &G)
{
    std::map<Edge, int> precolouring;
    for (auto &e : G.list(RP::all(), IP::primary(), IT::e())) {
        precolouring[e] = -1;
    }

    for (auto &v : G) {
        if (v.degree() == 1)
            precolouring[v.find(IP::all())->e()] = 0;
    }
    assert(!is_edge_colourable_basic(G, 3, precolouring));
    bool isColourableAfterRemoval = false;
    for (auto &v : G) {
        if (v.degree() == 1) continue;
        Factory f;
        auto [H, m] = copy_other_factory<EdgeMapper>(G, f);
        std::map<Edge, int> prec2;
        for (auto &e : G.list(RP::all(), IP::primary(), IT::e()))
            prec2[m.get(e)] = precolouring[e];
        deleteV(H, v.n(), f);
        if (is_edge_colourable_basic(H, 3, prec2))
            isColourableAfterRemoval = true;
    }
    return isColourableAfterRemoval;
}

int main()
{
    Graph g0(empty_graph(0));
    int c0 = 0;
    enumerate_colourings_basic(g0, 3, countColourings, &c0);
    assert(c0 == 1);

    Graph g1(empty_graph(11));
    assert(is_colourable_basic(g1, 5));
    assert(!is_colourable_basic(g1, 0));
    assert(chromatic_number_basic(g1) == 1);

    Graph g2(empty_graph(4));
    int c2 = 0;
    enumerate_colourings_basic(g2, 3, countColourings, &c2);
    assert(c2 == 81);
    addE(g2, Location(0, 1));
    addE(g2, Location(0, 2));
    addE(g2, Location(0, 3));
    addE(g2, Location(1, 2));
    assert(chromatic_number_basic(g2) == 3);
    Graph g2lg(line_graph(g2));
    assert(chromatic_number_basic(simplify(g2lg)) == max_deg(g2));

    Graph g3(create_petersen());
    assert(chromatic_number_basic(g3) == 3);
    Graph g3lg(line_graph(g3));
    assert(chromatic_number_basic(simplify(g3lg)) == 4);

    Graph g4(complete_bipartite_graph(4, 7));
    assert(chromatic_number_basic(g4) == 2);
    assert(chromatic_index_basic(g4) == 7);

    Graph g5(complete_graph(7));
    assert(chromatic_number_basic(g5) == 7);
    assert(chromatic_index_basic(g5) == 7);

    Graph g6(create_petersen());
    add_disjoint(g6, g3, 10);
    assert(chromatic_number_basic(g6) == 3);
    assert(chromatic_index_basic(g6) == 4);
    add_disjoint(g6, g4, 20);
    assert(chromatic_number_basic(g6) == 3);
    assert(chromatic_index_basic(g6) == 7);

    Graph a1(complete_graph(4));
    assert(is_edge_colourable_basic(a1, 3));
    std::map<Edge, int> precolouring;
    Edge e01 = a1[0].find(Location(0, 1))->e();
    Edge e02 = a1[0].find(Location(0, 2))->e();
    precolouring[e01] = 2;
    precolouring[e02] = 2;
    assert(!is_edge_colourable_basic(a1, 3, precolouring));
    precolouring[e02] = 0;
    assert(is_edge_colourable_basic(a1, 3, precolouring));
    int ca1 = 0;
    assert(enumerate_edge_colourings_basic(a1, 3, countEdgeColourings, &ca1, precolouring));
    assert(ca1 == 1);

    Graph a11(circuit(5));
    std::map<Vertex, int> precolouring11;
    int ca11 = 0;
    assert(enumerate_colourings_basic(a11, 3, countColourings, &ca11, precolouring11));
    assert(ca11 == 30);
    precolouring11[a11[1].v()] = 1;
    ca11 = 0;
    assert(enumerate_colourings_basic(a11, 3, countColourings, &ca11, precolouring11));
    assert(ca11 == 10);
    precolouring11[a11[2].v()] = 2;
    ca11 = 0;
    assert(enumerate_colourings_basic(a11, 3, countColourings, &ca11, precolouring11));
    assert(ca11 == 5);

    Graph a12(circuit(5));
    std::map<Edge, int> precolouring12;
    int ca12 = 0;
    assert(enumerate_edge_colourings_basic(a12, 3, countEdgeColourings, &ca12, precolouring12));
    assert(ca12 == 30);
    precolouring12[a12[2].find(Location(2, 1))->e()] = 1;
    ca12 = 0;
    assert(enumerate_edge_colourings_basic(a12, 3, countEdgeColourings, &ca12, precolouring12));
    assert(ca12 == 10);
    precolouring12[a12[2].find(Location(2, 3))->e()] = 2;
    ca12 = 0;
    assert(enumerate_edge_colourings_basic(a12, 3, countEdgeColourings, &ca12, precolouring12));
    assert(ca12 == 5);

    Graph a2(create_petersen());
    assert(!is_edge_colourable_basic(a2, 3));
    assert(chromatic_index_basic(a2) == 4);

    Graph a3 = empty_graph(2);
    addE(a3, Location(0, 1));
    addE(a3, Location(0, 1));
    addE(a3, Location(0, 1));
    addE(a3, Location(0, 1));
    addE(a3, Location(0, 1));
    assert(chromatic_index_basic(a3) == 5);
    assert(is_edge_colourable_basic(a3, 5));
    assert(!is_edge_colourable_basic(a3, 4));
    addE(a3, Location(0, 0));
    assert(chromatic_index_basic(a3) == -1);
    assert(!is_edge_colourable_basic(a3, 5));
    assert(!is_edge_colourable_basic(a3, 20));

    std::stringstream output;
    output << std::endl;
    for (int i = 4; i <= 14; i += 2) {
        std::string filename = "../../resources/graphs/" + internal::stored_cubic_path(1, 3, i);
        Factory f;
        auto graphs = read_graph6_file(filename, f).graphs();
        for (Graph &G : graphs)
            output << is_edge_colourable_basic(G, 3);
        output << std::endl;
    }
    assert(output.str() == correctAnswer);

    Graph b1(read_graph6_line(":]dBgEfGdeIf`KiehoaGfJQknP_MoUcSnWdVmPYlXsT["));
    assert(test_removal(b1));
}
