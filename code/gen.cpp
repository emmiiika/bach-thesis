#include "grph.cpp"
#include "convert_graphs.cpp"
#include "helpers.cpp"
#include "timer.cpp"
#include "statistics/stat_helpers.cpp"

#include <algorithms/isomorphism/isomorphism_nauty.hpp>
#include <invariants/colouring.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <unordered_map>
#include "ctpl_stl.h"

/*
 * This function removes edge uv from graph g.
*/
grph remove_edge(grph g, int u, int v) {
    grph mod_g;
    mod_g.num_of_vertices = g.num_of_vertices;
    std::vector<std::vector<int>> adj_mat = g.adj_matrix;
    adj_mat[u][v] -= 1;
    if (u != v) {
        adj_mat[v][u] -= 1;
    }
    mod_g.adj_matrix = adj_mat;

    return mod_g;
}

/*
 * This function tests whether graphs g1 and g2 are isomorphic.
*/
bool are_isomorphic(grph g1, grph g2) {
    ba_graph::Graph G1 = graph_to_ba_graph(g1);
    ba_graph::Graph G2 = graph_to_ba_graph(g2);

    return are_isomorphic_nauty(G1, G2);
}

/*
 * This function finds the disconnected veritces in each subgraph and removes them.
*/
void remove_components_of_one_vertex(std::unordered_set<std::string> &subgraphs) {
    std::unordered_set<std::string> g_one_component;
    for (std::string subgraph: subgraphs) {
        grph sub_g = sparse6_to_graph(subgraph);
        grph new_sub_g = remove_disconnected_vertex(sub_g);

        std::string new_subgraph = graph_to_sparse6(new_sub_g);
        g_one_component.insert(new_subgraph);
    }

    subgraphs = g_one_component;
}



/*
 * This function generates minimal subgraphs without the use of memoization.
*/
void generate_minimal_subgraphs_no_memo(grph g, std::unordered_set<std::string> &subgraphs) {
    if (!is_edge_colorable(g)) {
        std::string g_can_form = canonical_form(g);
        subgraphs.insert(g_can_form);
        for (int u = 0; u < g.num_of_vertices; u++) {
            for (int v = u; v < g.num_of_vertices; v++) {
                if (g.adj_matrix[u][v] != 0) {
                    grph new_g = remove_edge(g, u, v);
                    if (!is_edge_colorable(new_g)) {
                        if (subgraphs.count(g_can_form) == 1) {
                            subgraphs.erase(g_can_form);
                        }
                        std::string new_g_can_form = canonical_form(new_g);
                        generate_minimal_subgraphs_no_memo(new_g, subgraphs);
                    }
                }
            }
        }
    }
}

/*
 * This function generates minimal subgraphs with the use of memoization.
*/
void
generate_minimal_subgraphs(grph g, std::unordered_set<std::string> &subgraphs, std::unordered_set<std::string> &memo) {

    std::string g_can_form = canonical_form(g);
    if (memo.count(g_can_form) > 0) {
        return;
    }
    memo.insert(g_can_form);

    if (!is_edge_colorable(g)) {
        subgraphs.insert(g_can_form);
        for (int u = 0; u < g.num_of_vertices; u++) {
            for (int v = u; v < g.num_of_vertices; v++) {
                if (g.adj_matrix[u][v] != 0) {
                    grph new_g = remove_edge(g, u, v);
                    if (!is_edge_colorable(new_g)) {
                        if (subgraphs.count(g_can_form) == 1) {
                            subgraphs.erase(g_can_form);
                        }
                        std::string new_g_can_form = canonical_form(new_g);
                        generate_minimal_subgraphs(new_g, subgraphs, memo);
                    }
                }
            }
        }
    }
    return;
}

/*
 * This function generates mninimal subgraphs and remembers the origin graph of every subgraph.
*/
void generate_minimal_subgraphs_origin(grph g, std::unordered_set<std::string> &subgraphs,
                                       std::unordered_map<std::string, std::unordered_set<std::string>> &memo) {

    std::string g_can_form = canonical_form(g);
    if (memo.count(g_can_form) > 0) {
        for (auto elem: memo[g_can_form]) {
            subgraphs.insert(elem);
        }
        return;
    }
//    memo.insert(g_can_form);

    if (!is_edge_colorable(g)) {
        subgraphs.insert(g_can_form);
        for (int u = 0; u < g.num_of_vertices; u++) {
            for (int v = u; v < g.num_of_vertices; v++) {
                if (g.adj_matrix[u][v] != 0) {
                    grph new_g = remove_edge(g, u, v);
                    if (!is_edge_colorable(new_g)) {
                        if (subgraphs.count(g_can_form) == 1) {
                            subgraphs.erase(g_can_form);
                        }
//                        std::string new_g_can_form = canonical_form(new_g);
                        std::unordered_set<std::string> new_subgraphs;
                        generate_minimal_subgraphs_origin(new_g, new_subgraphs, memo);

                        for (auto elem: new_subgraphs) {
                            subgraphs.insert(elem);
                        }
                    }
                }
            }
        }
    }
    memo[g_can_form] = subgraphs;
}



