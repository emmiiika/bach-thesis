#ifndef STAT_HELPERS
#define STAT_HELPERS

#include <fstream>
#include <string>
#include <iostream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <limits>
#include <utility>
#include <filesystem>
#include <set>
#include <invariants/colouring.hpp>
#include "../ctpl_stl.h"

void file_line_count(std::string file_name){
    int count = 0;
    std::ifstream file;
    file.open(file_name);
    if(file.is_open()){
        std::string line;
        while (getline(file, line)) {
            count++;
        }
        std::cout << "lines in " + file_name + ": " << count << std::endl;

        file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }
}

void remove_previous_file(std::string file_name){
    int result = remove(file_name.c_str());
    if (result == 0) {
        std::cout << "File deleted (" + file_name + ")" << std::endl;
    } else {
        std::cout << "No such file (" + file_name+ ")" << std::endl;
    }
}

void remove_create_directory(std::string directory){
    int result = std::filesystem::remove_all(directory);
    if(result > 0) {
        std::cout << "Folder deleted (" + directory + ")" << std::endl;
    } else {
        std::cout << "No such folder (" + directory + ")" << std::endl;
    }

    std::filesystem::create_directory(directory);
    std::cout << "Folder created (" + directory + ")" << std::endl;

}

bool check_if_present(std::unordered_set<std::string> graphs, std::string g){
    if(graphs.count(g) > 1) {
        return true;
    } else {
        return false;
    }
}

int get_num_of_vertices(std::string graph6){
    grph g = graph6_to_graph(graph6);
    return g.num_of_vertices;
}

int get_num_of_edges(std::string graph6){
    grph g = graph6_to_graph(graph6);
    std::vector<std::vector<int>> adj_mat = g.adj_matrix;

    int count = 0;
    for(int u = 0; u < g.num_of_vertices; u++){
        for(int v = u; v < g.num_of_vertices; v++){
            if(adj_mat[u][v] > 0){
                count += 1;
            }
        }
    }
    return count;
}

int bfs(grph g, int src){
    std::vector<std::vector<int>> adj_list = adj_mat_to_adj_list(g);
    std::vector<int> dist(g.num_of_vertices, -1);
    dist[src] = 0;

    std::deque<std::pair<int, int>> deque;
    deque.push_back({-1, src});

    while(!deque.empty()){
        int prev = deque.at(0).first;
        int v = deque.at(0).second;
        deque.pop_front();

        for(int neigh: adj_list[v]){
            if(dist[neigh] == -1){
                dist[neigh] = dist[v] + 1;
                deque.push_back({v, neigh});
            } else if(prev != neigh){
                return dist[v] + dist[neigh] + 1; //cycle
            }
        }
    }
    return std::numeric_limits<int>::max();
}

int get_girth(grph g){
    int min_cycle = std::numeric_limits<int>::max();
    for(int v = 0; v < g.num_of_vertices; v++){
        min_cycle = std::min(min_cycle, bfs(g, v));
    }
    return min_cycle;
}

/*
 * This function tests graph g for edge colorability.
*/
bool is_edge_colorable(grph g) {
    ba_graph::Graph G = graph_to_ba_graph(g);
    return ba_graph::is_edge_colourable_basic(G, 3);
}

std::pair<std::string, std::vector<std::string>> parse_origin_line(std::string line){
    std::string delimiter = ": ";
    std::string graph = line.substr(0, line.find(delimiter));
    std::string subgraphs = line.substr(line.find(delimiter) + delimiter.length(), line.length());

    delimiter = ", ";
    size_t pos;
    std::string token;
    std::pair<std::string, std::vector<std::string>> parsed;
    parsed.first = graph;
    parsed.second = std::vector<std::string>();
    while((pos = subgraphs.find(delimiter)) != std::string::npos){
//    while(subgraphs != ", "){
        token = subgraphs.substr(0, pos);
        parsed.second.push_back(token);
        subgraphs.erase(0, pos + delimiter.length());
    }
    return parsed;
}


/*
 *  This function sums all the times per file together and prints
 *  output to a file.
 */
void sum_times_per_file(std::string time_file, std::string out_file) {
    std::cout << std::endl << "SUMMING UP TIMES PER FILE" << std::endl;
    remove_previous_file(out_file);

    std::ofstream output_file;
    output_file.open(out_file, std::ios_base::app);
    if(output_file.is_open()){

        std::ifstream input_file;
        input_file.open(time_file);
        if(input_file.is_open()){
            int sum = 0;
            std::string line;
            while(getline(input_file, line)){
                sum += std::stoi(line);
            }

            output_file << time_file << std::endl;
            output_file << "       [ms]: " << sum << std::endl;
            output_file << "       [s]: " << sum/1000 << std::endl;
            output_file << "       [min]: " << sum/1000/60 << std::endl;
            output_file << "       [h]: " << sum/1000/60/60 << std::endl;

            input_file.close();
        }
        else{
            std::cout << "Couldn't open file (" << time_file << ")." << std::endl;
        }
        output_file.close();
    }
    else{
        std::cout << "Couldn't open file (" << out_file << ")." << std::endl;
    }
}

/*
 *  This function filters redundant graphs per file and prints
 *  an output to files. The original order of graphs is not
 *  preserved.
 */
