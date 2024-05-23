#ifndef GRAPH
#define GRAPH

#include <vector>

struct grph {
    int num_of_vertices;
    std::vector<std::vector<int>> adj_matrix;
};

#endif