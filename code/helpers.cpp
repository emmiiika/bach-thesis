#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

#include "convert_graphs.cpp"
#include "grph.cpp"
#include "timer.cpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_set>
#include <iomanip>


//#include "nauty2_8_8/gtools.h"


/*
 * Helper function for printing vectors.
 */
void print_vector(std::vector<int> v, std::string msg) {
    if (v.size() != 0) {
        if (msg.size() != 0) {
            std::cout << msg << " [";
        } else {
            std::cout << "[";
        }
        for (int i = 0; i < v.size(); i++) {
            if (i != v.size() - 1) {
                std::cout << v[i] << ", ";
            } else {
                std::cout << v[i] << "]" << std::endl;
            }
        }
    } else { std::cout << "[]" << std::endl; }
}

/*
 * Helper function for printing vectors.
 */
void print_vector(std::unordered_set<std::string> v, std::string msg) {
    if (v.size() != 0) {
        if (msg.size() != 0) {
            std::cout << msg << " [";
        } else {
            std::cout << "[";
        }
        for (auto it = v.begin(); it != v.end();) {
            std::cout << *it;
            if (++it != v.end()) {
                 std::cout << ", ";
            } else {
                std::cout << "]" << std::endl;
            }
        }
    } else { std::cout << "[]" << std::endl; }
}

/*
 * Helper function for printing matrices.
 */
void print_2d_vector(std::vector<std::vector<int>> v) {
//    std::cout << "[";
    for (int i = 0; i < v.size(); i++) {
        if (i != v.size() - 1) {
            print_vector(v[i], "");
//            std::cout << ", ";
        } else {
            print_vector(v[i], "");
//            std::cout << "]" << std::endl;
        }
    }
    std::cout << std::endl;
}

/*
 * Helper function for copying adj. matrix from [0][0] to [iMax][jMax]
 */
void
copy_matrix(std::vector<std::vector<int>> from_matrix, std::vector<std::vector<int>> &to_matrix, int iMax, int jMax) {
    for (int i = 0; i < iMax; i++) {
        for (int j = i; j < jMax; j++) {
            to_matrix[i][j] += from_matrix[i][j];
            if (i != j) {
                to_matrix[j][i] += from_matrix[j][i];
            }
        }
    }
}

/*
 * Helper function for copying adj. matrix from [iStart][jStart] to [iMax][jMax]
 */
void
copy_matrix(std::vector<std::vector<int>> from_matrix, std::vector<std::vector<int>> &to_matrix, int iStart, int jStart,
            int iMax, int jMax) {
    for (int i = iStart; i < iMax; i++) {
        for (int j = i; j < jMax; j++) {
            to_matrix[i][j] += from_matrix[i - iStart][j - jStart];
            if (i != j) {
                to_matrix[j][i] += from_matrix[j - jStart][i - iStart];
            }

        }
    }
}

/*
 * Helper function for copying adj. matrix from [iStart][jStart] to [iMax][jMax]
 */
void
copy_matrix(std::vector<std::vector<int>> from_matrix, std::vector<std::vector<int>> &to_matrix, int iStart, int jStart,
            int iMax, int jMax, int skipU, int skipV) {
    int iOffset = 0, jOffset = 0;
    for (int i = iStart; i < iMax; i++) {
        for (int j = jStart; j < jMax; j++) {
            if ((i != skipU && j != skipV) && (i != skipV && j != skipU)) {
                to_matrix[i - iOffset][j - jOffset] += from_matrix[i - iStart][j - jStart];
            }
            if ((j == skipV) || (j == skipU)) {
                jOffset++;
            }
        }
        if ((i == skipV) || (i == skipU)) {
            iOffset++;
        }
        jOffset = 0;
    }
}

/*
 * Helper function for printing tests.
 */
void print_function_test(grph function(grph, int, int), grph g, int u, int v) {
    grph ret = function(g, u, v);
    std::cout << "Number of vertices: " << ret.num_of_vertices << std::endl;
    print_2d_vector(ret.adj_matrix);
}

/*
 * Helper function for printing tests.
 */
void print_function_test(grph function(grph, int), grph g, int v) {
    grph ret = function(g, v);
    std::cout << "Number of vertices: " << ret.num_of_vertices << std::endl;
    print_2d_vector(ret.adj_matrix);
}

/*
 * Helper function for printing tests.
 */
void
print_function_test(grph function(grph, int, int, int, int), grph g, int u1, int v1, int u2,
                    int v2) {
    grph ret = function(g, u1, v1, u2, v2);
    std::cout << "Number of vertices: " << ret.num_of_vertices << std::endl;
    print_2d_vector(ret.adj_matrix);
}

/*
 * Helper function for printing tests.
 */