/*
 * This function runs the generation algorithm without memoization and writes outputs to files.
*/
int run_no_memo(std::vector<std::string> suffixes, int suffix, int line_start, int line_end, std::string type) {
    int num_of_subgraphs = 0;
    std::unordered_map<std::string, std::unordered_set<std::string>> graphs;

    std::string subg_out_file;
    std::string time_out_file;
    if (type == "cubic") {
        subg_out_file = "iofiles/cubic-graphs/subgraphs/cub" + suffixes[suffix] + "-gir5-subgraphs.g6";
        time_out_file = "iofiles/cubic-graphs/times/cub" + suffixes[suffix] + "-gir5-time.g6";
    } else if (type == "snark") {
        subg_out_file = "iofiles/snarks-graphs/snarks-subgraphs/Generated_graphs." + suffixes[suffix] +
                        ".05.sn.cyc4-subgraph.g6";
        time_out_file =
                "iofiles/snarks-graphs/snarks-times/Generated_graphs." + suffixes[suffix] + ".05.sn.cyc4-time.g6";
    } else if (type == "small-cubic") {
        subg_out_file = "iofiles/smaller-cubic-graphs/smaller-cubic-subgraphs/Generated_graphs." + suffixes[suffix] +
                        ".04-subgraph.g6";
        time_out_file =
                "iofiles/smaller-cubic-graphs/smaller-cubic-times/Generated_graphs." + suffixes[suffix] + ".04.-time.g6";
    } else if (type == "small-snark") {
        subg_out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-subgraphs/Generated_graphs." + suffixes[suffix] +
                        ".04.sn.cyc4-subgraph.g6";
        time_out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-times/Generated_graphs." + suffixes[suffix] +
                        ".04.sn.cyc4-time.g6";
    }

    std::string file_names[] = {subg_out_file, time_out_file};
    for(std::string file_name: file_names) {
        remove_previous_file(file_name);
    }

    std::cout << std::endl;

    Timer timer;
    std::ifstream file;

    int num_of_subgraphs_per_set = 0;
    double time_per_set = 0;

    std::string graph_in_file;
    if (type == "cubic") {
        graph_in_file = "iofiles/cubic-graphs/cubic/cub" + suffixes[suffix] + "-gir5.g6";
    } else if (type == "snark") {
        graph_in_file = "iofiles/snarks-graphs/snarks/Generated_graphs." + suffixes[suffix] + ".05.sn.cyc4.g6";
    } else if (type == "small-cubic") {
        graph_in_file = "iofiles/smaller-cubic-graphs/smaller-cubic/Generated_graphs." + suffixes[suffix] + ".04.g6";
    } else if(type == "small-snark") {
        graph_in_file = "iofiles/smaller-snarks-graphs/smaller-snarks/Generated_graphs." + suffixes[suffix] + ".04.sn.cyc4.g6";
    }

    file.open(graph_in_file);
    if (file.is_open()) {
        std::cout << "  File " + graph_in_file << std::endl;
        std::string coded_graph;
        int line_counter = 0;
        for (int line = 0; line < line_end; line++) {
            std::getline(file, coded_graph);
            if (line < line_start) {
                continue;
            } else {
                line_counter++;

                auto time = std::chrono::system_clock::now();
                std::time_t start_time = std::chrono::system_clock::to_time_t(time);
                std::cout << "      " << std::ctime(&start_time);

                std::cout << "      line: " << coded_graph << std::endl;
                std::unordered_set<std::string> subgraphs;
                grph g = graph6_to_graph(coded_graph);
                std::cout << "                                          " << line + 1 << "/" << line_end << "("
                        << std::fixed << std::setprecision(2) << (double) line / (double) line_end * 100 << "%)" << std::endl;

                timer.start();
                generate_minimal_subgraphs_no_memo(g, subgraphs);
                if (!subgraphs.empty()) {
                    remove_components_of_one_vertex(subgraphs);
                }

                timer.stop();
                num_of_subgraphs_per_set += subgraphs.size();
                time_per_set += timer.elapsedMilliseconds();

                std::cout << "          subgraphs count: " << subgraphs.size() << std::endl;
                std::cout << "          time [ms]: " << std::fixed << std::setprecision(0)
                          << timer.elapsedMilliseconds() << std::endl;
                write_subgraphs_to_file(subg_out_file, subgraphs);
                write_time_to_file(time_out_file, timer);
            }
        }
        file.close();

        std::cout << std::endl << "SUBGRAPHS PER SET: " << num_of_subgraphs_per_set << " (from " << line_counter
                  << " graph(s))" << std::endl;
        std::cout << "TIME PER SET [ms]: " << time_per_set << std::endl << std::endl;

        num_of_subgraphs += num_of_subgraphs_per_set;
    } else {
        std::cout << "Couldn't open file." << std::endl;
        return -1;
    }

    return num_of_subgraphs;
}

