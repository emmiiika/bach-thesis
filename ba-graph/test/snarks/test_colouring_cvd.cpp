#include <cassert>
#include <iostream>
#include <sstream>

#include "implementation.h"
#include <graphs.hpp>
#include <io/graph6.hpp>
#include <snarks/colouring.hpp>
#include <snarks/colouring_cvd.hpp>

using namespace ba_graph;

const std::string correctAnswer = R"(
1
11
11111
0111111111111111110
0000111111111111111111111111111111111111111111111111111111111101111111111111111111111
00000000000000000000000111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000011111111111111001111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111101111111111101111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
)";

int main()
{
    Graph G(complete_graph(4));
    assert(is3EdgeColourable_cvd(G) == true);
    assert(is_colourable<HeuristicsColouriser>(G) == true);

    G = std::move(create_petersen());
    assert(is3EdgeColourable_cvd(G) == false);
    assert(is_colourable<HeuristicsColouriser>(G) == false);

    std::stringstream output;
    output << std::endl;
    for (int i = 4; i <= 14; i += 2) {
        std::string filename = "../../resources/graphs/"
                + internal::stored_cubic_path(1, 3, i);
        Factory f;
        auto graphs = read_graph6_file(filename, f).graphs();
        for (Graph &G : graphs) {
            output << is3EdgeColourable_cvd(G);
        }
        output << std::endl;
    }
    assert(output.str() == correctAnswer);
}