void print_function_test(grph function(grph, grph, int, int, int, int), grph g1,
                         grph g2, int u1, int v1, int u2,
                         int v2) {
    grph ret = function(g1, g2, u1, v1, u2, v2);
    std::cout << "Number of vertices: " << ret.num_of_vertices << std::endl;
    print_2d_vector(ret.adj_matrix);
}

/*
 * Helper function for printing tests.
 */
void print_function_test(grph function(grph, grph, int, int), grph g1, grph g2,
                         int u, int v) {
    grph ret = function(g1, g2, u, v);
    std::cout << "Number of vertices: " << ret.num_of_vertices << std::endl;
    print_2d_vector(ret.adj_matrix);
}

/*
 * Helper function for printing tests.
 */
void print_function_test(bool function(grph, grph), grph g1, grph g2) {
    bool ret = function(g1, g2);
    std::cout << ret << std::endl;
}

/*
 * Helper function for printing tests.
 */
void print_function_test(bool function(grph), grph g) {
    bool ret = function(g);
    std::cout << ret << std::endl;
}

/*
 * Helper function for printing tests.
 */
void print_function_test(grph function(std::string), std::string s) {
    grph ret = function(s);
    std::cout << "Number of vertices: " << ret.num_of_vertices << std::endl;
    print_2d_vector(ret.adj_matrix);
}

/*
 * Prints a line of = of a given length.
 */
void print_separator(int length){
    std::cout << std::endl;
    for(int i=0; i<length; i++){
        std::cout << "=";
    }
    std::cout << std::endl;
}

bool write_subgraphs_to_file(std::string file_name, std::unordered_set<std::string> subgraphs){
    std::ofstream file;
    file.open(file_name, std::ios_base::app);
    if (file.is_open()){
        for(std::string elem: subgraphs){
//            std::cout << elem << std::endl;

            std::string subgraph = sparse6_to_graph6(elem) + "\n";
//            std::cout << subgraph << std::endl;
            file << subgraph;
        }
        file.close();
        std::cout << "      File updated." << std::endl;

        return true;
    }
    else{
        return false;
    }
}

bool write_subgraphs_to_file(std::string file_name, std::unordered_set<std::string> subgraphs, std::string msg){
    std::ofstream file;
    file.open(file_name, std::ios_base::app);
    if (file.is_open()){
        for(std::string elem: subgraphs){
//            std::cout << elem << std::endl;

            std::string subgraph = sparse6_to_graph6(elem) + "\n";
//            std::cout << subgraph << std::endl;
            file << subgraph;
        }
        file.close();
        std::cout << "      File updated." << std::endl;

        return true;
    }
    else{
        return false;
    }
}

std::unordered_set<int> find_disconnected_vertices(grph g){
    std::unordered_set<int> disc_vertices;
    for(int i=0; i<g.num_of_vertices; i++){
        bool disconnected = true;
        for(int j=0; j<g.num_of_vertices; j++){
            if(g.adj_matrix[i][j] != 0) {
                disconnected = false;
            }
        }
        if(disconnected){
            disc_vertices.insert(i);
        }
    }
    return disc_vertices;
}

grph remove_disconnected_vertex(grph g){
    std::unordered_set<int> disc_vertices = find_disconnected_vertices(g);
//    std::cout << disc_vertices.size() << std::endl;
    if(disc_vertices.size() != 0){

//        for(auto v: disc_vertices){
//            std::cout << v << std::endl;
//        }

        grph mod_g;
        int num_of_vertices = g.num_of_vertices - disc_vertices.size();
        std::vector<std::vector<int>> adj_mat(num_of_vertices, std::vector<int>(num_of_vertices, 0));

        int iOffset = 0, jOffset = 0;
        for (int i = 0; i < g.num_of_vertices; i++) {
            for (int j = 0; j < g.num_of_vertices; j++) {
                if(disc_vertices.count(i) == 0 && disc_vertices.count(j) == 0){
                    adj_mat[i - iOffset][j - jOffset] = g.adj_matrix[i][j];
                }
                if(disc_vertices.count(j) != 0){
                    jOffset++;
                }
            }
            if(disc_vertices.count(i) != 0){
                iOffset++;
            }
            jOffset = 0;
        }

        mod_g.num_of_vertices = num_of_vertices;
        mod_g.adj_matrix = adj_mat;

        return mod_g;
    }
    else{
        return g;
    }
}

bool write_time_to_file(std::string file_name, Timer timer){
    std::ofstream file;
    file.open(file_name, std::ios_base::app);
    if (file.is_open()){
        file << std::fixed << std::setprecision (0) << timer.elapsedMilliseconds();
        file << "\n";
        file.close();
        std::cout << "      Time-file updated." << std::endl;

        return true;
    }
    else{
        return false;
    }
}

#endif