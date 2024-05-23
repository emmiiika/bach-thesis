#ifndef CONVERT_GRAPHS
#define CONVERT_GRAPHS

#include <impl/basic/include.hpp>
#include <io/print_nice.hpp>
#include <string>
#include <vector>
#include "grph.cpp"
#include <io/graph6.hpp>

// g++ gen.cpp -o gen -std=c++17 -fconcepts -I"../ba-graph/include"

ba_graph::Graph graph_to_ba_graph(grph g) {
    int num_of_vertices = g.num_of_vertices;
    std::vector<std::vector<int>> adj_mat = g.adj_matrix;

    ba_graph::Graph G(ba_graph::createG());
    std::vector<ba_graph::Vertex> vertices;
    for (int i = 0; i < num_of_vertices; i++) {
        ba_graph::Vertex v = ba_graph::addV(G, i).v();
        vertices.push_back(v);
    }

    for (int i = 0; i < num_of_vertices; i++) {
        for (int j = i; j < num_of_vertices; j++) {
            for (int k = 0; k < adj_mat[i][j]; k++) {
                ba_graph::addE(G, vertices[i], vertices[j]);

            }
        }
    }
    return G;
}

grph ba_graph_to_graph(ba_graph::Graph &G) {
    grph g;
    g.num_of_vertices = G.order();
    std::vector<std::vector<int>> adj_mat(g.num_of_vertices, std::vector<int>(g.num_of_vertices, 0));
    for (auto it = G.begin(); it != G.end(); it++) {
        int vertex = it->n().to_int();
        for (auto neighbour: it->neighbours()) {
            if (vertex < neighbour.to_int()) {
                adj_mat[vertex][neighbour.to_int()] += 1;

                if (vertex != neighbour.to_int()) {
                    adj_mat[neighbour.to_int()][vertex] += 1;
                }
            }
        }
    }
    g.adj_matrix = adj_mat;
    return g;

}

std::string graph_to_sparse6(grph g, bool include_new_line = false) {
    ba_graph::Graph G = graph_to_ba_graph(g);
    std::string sparse6_string = ba_graph::write_sparse6(G, include_new_line);

    return sparse6_string;
}

std::string graph_to_graph6(grph g, bool include_new_line = false) {
    ba_graph::Graph G = graph_to_ba_graph(g);
//    std::cout << "graph_to_graph6" << std::endl;
//    std::cout << ba_graph::print_nice(G) << std::endl;
    std::string graph6_string = ba_graph::write_graph6(G, include_new_line);

    return graph6_string;
}

grph sparse6_to_graph(std::string sparse6_string) {
    ba_graph::Graph G(ba_graph::read_sparse6(sparse6_string.begin(), sparse6_string.end()));
    grph g = ba_graph_to_graph(G);
    return g;
}

grph graph6_to_graph(std::string graph6_string) {
    ba_graph::Graph G(ba_graph::read_graph6(graph6_string.begin(), graph6_string.end()));
//    std::cout << "graph6_to_graph " <<graph6_string<< std::endl;
//    std::cout << ba_graph::print_nice(G) << std::endl;
    grph g = ba_graph_to_graph(G);
    return g;
}

std::string sparse6_to_graph6(std::string sparse6_string) {
    grph g = sparse6_to_graph(sparse6_string);
    std::string graph6 = graph_to_graph6(g, false);
    return graph6;
}

std::string graph6_to_sparse6(std::string graph6_string) {
    grph g = graph6_to_graph(graph6_string);
    std::string sparse6 = graph_to_sparse6(g, false);
    return sparse6;
}

std::vector<std::vector<int>> adj_mat_to_adj_list(grph g){
    std::vector<std::vector<int>> adj_list(g.num_of_vertices, std::vector<int>());

    for(int i = 0; i < g.num_of_vertices; i++){
        for(int j = 0; j < g.num_of_vertices; j++){
            if(g.adj_matrix[i][j] != 0){
                adj_list[i].push_back(j);
            }
        }
    }
    return adj_list;
}

#endif