/*
 * This function runs the generation algorithm with memoization and writes outputs to files.
*/
int run(std::vector<std::string> suffixes, int suffix, int line_start, int line_end, std::string type) {
    int num_of_subgraphs = 0;
    std::unordered_map<std::string, std::unordered_set<std::string>> graphs;

    std::string subg_out_file;
    std::string time_out_file;
    if (type == "cubic") {
        subg_out_file = "iofiles/cubic-graphs/subgraphs/cub" + suffixes[suffix] + "-gir5-subgraphs.g6";
        time_out_file = "iofiles/cubic-graphs/times/cub" + suffixes[suffix] + "-gir5-time.g6";
    } else if (type == "snark") {
        subg_out_file = "iofiles/snarks-graphs/snarks-subgraphs/Generated_graphs." + suffixes[suffix] +
                        ".05.sn.cyc4-subgraph.g6";
        time_out_file =
                "iofiles/snarks-graphs/snarks-times/Generated_graphs." + suffixes[suffix] + ".05.sn.cyc4-time.g6";
    } else if (type == "small-cubic") {
        subg_out_file = "iofiles/smaller-cubic-graphs/smaller-cubic-subgraphs/Generated_graphs." + suffixes[suffix] +
                        ".04-subgraph.g6";
        time_out_file =
                "iofiles/smaller-cubic-graphs/smaller-cubic-times/Generated_graphs." + suffixes[suffix] + ".04.-time.g6";
    } else if(type == "small-snark") {
        subg_out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-subgraphs/Generated_graphs." + suffixes[suffix] + ".04.sn.cyc4-subgraph.g6";
        time_out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-times/Generated_graphs." + suffixes[suffix] + ".04.sn.cyc4-time.g6";
    }

    std::string file_names[] = {subg_out_file, time_out_file};
    for (std::string file_name: file_names) {
        remove_previous_file(file_name);
    }

    std::cout << std::endl;

    Timer timer;
    std::ifstream file;

    int num_of_subgraphs_per_set = 0;
    double time_per_set = 0;

    std::string graph_in_file;
    if (type == "cubic") {
        graph_in_file = "iofiles/cubic-graphs/cubic/cub" + suffixes[suffix] + "-gir5.g6";
    } else if (type == "snark") {
        graph_in_file = "iofiles/snarks-graphs/snarks/Generated_graphs." + suffixes[suffix] + ".05.sn.cyc4.g6";
    } else if (type == "small-cubic") {
        graph_in_file = "iofiles/smaller-cubic-graphs/smaller-cubic/Generated_graphs." + suffixes[suffix] + ".04.g6";
    } else if(type == "small-snark") {
        graph_in_file = "iofiles/smaller-snarks-graphs/smaller-snarks/Generated_graphs." + suffixes[suffix] + ".04.sn.cyc4.g6";
    }

    file.open(graph_in_file);
    if (file.is_open()) {
        std::cout << "  File " + graph_in_file << std::endl;
        std::string coded_graph;
        int line_counter = 0;
        std::unordered_set<std::string> memo;
        for (int line = 0; line < line_end; line++) {
            std::getline(file, coded_graph);
            if (line < line_start) {
                continue;
            } else {
                line_counter++;

                auto time = std::chrono::system_clock::now();
                std::time_t start_time = std::chrono::system_clock::to_time_t(time);
                std::cout << "      " << std::ctime(&start_time);

                std::cout << "      line: " << coded_graph << std::endl;
                std::unordered_set<std::string> subgraphs;
                grph g = graph6_to_graph(coded_graph);
                std::cout << "                                          " << line + 1 << "/" << line_end << "("
                        << std::fixed << std::setprecision(2) << (double) line / (double) line_end * 100 << "%)" << std::endl;

                timer.start();
                generate_minimal_subgraphs(g, subgraphs, memo);
                if (!subgraphs.empty()) {
                    remove_components_of_one_vertex(subgraphs);

                }

                timer.stop();
                num_of_subgraphs_per_set += subgraphs.size();
                time_per_set += timer.elapsedMilliseconds();

                std::cout << "          subgraphs count: " << subgraphs.size() << std::endl;
                std::cout << "          time [ms]: " << std::fixed << std::setprecision(0)
                          << timer.elapsedMilliseconds() << std::endl;
                write_subgraphs_to_file(subg_out_file, subgraphs);
                write_time_to_file(time_out_file, timer);
            }
        }
        file.close();

        std::cout << std::endl << "SUBGRAPHS PER SET: " << num_of_subgraphs_per_set << " (from " << line_counter
                  << " graph(s))" << std::endl;
        std::cout << "TIME PER SET [ms]: " << time_per_set << std::endl << std::endl;

        num_of_subgraphs += num_of_subgraphs_per_set;
    } else {
        std::cout << "Couldn't open file." << std::endl;
        return -1;
    }

    return num_of_subgraphs;
}

