#include <fstream>
#include <string>
#include <iostream>

#include "../grph.cpp"
#include "../convert_graphs.cpp"
#include "../helpers.cpp"
#include "../gen.cpp"

int main() {
    std::string g1 = "H@Q@YiW";
    grph graph = graph6_to_graph(g1);
    print_2d_vector(graph.adj_matrix);

    grph g2;
    g2.num_of_vertices = 1;
    g2.adj_matrix = {{0}};

    std::cout << graph_to_graph6(g2) << std::endl;

    std::cout << sparse6_to_graph6(canonical_form(graph6_to_graph("U???WCC@?O`C?O?OO?`?OOC?O_?_Aa?DA?AGG?P?"))) << std::endl;
}