void filter_redundant_per_file(std::string in_file, std::string out_file) {
    std::cout << std::endl << "FILTERING REDUNDANT SUBGRAPHS PER FILE" << std::endl;
    remove_previous_file(out_file);

    std::unordered_set<std::string> graphs;

    std::ifstream input_file;
    input_file.open(in_file);
    if(input_file.is_open()){
        std::string line;
        while(getline(input_file, line)){
            graphs.insert(line);
        }
        input_file.close();
    }
    else{
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::ofstream output_file;
    output_file.open(out_file);
    if(output_file.is_open()) {
        for (std::string graph: graphs) {
            output_file << graph + "\n";
        }
        output_file.close();
    }
    else{
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function filters redundant graphs per file and prints
 *  an output to files. The original order of graphs is preserved.
 */
void filter_redundant_sorted_per_file(std::string in_file, std::string out_file) {
    std::cout << std::endl << "FILTERING REDUNDANT SUBGRAPHS PER FILE SORTED" << std::endl;
    remove_previous_file(out_file);

    std::unordered_set<std::string> container;
    std::vector<std::string> graphs;
    std::ifstream input_file;
    std::ofstream output_file;
    input_file.open(in_file);
    if(input_file.is_open()){
        std::string line;
        while(getline(input_file, line)){
            bool in = check_if_present(container,line);
            if(!in) {
                graphs.push_back(line);
                container.insert(line);
            }
        }

        std::cout << "writing to " << out_file << std::endl;
        output_file.open(out_file);
        if(output_file.is_open()) {
            for (std::string graph: graphs) {
                output_file << graph + "\n";
            }
            output_file.close();
        }
        else{
            std::cout << "Couldn't open output file." << std::endl;
        }

        input_file.close();
        std::cout << "writing done" << std::endl;

    }
    else{
        std::cout << "Couldn't open input file." << std::endl;
    }
}

/*
 *  This function puts all the filtered files to one. Redundancy throughout
 *  the files is possible.
 */
void merge_filtered_files(const int SIZE, std::string in_files[], std::string out_file) {
    std::cout << std::endl << "MERGING FILTERED SUBGRAPHS INTO ONE FILE" << std::endl;

    int lines_per_all_files = 0;
    remove_previous_file(out_file);

    std::ofstream output_file;
    output_file.open(out_file);
    if (output_file.is_open()) {
        for (int i = 0; i < SIZE; i++) {
            std::string in_file = in_files[i];
            std::ifstream input_file;
            input_file.open(in_file);
            int lines_per_file = 0;
            if (input_file.is_open()) {
                std::string line;
                while (getline(input_file, line)) {
                    output_file << line + "\n";
                    lines_per_file++;
                    lines_per_all_files++;
                }
                std::cout << "lines in the file (" << in_file << "): " << lines_per_file << std::endl;
                input_file.close();
            }
            else {
                std::cout << "Couldn't open input file." << std::endl;
            }
        }
        std::cout << "File updated (" << out_file << ")." << std::endl;

        output_file.close();
        std::cout << "lines in all files together: " << lines_per_all_files << std::endl;

    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function puts all input cubic graphs into one file.
 */
void merge_original_files(const int SIZE, std::string in_files[], std::string out_file) {
    std::cout << std::endl << "MERGING ORIGINAL GRAPHS INTO ONE FILE" << std::endl;

    int lines_per_all_files = 0;
    remove_previous_file(out_file);

    std::ofstream output_file;
    output_file.open(out_file);
    if (output_file.is_open()) {
        for (int i = 0; i < SIZE; i++) {
            std::string in_file = in_files[i];
            std::ifstream input_file;
            input_file.open(in_file);
            int lines_per_file = 0;
            if (input_file.is_open()) {
                std::string line;
                while (getline(input_file, line)) {
                    output_file << line + "\n";
                    lines_per_file++;
                    lines_per_all_files++;
                }
                std::cout << "lines in file (" + in_file + "): " << lines_per_file << std::endl;
                input_file.close();
            }
            else {
                std::cout << "Couldn't open input file." << std::endl;
            }
        }
        std::cout << "File updated (" << out_file << ")." << std::endl;

        output_file.close();
        std::cout << "lines in all files together: " << lines_per_all_files << std::endl;

    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function filters redundant graphs from the file containing
 *  all generated subgraphs.
 */
void filter_merged_subgraphs(std::string in_file, std::string out_file) {
    std::cout << std::endl << "FILTERING REDUNDANT SUBGRAPHS MERGED INTO ONE FILE" << std::endl;
    remove_previous_file(out_file);

    std::unordered_set<std::string> graphs;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        while (getline(input_file, line)) {
            graphs.insert(line);
        }

        input_file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::ofstream output_file;
    output_file.open(out_file);
    if (output_file.is_open()) {
        for (std::string graph: graphs) {
            output_file << graph + "\n";
        }
        std::cout << "File updated (" << out_file << ")." << std::endl;
        output_file.close();
    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function sorts filtered graphs into separate files
 *  by the number of vertices.
 */
void sort_by_num_of_vertices(std::string in_file, std::string type) {
    std::cout << std::endl << "SORTING FILTERED SUBGRAPHS BY THE NUMBER OF VERTICES" << std::endl;

    if(type ==  "cubic") {
        remove_create_directory("iofiles/cubic-graphs/numOfVertices");
    } else if(type == "snark"){
        remove_create_directory("iofiles/snarks-graphs/snarks-numOfVertices");
    } else if(type == "small-cubic"){
        remove_create_directory("iofiles/smaller-cubic-graphs/smaller-cubic-numOfVertices");
    } else if(type == "small-snark"){
        remove_create_directory("iofiles/smaller-snarks-graphs/smaller-snarks-numOfVertices");
    }


    std::unordered_map<int, std::vector<std::string>> graphs_based_on_vertices;
    int max_num_of_vertices = -1;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        int num_of_vertices;
        while(getline(input_file, line)){
            num_of_vertices = get_num_of_vertices(line);
            max_num_of_vertices = std::max(max_num_of_vertices, num_of_vertices);
            graphs_based_on_vertices[num_of_vertices].push_back(line);
        }
        input_file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    int num_of_graphs = 0;
    std::ofstream output_file;
    for(int num_of_vertices = 1; num_of_vertices <= max_num_of_vertices; num_of_vertices++){
        int num_of_graphs_by_set = 0;
        if(graphs_based_on_vertices.find(num_of_vertices) != graphs_based_on_vertices.end()){

            std::string out_file;
            if(type ==  "cubic") {
                out_file = "iofiles/cubic-graphs/numOfVertices/cub" + std::to_string(num_of_vertices) + "-gir5-numOfVertices.g6";
            } else if(type == "snark"){
                out_file = "iofiles/snarks-graphs/snarks-numOfVertices/" + std::to_string(num_of_vertices) + "-snark-numOfVertices.g6";
            } else if(type == "small-cubic"){
                out_file = "iofiles/smaller-cubic-graphs/smaller-cubic-numOfVertices/" + std::to_string(num_of_vertices) + "-smaller-cubic-numOfVertices.g6";
            } else if(type == "small-snark"){
                out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-numOfVertices/" + std::to_string(num_of_vertices) + "-smaller-snarks-numOfVertices.g6";
            }


            output_file.open(out_file);
            if (output_file.is_open()) {
                for(std::string graph: graphs_based_on_vertices[num_of_vertices]){
                    output_file << graph + "\n";
                    num_of_graphs++;
                    num_of_graphs_by_set++;
                }
                std::cout << "number of graphs by set: " << num_of_graphs_by_set << std::endl;
                output_file.close();
            }
            else {
                std::cout << "Couldn't open output file." << std::endl;
            }
        }
    }
    std::cout << "number of all graphs: " << num_of_graphs << std::endl;
}

/*
 *  This function adds number of vertices to the file with origin of subgraphs.
 */
void add_num_of_vertices_to_origin_file(std::string in_file, std::string out_file){
    std::cout << std::endl << "ADDING NUMBER OF VERTICES TO THE ORIGIN FILE" << std::endl;
    std::vector<std::string> outputs;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        while(getline(input_file, line)){
            std::string output = "";
            std::pair<std::string, std::vector<std::string>> parsed_line = parse_origin_line(line);
            std::string graph = parsed_line.first;
            std::vector<std::string> subgraphs = parsed_line.second;

            int graph_vertices = get_num_of_vertices(graph);

            output += graph + " (";
            output += std::to_string(graph_vertices) + "): ";

            for(auto subgraph: subgraphs){
                int subgraph_vertices = get_num_of_vertices(subgraph);

                output += subgraph + " (";
                output += std::to_string(subgraph_vertices) + "), ";
            }
            outputs.push_back(output);
        }
        input_file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::ofstream output_file;
    output_file.open(out_file);
    if(output_file.is_open()){
        for(std::string output: outputs){
            output_file << output + "\n";
        }
        output_file.close();
    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function sorts filtered graphs into separate files
 *  by their girth.
 */
void sort_by_girth(std::string in_file, std::string type){
    std::cout << std::endl << "SORTING FILTERED SUBGRAPHS BY THEIR GIRTH" << std::endl;

    if(type ==  "cubic") {
        remove_create_directory("iofiles/cubic-graphs/girth");
    } else if(type == "snark"){
        remove_create_directory("iofiles/snarks-graphs/snarks-girth");
    } else if(type == "small-cubic"){
        remove_create_directory("iofiles/smaller-cubic-graphs/smaller-cubic-girth");
    } else if(type == "small-snark"){
        remove_create_directory("iofiles/smaller-snarks-graphs/smaller-snarks-girth");
    }


    std::unordered_map<int, std::unordered_set<std::string>> graphs_based_on_girth;
    int max_girth = -1;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        int girth;
        while(getline(input_file, line)){
            grph g = graph6_to_graph(line);
            girth = get_girth(g);

            if(girth != std::numeric_limits<int>::max()){
                max_girth = std::max(max_girth, girth);
                graphs_based_on_girth[girth].insert(line);
            }
            else {
                graphs_based_on_girth[0].insert(line);
            }
        }
        input_file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    int num_of_graphs = 0;
    std::ofstream output_file;
    for(int grth = 0; grth <= max_girth; grth++){
        int num_of_graphs_by_set = 0;
        if(graphs_based_on_girth.find(grth) != graphs_based_on_girth.end()){


            std::string out_file;
            if(type ==  "cubic") {
                out_file = "iofiles/cubic-graphs/girth/" + std::to_string(grth) + "-cub-girth.g6";
            } else if(type == "snark"){
                out_file = "iofiles/snarks-graphs/snarks-girth/" + std::to_string(grth) + "-snark-girth.g6";
            } else if(type == "small-cubic"){
                out_file = "iofiles/smaller-cubic-graphs/smaller-cubic-girth/" + std::to_string(grth) + "-smaller-cubic-girth.g6";
            } else if(type == "small-snark"){
                out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-girth/" + std::to_string(grth) + "-smaller-snarks-girth.g6";
            }


            output_file.open(out_file);
            if (output_file.is_open()) {
                for(std::string graph: graphs_based_on_girth[grth]){
                    output_file << graph + "\n";
                    num_of_graphs++;
                    num_of_graphs_by_set++;
                }
                std::cout << "number of graphs by set: " << num_of_graphs_by_set << std::endl;
                output_file.close();
            }
            else {
                std::cout << "Couldn't open output file." << std::endl;
            }
        }
    }
    std::cout << "number of all graphs: " << num_of_graphs << std::endl;
}

/*
 *  This function adds girth to the file with origin of subgraphs.
 */
void add_girth_to_origin_file(std::string in_file, std::string out_file){
    std::cout << std::endl << "ADDING GIRTH TO THE ORIGIN FILE" << std::endl;
    std::vector<std::string> outputs;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        while(getline(input_file, line)){
            std::string output = "";
            std::pair<std::string, std::vector<std::string>> parsed_line = parse_origin_line(line);
            std::string graph = parsed_line.first;
            std::vector<std::string> subgraphs = parsed_line.second;

            grph g = graph6_to_graph(graph);
            int graph_girth = get_girth(g);
            if(graph_girth == std::numeric_limits<int>::max()){
                graph_girth = -1;
            }

            output += graph + " (";
            output += std::to_string(graph_girth) + "): ";

            for(auto subgraph: subgraphs){
                g = graph6_to_graph(subgraph);
                int subgraph_girth = get_girth(g);

                output += subgraph + " (";
                output += std::to_string(subgraph_girth) + "), ";
            }
            outputs.push_back(output);
        }
        input_file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::ofstream output_file;
    output_file.open(out_file);
    if(output_file.is_open()){
        for(std::string output: outputs){
            output_file << output + "\n";
        }
        output_file.close();
    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function sorts filtered graphs into separate files
 *  by the number of edges.
 */
void sort_by_num_of_edges(std::string in_file, std::string type){
    std::cout << std::endl << "SORTING FILTERED SUBGRAPHS BY THE NUMBER OF EDGES" << std::endl;

    if(type ==  "cubic") {
        remove_create_directory("iofiles/cubic-graphs/edges");
    } else if(type == "snark"){
        remove_create_directory("iofiles/snarks-graphs/snarks-edges");
    } else if(type == "small-cubic"){
        remove_create_directory("iofiles/smaller-cubic-graphs/smaller-cubic-edges");
    } else if(type == "small-snark"){
        remove_create_directory("iofiles/smaller-snarks-graphs/smaller-snarks-edges");
    }

    std::unordered_map<int, std::unordered_set<std::string>> graphs_based_on_edges;
    int max_edges = -1;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        int num_of_edges;
        while(getline(input_file, line)){
            num_of_edges = get_num_of_edges(line);
            graphs_based_on_edges[num_of_edges].insert(line);

            max_edges = std::max(max_edges, num_of_edges);
        }
        input_file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    int num_of_graphs = 0;
    std::ofstream output_file;
    for(int num_of_edges = 0; num_of_edges <= max_edges; num_of_edges++){
        int num_of_graphs_by_set = 0;
        if(graphs_based_on_edges.find(num_of_edges) != graphs_based_on_edges.end()){


            std::string out_file;
            if(type ==  "cubic") {
                out_file = "iofiles/cubic-graphs/edges/" + std::to_string(num_of_edges) + "-cub-edges.g6";
            } else if(type == "snark"){
                out_file = "iofiles/snarks-graphs/snarks-edges/" + std::to_string(num_of_edges) + "-snark-edges.g6";
            } else if(type == "small-cubic"){
                out_file = "iofiles/smaller-cubic-graphs/smaller-cubic-edges/" + std::to_string(num_of_edges) + "-smaller-cubic-edges.g6";
            } else if(type == "small-snark"){
                out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-edges/" + std::to_string(num_of_edges) + "-smaller-snarks-edges.g6";
            }

            output_file.open(out_file);
            if (output_file.is_open()) {
                for(std::string graph: graphs_based_on_edges[num_of_edges]){
                    output_file << graph + "\n";
                    num_of_graphs++;
                    num_of_graphs_by_set++;
                }
                std::cout << "number of graphs by set: " << num_of_graphs_by_set << std::endl;
                output_file.close();
            }
            else {
                std::cout << "Couldn't open output file." << std::endl;
            }
        }
    }
    std::cout << "number of all graphs: " << num_of_graphs << std::endl;
}

/*
 *  This function adds number of edges to the file with origin of subgraphs.
 */
void add_num_of_edges_to_origin_file(std::string in_file, std::string out_file){
    std::cout << std::endl << "ADDING NUMBER OF EDGES TO THE ORIGIN FILE" << std::endl;
    std::vector<std::string> outputs;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        while(getline(input_file, line)){
            std::string output = "";
            std::pair<std::string, std::vector<std::string>> parsed_line = parse_origin_line(line);
            std::string graph = parsed_line.first;
            std::vector<std::string> subgraphs = parsed_line.second;

//            grph g = graph6_to_graph(graph);
            int graph_edges = get_num_of_edges(graph);

            output += graph + " (";
            output += std::to_string(graph_edges) + "): ";

            for(auto subgraph: subgraphs){
                int subgraph_edges = get_num_of_edges(subgraph);

                output += subgraph + " (";
                output += std::to_string(subgraph_edges) + "), ";
            }
            outputs.push_back(output);
        }
        input_file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::ofstream output_file;
    output_file.open(out_file);
    if(output_file.is_open()){
        for(std::string output: outputs){
            output_file << output + "\n";
        }
        output_file.close();
    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function
 */
void vertices_diff(std::string in_file, std::string type) {
    std::cout << std::endl << "ONLY THE NUMBER OF VERTICES OF SUBGRAPHS ASSIGNED TO THE ORIGIN GRAPH'S VERTICES" << std::endl;

    if (type == "cubic") {
        remove_previous_file("iofiles/cubic-graphs/vertex-diff.txt");
    } else if (type == "snark") {
        remove_previous_file("iofiles/snarks-graphs/snark-vertex-diff.txt");
    } else if (type == "small-cubic") {
        remove_previous_file("iofiles/smaller-cubic-graphs/smaller-cubic-vertex-diff.txt");
    } else if (type == "small-snark") {
        remove_previous_file("iofiles/smaller-snarks-graphs/smaller-snarks-vertex-diff.txt");
    }

    std::unordered_map<int, std::set<int>> sorted;
    int max_num_of_vertices = -1;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        while (getline(input_file, line)) {
            std::pair<std::string, std::vector<std::string>> parsed_line = parse_origin_line(line);
            std::string graph = parsed_line.first;
            std::vector<std::string> subgraphs = parsed_line.second;

            int graph_vertices = get_num_of_vertices(graph);

            max_num_of_vertices = std::max(max_num_of_vertices, graph_vertices);
            if(sorted.find(graph_vertices) == sorted.end()) {
                sorted[graph_vertices] = std::set<int>();
            }

            for (auto subgraph: subgraphs) {
                int subgraph_vertices = get_num_of_vertices(subgraph);
                sorted[graph_vertices].insert(subgraph_vertices);
            }
        }
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::string out_file;
    if (type == "cubic") {
        out_file = "iofiles/cubic-graphs/vertex-diff.txt";
    } else if (type == "snark") {
        out_file = "iofiles/snarks-graphs/snark-vertex-diff.txt";
    } else if (type == "small-cubic") {
        out_file = "iofiles/smaller-cubic-graphs/smaller-cubic-vertex-diff.txt";
    } else if (type == "small-snark") {
        out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-vertex-diff.txt";
    }

    std::ofstream output_file;
    output_file.open(out_file, std::ios_base::app);
    if (output_file.is_open()) {
        for(int i = 0; i <= max_num_of_vertices; i++){
            if(sorted.find(i) != sorted.end()){
                output_file << i << ": ";
                for(auto item: sorted[i]){
                output_file << item << ", ";
                }
                output_file << "\n";
            }
        }

        output_file.close();
    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function
 */
void edges_diff(std::string in_file, std::string type) {
    std::cout << std::endl << "ONLY THE NUMBER OF EDGES OF SUBGRAPHS ASSIGNED TO THE ORIGIN GRAPH'S VERTICES" << std::endl;

    if (type == "cubic") {
        remove_previous_file("iofiles/cubic-graphs/edges-diff.txt");
    } else if (type == "snark") {
        remove_previous_file("iofiles/snarks-graphs/snark-edges-diff.txt");
    } else if (type == "small-cubic") {
        remove_previous_file("iofiles/smaller-cubic-graphs/smaller-cubic-edges-diff.txt");
    } else if (type == "small-snark") {
        remove_previous_file("iofiles/smaller-snarks-graphs/smaller-snarks-edges-diff.txt");
    }

    std::unordered_map<int, std::set<int>> sorted;
    int max_num_of_edges = -1;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        while (getline(input_file, line)) {
            std::pair<std::string, std::vector<std::string>> parsed_line = parse_origin_line(line);
            std::string graph = parsed_line.first;
            std::vector<std::string> subgraphs = parsed_line.second;

            int graph_edges = get_num_of_edges(graph);

            max_num_of_edges = std::max(max_num_of_edges, graph_edges);
            if(sorted.find(graph_edges) == sorted.end()) {
                sorted[graph_edges] = std::set<int>();
            }

            for (auto subgraph: subgraphs) {
                int subgraph_edges = get_num_of_edges(subgraph);
                sorted[graph_edges].insert(subgraph_edges);
            }
        }
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::string out_file;
    if (type == "cubic") {
        out_file = "iofiles/cubic-graphs/edges-diff.txt";
    } else if (type == "snark") {
        out_file = "iofiles/snarks-graphs/snark-edges-diff.txt";
    } else if (type == "small-cubic") {
        out_file = "iofiles/smaller-cubic-graphs/smaller-cubic-edges-diff.txt";
    } else if (type == "small-snark") {
        out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-edges-diff.txt";
    }

    std::ofstream output_file;
    output_file.open(out_file, std::ios_base::app);
    if (output_file.is_open()) {
        for(int i = 0; i <= max_num_of_edges; i++){
            if(sorted.find(i) != sorted.end()){
                output_file << i << ": ";
                for(auto item: sorted[i]){
                    output_file << item << ", ";
                }
                output_file << "\n";
            }
        }

        output_file.close();
    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 * This function writes all colorable graphs from one group to a file.
 * */
void count_colorable_graphs_per_group(std::string in_file, std::string out_file){
    std::cout << std::endl << "FILTERING COLORABLE GRAPHS" << std::endl;

    remove_previous_file(out_file);

    std::unordered_set<std::string> colorable_graphs;

    std::ifstream input_file;
    input_file.open(in_file);
    if(input_file.is_open()){
        std::string line;
        while(getline(input_file, line)){
            grph g = graph6_to_graph(line);
            bool colorable = is_edge_colorable(g);

            if(colorable){
                colorable_graphs.insert(line);
            }
        }
    }  else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::ofstream output_file;
    output_file.open(out_file);
    if (output_file.is_open()) {
        for(std::string graph: colorable_graphs){
            output_file << graph + "\n";
        }
        output_file.close();
    } else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/*
 *  This function sorts origin graphs by the subgraphs.
 */
void sort_origin_by_subgraphs(std::string in_file, std::string type){
    std::cout << std::endl << "SORTING GRAPHS BY SUBGRAPHS" << std::endl;

    if(type ==  "cubic") {
        remove_previous_file("iofiles/cubic-graphs/subgrahps-origin.txt");
    } else if(type == "snark"){
        remove_previous_file("iofiles/snarks-graphs/subgrahps-origin.txt");
    } else if(type == "small-cubic"){
        remove_previous_file("iofiles/smaller-cubic-graphs/subgrahps-origin.txt");
    } else if(type == "small-snark"){
        remove_previous_file("iofiles/smaller-snarks-graphs/subgrahps-origin.txt");
    }


    std::map<std::string , std::set<std::string>> graphs_by_subgraphs;

    std::ifstream input_file;
    input_file.open(in_file);
    if (input_file.is_open()) {
        std::string line;
        while(getline(input_file, line)){
            std::pair<std::string, std::vector<std::string>> parsed_line = parse_origin_line(line);
            std::string graph = parsed_line.first;
            std::vector<std::string> subgraphs = parsed_line.second;

            for(auto subgraph: subgraphs){
                if(graphs_by_subgraphs.find(subgraph) != graphs_by_subgraphs.end()){
                    graphs_by_subgraphs[subgraph].insert(graph);
                } else {
                    graphs_by_subgraphs[subgraph] = {graph};
                }
            }
        }
        input_file.close();
    } else {
        std::cout << "Couldn't open input file." << std::endl;
    }

    std::ofstream output_file;
    std::string out_file;
    if(type ==  "cubic") {
        out_file = "iofiles/cubic-graphs/subgrahps-origin.txt";
    } else if(type == "snark"){
        out_file = "iofiles/snarks-graphs/subgrahps-origin.txt";
    } else if(type == "small-cubic"){
        out_file = "iofiles/smaller-cubic-graphs/subgrahps-origin.txt";
    } else if(type == "small-snark"){
        out_file = "iofiles/smaller-snarks-graphs/subgrahps-origin.txt";
    }
    output_file.open(out_file);
    if (output_file.is_open()) {
        for(auto elem: graphs_by_subgraphs){
            std::string output = "";
            output += elem.first + "(" + std::to_string(elem.second.size()) + "): ";
            for(std::string subgraph: elem.second){
                output += subgraph + ", ";
            }
            output += "\n";

            output_file << output;
        }
        output_file.close();
    }
    else {
        std::cout << "Couldn't open output file." << std::endl;
    }
}

/* To run this file time and subgraph files must be generated. */
void get_stats(std::string type) {
    int SIZE;

    std::vector<std::string> orders;

    std::string pre_time;
    std::string post_time;
    std::string pre_out_time;
    std::string post_out_time;

    std::string pre_noRedun;
    std::string post_noRedun;
    std::string pre_out_noRedun;
    std::string post_out_noRedun;

    std::string pre_noRedunSorted;
    std::string post_noRedunSorted;
    std::string pre_out_noRedunSorted;
    std::string post_out_noRedunSorted;

    std::string pre_all_in_one;
    std::string post_all_in_one;
    std::string out_all_in_one;

    std::string pre_all_orig_in_one;
    std::string post_all_orig_in_one;
    std::string out_all_orig_in_one;

    std::string filter_merged_subg;
    std::string out_filter_merged_subg;

    std::string pre_count_colorable;
    std::string post_count_colorable;
    std::string out_pre_count_colorable;
    std::string out_post_count_colorable;

    std::string vertices_origin;
    std::string out_vertices_origin;

    std::string girth_origin;
    std::string out_girth_origin;

    std::string edges_origin;
    std::string out_edges_origin;

    std::string vertices_origin_diff;
    std::string edges_origin_diff;

    std::string graphs_by_subgraphs;

    if (type == "cubic") {
        orders = {"10", "12", "14", "16", "18", "20"};
        SIZE = 6;

        pre_time = "iofiles/cubic-graphs/times/cub";
        post_time = "-gir5-time.g6";
        pre_out_time = "iofiles/cubic-graphs/times/cub";
        post_out_time = "-gir5-time-summary.txt";

        pre_noRedun = "iofiles/cubic-graphs/subgraphs/cub";
        post_noRedun = "-gir5-subgraphs.g6";
        pre_out_noRedun = "iofiles/cubic-graphs/redundancyFree/cub";
        post_out_noRedun = "-gir5-noRedun.g6";

        pre_noRedunSorted = pre_noRedun;
        post_noRedunSorted = post_noRedun;
        pre_out_noRedunSorted = "iofiles/cubic-graphs/redundancyFreeSorted/cub";
        post_out_noRedunSorted = "-gir5-noRedunSorted.g6";

        pre_all_in_one = pre_out_noRedun;
        post_all_in_one = post_out_noRedun;
        out_all_in_one = "iofiles/cubic-graphs/general/cub-gir5-allInOne.g6";

        pre_all_orig_in_one = "iofiles/cubic-graphs/cubic/cub";
        post_all_orig_in_one = "-gir5.g6";
        out_all_orig_in_one = "iofiles/cubic-graphs/general/cub-gir5-allGraphsInOne.g6";

        filter_merged_subg = out_all_in_one;
        out_filter_merged_subg = "iofiles/cubic-graphs/general/cub-gir5-noRedunAllInOne.g6";

        pre_count_colorable = "iofiles/cubic-graphs/cubic/cub";
        post_count_colorable = "-gir5.g6";
        out_pre_count_colorable = "iofiles/cubic-graphs/colorable/cub";
        out_post_count_colorable = "-gir5-colorable.g6";

        vertices_origin = "iofiles/cubic-graphs/origin.txt";
        out_vertices_origin = "iofiles/cubic-graphs/origin-vertices.txt";

        girth_origin = vertices_origin;
        out_girth_origin = "iofiles/cubic-graphs/origin-girth.txt";

        edges_origin = girth_origin;
        out_edges_origin = "iofiles/cubic-graphs/origin-edges.txt";

        vertices_origin_diff = edges_origin;
        edges_origin_diff = edges_origin;

        graphs_by_subgraphs = vertices_origin;

    } else if (type == "snark") {
        orders = {"10", "18", "20", "22", "24", "26", "28"};
        SIZE = 6;

        pre_time = "iofiles/snarks-graphs/snarks-times/Generated_graphs.";
        post_time = ".05.sn.cyc4-time.g6";
        pre_out_time = "iofiles/snarks-graphs/snarks-times/";
        post_out_time = "-snark-time-summary.txt";

        pre_noRedun = "iofiles/snarks-graphs/snarks-subgraphs/Generated_graphs.";
        post_noRedun = ".05.sn.cyc4-subgraph.g6";
        pre_out_noRedun = "iofiles/snarks-graphs/snarks-redundancyFree/";
        post_out_noRedun = "-snark-subgraphs-noRedun.txt";

        pre_noRedunSorted = pre_noRedun;
        post_noRedunSorted = post_noRedun;
        pre_out_noRedunSorted = "iofiles/snarks-graphs/snarks-redundancyFreeSorted/";
        post_out_noRedunSorted = "-snark-subgraphs-noRedunSorted.txt";

        pre_all_in_one = pre_out_noRedun;
        post_all_in_one = post_out_noRedun;
        out_all_in_one = "iofiles/snarks-graphs/snarks-general/snark-allInOne.g6";

        pre_all_orig_in_one = "iofiles/snarks-graphs/snarks/Generated_graphs.";
        post_all_orig_in_one = ".05.sn.cyc4.g6";
        out_all_orig_in_one = "iofiles/snarks-graphs/snarks-general/snark-allGraphsInOne.g6";

        filter_merged_subg = out_all_in_one;
        out_filter_merged_subg = "iofiles/snarks-graphs/snarks-general/snark-noRedunAllInOne.g6";

        pre_count_colorable = "iofiles/snarks-graphs/snarks/Generated_graphs.";
        post_count_colorable = ".05.sn.cyc4.g6";
        out_pre_count_colorable = "iofiles/snarks-graphs/snarks-colorable/Generated_graphs.";
        out_post_count_colorable = ".05.sn.cyc4-colorable.g6";

        vertices_origin = "iofiles/snarks-graphs/origin.txt";
        out_vertices_origin = "iofiles/snarks-graphs/origin-vertices.txt";

        girth_origin = vertices_origin;
        out_girth_origin = "iofiles/snarks-graphs/origin-girth.txt";

        edges_origin = girth_origin;
        out_edges_origin = "iofiles/snarks-graphs/origin-edges.txt";

        vertices_origin_diff = edges_origin;
        edges_origin_diff = edges_origin;

        graphs_by_subgraphs = vertices_origin;


    } else if (type == "small-cubic") {
        orders = {"6", "8", "10", "12", "14", "16", "18", "20"};
        SIZE = 8;

        pre_time = "iofiles/smaller-cubic-graphs/smaller-cubic-times/Generated_graphs.";
        post_time = ".04-time.g6";
        pre_out_time = "iofiles/smaller-cubic-graphs/smaller-cubic-times/";
        post_out_time = "-small-cubic-time-summary.txt";

        pre_noRedun = "iofiles/smaller-cubic-graphs/smaller-cubic-subgraphs/Generated_graphs.";
        post_noRedun = ".04-subgraph.g6";
        pre_out_noRedun = "iofiles/smaller-cubic-graphs/smaller-cubic-redundancyFree/";
        post_out_noRedun = "-small-cubic-subgraphs-noRedun.txt";

        pre_noRedunSorted = pre_noRedun;
        post_noRedunSorted = post_noRedun;
        pre_out_noRedunSorted = "iofiles/smaller-cubic-graphs/smaller-cubic-redundancyFreeSorted/";
        post_out_noRedunSorted = "-small-cubic-subgraphs-noRedunSorted.txt";

        pre_all_in_one = pre_out_noRedun;
        post_all_in_one = post_out_noRedun;
        out_all_in_one = "iofiles/smaller-cubic-graphs/smaller-cubic-general/small-cubic-allInOne.g6";

        pre_all_orig_in_one = "iofiles/smaller-cubic-graphs/smaller-cubic/Generated_graphs.";
        post_all_orig_in_one = ".04.g6";
        out_all_orig_in_one = "iofiles/smaller-cubic-graphs/smaller-cubic-general/small-cubic-allGraphsInOne.g6";

        filter_merged_subg = out_all_in_one;
        out_filter_merged_subg = "iofiles/smaller-cubic-graphs/smaller-cubic-general/small-cubic-noRedunAllInOne.g6";

        pre_count_colorable = "iofiles/smaller-cubic-graphs/smaller-cubic/Generated_graphs.";
        post_count_colorable = ".04.g6";
        out_pre_count_colorable = "iofiles/smaller-cubic-graphs/smaller-cubic-colorable/Generated_graphs.";
        out_post_count_colorable = ".04-colorable.g6";

        vertices_origin = "iofiles/smaller-cubic-graphs/origin.txt";
        out_vertices_origin = "iofiles/smaller-cubic-graphs/origin-vertices.txt";

        girth_origin = vertices_origin;
        out_girth_origin = "iofiles/smaller-cubic-graphs/origin-girth.txt";

        edges_origin = vertices_origin;
        out_edges_origin = "iofiles/smaller-cubic-graphs/origin-edges.txt";

        vertices_origin_diff = edges_origin;
        edges_origin_diff = edges_origin;

        graphs_by_subgraphs = vertices_origin;

    } else if(type == "small-snark"){
        orders = {"10", "18", "20", "22", "24"};
        SIZE = 5;

        pre_time = "iofiles/smaller-snarks-graphs/smaller-snarks-times/Generated_graphs.";
        post_time = ".04.sn.cyc4-time.g6";
        pre_out_time = "iofiles/smaller-snarks-graphs/smaller-snarks-times/";
        post_out_time = "-small-snark-time-summary.txt";

        pre_noRedun = "iofiles/smaller-snarks-graphs/smaller-snarks-subgraphs/Generated_graphs.";
        post_noRedun = ".04.sn.cyc4-subgraph.g6";
        pre_out_noRedun = "iofiles/smaller-snarks-graphs/smaller-snarks-redundancyFree/";
        post_out_noRedun = "-small-snark-subgraphs-noRedun.txt";

        pre_noRedunSorted = pre_noRedun;
        post_noRedunSorted = post_noRedun;
        pre_out_noRedunSorted = "iofiles/smaller-snarks-graphs/smaller-snarks-redundancyFreeSorted/";
        post_out_noRedunSorted = "-small-snark-subgraphs-noRedunSorted.txt";

        pre_all_in_one = pre_out_noRedun;
        post_all_in_one = post_out_noRedun;
        out_all_in_one = "iofiles/smaller-snarks-graphs/smaller-snarks-general/small-snark-allInOne.g6";

        pre_all_orig_in_one = "iofiles/smaller-snarks-graphs/smaller-snarks/Generated_graphs.";
        post_all_orig_in_one = ".04.sn.cyc4.g6";
        out_all_orig_in_one = "iofiles/smaller-snarks-graphs/smaller-snarks-general/small-snark-allGraphsInOne.g6";

        filter_merged_subg = out_all_in_one;
        out_filter_merged_subg = "iofiles/smaller-snarks-graphs/smaller-snarks-general/small-snark-noRedunAllInOne.g6";

        pre_count_colorable = "iofiles/smaller-snarks-graphs/smaller-snarks/Generated_graphs.";
        post_count_colorable = ".04.sn.cyc4.g6";
        out_pre_count_colorable = "iofiles/smaller-snarks-graphs/smaller-snarks-colorable/Generated_graphs.";
        out_post_count_colorable = ".04.sn.cyc4-colorable.g6";

        vertices_origin = "iofiles/smaller-snarks-graphs/origin.txt";
        out_vertices_origin = "iofiles/smaller-snarks-graphs/origin-vertices.txt";

        girth_origin = vertices_origin;
        out_girth_origin = "iofiles/smaller-snarks-graphs/origin-girth.txt";

        edges_origin = vertices_origin;
        out_edges_origin = "iofiles/smaller-snarks-graphs/origin-edges.txt";

        vertices_origin_diff = edges_origin;
        edges_origin_diff = edges_origin;

        graphs_by_subgraphs = vertices_origin;

    }

    std::string in_files_filtered[SIZE];
    std::string in_files_original[SIZE];
    for (int i = 0; i < SIZE; i++) {
        std::string order = orders[i];
        sum_times_per_file(pre_time + order + post_time, pre_out_time + order + post_out_time);
        filter_redundant_per_file(pre_noRedun + order + post_noRedun, pre_out_noRedun + order + post_out_noRedun);
        filter_redundant_sorted_per_file(pre_noRedunSorted + order + post_noRedunSorted,
                                         pre_out_noRedunSorted + order + post_out_noRedunSorted);

        in_files_filtered[i] = pre_all_in_one + order + post_all_in_one;
        in_files_original[i] = pre_all_orig_in_one + order + post_all_orig_in_one;

    }
    merge_filtered_files(SIZE, in_files_filtered, out_all_in_one);
    merge_original_files(SIZE, in_files_original, out_all_orig_in_one);

    for (int i = 0; i < SIZE; i++) {
        std::string order = orders[i];
        filter_merged_subgraphs(filter_merged_subg, out_filter_merged_subg);
    }

    for (int i = 0; i < SIZE; i++) {
        std::string order = orders[i];
        count_colorable_graphs_per_group(pre_count_colorable + order + post_count_colorable,
                                         out_pre_count_colorable + order + out_post_count_colorable);
    }

    sort_by_num_of_vertices(out_filter_merged_subg, type);
    add_num_of_vertices_to_origin_file(vertices_origin, out_vertices_origin);

    sort_by_girth(out_filter_merged_subg, type);
    add_girth_to_origin_file(girth_origin, out_girth_origin);

    sort_by_num_of_edges(out_filter_merged_subg, type);
    add_num_of_edges_to_origin_file(edges_origin, out_edges_origin);

    vertices_diff(vertices_origin_diff, type);
    edges_diff(edges_origin_diff, type);

    sort_origin_by_subgraphs(graphs_by_subgraphs, type);
}

#endif