/*
 * This function
*/
//void generate_and_write(/*double &time_per_set, int &num_of_subgraphs_per_set, */std::string line, std::unordered_map<std::string, std::unordered_set<std::string>> &graphs, std::unordered_map<std::string, std::unordered_set<std::string>> &memo){
//    graphs[line] = std::unordered_set<std::string>();
//    std::unordered_set<std::string> subgraphs;
//
//    auto time = std::chrono::system_clock::now();
//    std::time_t start_time = std::chrono::system_clock::to_time_t(time);
//    std::cout << "      graph: " << line << "       at:" << std::ctime(&start_time) << std::endl;
//
//    grph g = graph6_to_graph(line);
//
//    Timer timer;
//    timer.start();
//    generate_minimal_subgraphs_origin(g, subgraphs, memo);
////    pool.push(generate_minimal_subgraphs_origin, g, subgraphs, memo);
//    if(!subgraphs.empty()){
//        remove_components_of_one_vertex(subgraphs);
//
//        for(auto subgraph: subgraphs){
//            std::string g6 = sparse6_to_graph6(subgraph);
//            graphs[line].insert(g6);
//        }
//    }
//    timer.stop();
//
////    num_of_subgraphs_per_set += subgraphs.size();
////    time_per_set += timer.elapsedMilliseconds();
//
//    std::cout << "          subgraphs count: " << subgraphs.size() << std::endl;
////    std::cout << "          time [ms]: " << std::fixed << std::setprecision(0)
////              << timer.elapsedMilliseconds() << std::endl;
//    write_subgraphs_to_file(subg_out_file, subgraphs);
//    write_time_to_file(time_out_file, timer);
//}

