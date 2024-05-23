#include "implementation.h"

#include <cassert>

#include <invariants/connectivity.hpp>
#include <io.hpp>
#include <graphs.hpp>
#include <operations/basic.hpp>
#include <operations/add_graph.hpp>


using namespace ba_graph;

int main()
{
    const int n = 11;
    Graph G(empty_graph(n));
    assert(has_cut_vertex(G));
    assert(cut_vertices(G).size() == n);

    G = createG();
    addV(G, 0);
    addV(G, 1);
    addV(G, 2);
    addV(G, 3);
    addV(G, 4);
    addV(G, 5);
    addE(G, Location(0, 1));
    addE(G, Location(1, 2));
    addE(G, Location(2, 3));
    addE(G, Location(3, 1));
    addE(G, Location(3, 4));
    addE(G, Location(4, 5));
    assert(cut_vertices(G).size() == 3);
    assert(cut_edges(G).size() == 3);

    G = create_petersen();
    assert(!has_cut_vertex(G));
    assert(cut_vertices(G).size() == 0);
    assert(!has_cut_edge(G));

    G = read_graph6_line(std::string(":EgPcgABR")); // dumbbell, v kazdej slucke vlozena dvojnasobna hrana
    assert(has_cut_vertex(G));
    assert(cut_vertices(G).size() == 2);
    assert(has_cut_edge(G));
    assert(cut_edges(G).size() == 1);
}