/*
 * This function runs the generation algorithm with memoization, remembers the origin graph for each subgraph and writes outputs to files.
*/
int run_origin(std::vector<std::string> suffixes, int suffix, int line_start, int line_end, std::string type, std::unordered_map<std::string, std::unordered_set<std::string>> memo) {
    int num_of_subgraphs = 0;
    std::unordered_map<std::string, std::unordered_set<std::string>> graphs;


    std::string subg_out_file;
    std::string time_out_file;
    if (type == "cubic") {
        subg_out_file = "iofiles/cubic-graphs/subgraphs/cub" + suffixes[suffix] + "-gir5-subgraphs.g6";
        time_out_file = "iofiles/cubic-graphs/times/cub" + suffixes[suffix] + "-gir5-time.g6";
    } else if (type == "snark") {
        subg_out_file = "iofiles/snarks-graphs/snarks-subgraphs/Generated_graphs." + suffixes[suffix] +
                        ".05.sn.cyc4-subgraph.g6";
        time_out_file =
                "iofiles/snarks-graphs/snarks-times/Generated_graphs." + suffixes[suffix] + ".05.sn.cyc4-time.g6";
    } else if (type == "small-cubic") {
        subg_out_file = "iofiles/smaller-cubic-graphs/smaller-cubic-subgraphs/Generated_graphs." + suffixes[suffix] +
                        ".04-subgraph.g6";
        time_out_file =
                "iofiles/smaller-cubic-graphs/smaller-cubic-times/Generated_graphs." + suffixes[suffix] + ".04.-time.g6";
    } else if (type == "small-snark"){
        subg_out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-subgraphs/Generated_graphs." + suffixes[suffix] + ".04.sn.cyc4-subgraph.g6";
        time_out_file = "iofiles/smaller-snarks-graphs/smaller-snarks-times/Generated_graphs." + suffixes[suffix] + ".04.sn.cyc4-time.g6";
    }

    remove_previous_file(subg_out_file);
    remove_previous_file(time_out_file);

    std::cout << std::endl;

    Timer timer;
    double time_per_set = 0;
    int num_of_subgraphs_per_set = 0;

    std::string graph_file;
    if (type == "cubic") {
        graph_file = "iofiles/cubic-graphs/cubic/cub" + suffixes[suffix] + "-gir5.g6";
    } else if (type == "snark") {
        graph_file = "iofiles/snarks-graphs/snarks/Generated_graphs." + suffixes[suffix] + ".05.sn.cyc4.g6";
    } else if (type == "small-cubic") {
        graph_file = "iofiles/smaller-cubic-graphs/smaller-cubic/Generated_graphs." + suffixes[suffix] + ".04.g6";
    } else if (type == "small-snark") {
        graph_file = "iofiles/smaller-snarks-graphs/smaller-snarks/Generated_graphs." + suffixes[suffix] + ".04.sn.cyc4.g6";
    }

    std::ifstream file;
    file.open(graph_file);
    if (file.is_open()) {
        std::cout << "  File " + graph_file << std::endl;
        std::string line;
        std::unordered_map<std::string, std::unordered_set<std::string>> memo;
//        ctpl::thread_pool pool(20);
        for (int line_num = 0; line_num < line_end; line_num++) {
            std::getline(file, line);
            if (line_num < line_start) {
                continue;
            } else {
                graphs[line] = std::unordered_set<std::string>();

                auto time = std::chrono::system_clock::now();
                std::time_t start_time = std::chrono::system_clock::to_time_t(time);
                std::cout << "      " << std::ctime(&start_time);

                std::cout << "      graph: " << line << std::endl;
                std::unordered_set<std::string> subgraphs;
                grph g = graph6_to_graph(line);

                std::cout << "                                          " << line_num + 1 << "/" << line_end << "("
                          << std::fixed << std::setprecision(3) << (double) line_num / (double) line_end * 100
                          << "%)" << std::endl;

                timer.start();
                generate_minimal_subgraphs_origin(g, subgraphs, memo);
//                pool.push(generate_minimal_subgraphs_origin, g, subgraphs, memo);
                if (!subgraphs.empty()) {
                    remove_components_of_one_vertex(subgraphs);

                    for (auto subgraph: subgraphs) {
                        std::string g6 = sparse6_to_graph6(subgraph);
                        graphs[line].insert(g6);
                    }
                }

                timer.stop();
                num_of_subgraphs_per_set += subgraphs.size();
                time_per_set += timer.elapsedMilliseconds();

                std::cout << "          subgraphs count: " << subgraphs.size() << std::endl;
                std::cout << "          time [ms]: " << std::fixed << std::setprecision(0)
                          << timer.elapsedMilliseconds() << std::endl;
                write_subgraphs_to_file(subg_out_file, subgraphs);
                write_time_to_file(time_out_file, timer);
            }
        }
        file.close();

        std::string origin_file;
        if (type == "cubic") {
            origin_file = "iofiles/cubic-graphs/origin.txt";
        } else if (type == "snark") {
            origin_file = "iofiles/snarks-graphs/origin.txt";
        } else if (type == "small-cubic") {
            origin_file = "iofiles/smaller-cubic-graphs/origin.txt";
        } else if(type == "small-snark"){
            origin_file = "iofiles/smaller-snarks-graphs/origin.txt";
        }

//        remove_previous_file(origin_file);

        std::ofstream origin_out;
        origin_out.open(origin_file, std::ios_base::app);
        if (origin_out.is_open()) {
            for (auto set: graphs) {
                if (set.second.size() > 0) {
                    origin_out << set.first << ": ";
                    for (auto elem: set.second) {
                        origin_out << elem << ", ";
                    }
                    origin_out << "\n";
                }
            }
            origin_out.close();
            std::cout << "Origin of subgraphs written to file." << std::endl << std::endl;

        } else {
            std::cout << "Couldn't open file (" + origin_file + ")." << std::endl;
        }

        std::cout << std::endl << "SUBGRAPHS PER SET: " << num_of_subgraphs_per_set << " (from "
                  << line_end - line_start << " graph(s))" << std::endl;
        std::cout << "TIME PER SET [ms]: " << time_per_set << std::endl << std::endl;

        num_of_subgraphs += num_of_subgraphs_per_set;
    } else {
        std::cout << "Couldn't open file." << std::endl;
        return -1;
    }

    return num_of_subgraphs;
}
