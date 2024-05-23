#include <fstream>
#include <string>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "../gen.cpp"


int main(){
    std::ifstream graph_file;
    std::ofstream output_file;
    std::unordered_map<std::string, std::unordered_set<std::string>> graphs;

    output_file.open("iofiles/origin.txt");
    graph_file.open("iofiles/cub-gir5-allGraphsInOne.g6");
    if(output_file.is_open()) {
        if (graph_file.is_open()) {
            std::string line;
            std::unordered_set<std::string> memo;
            while (getline(graph_file, line)) {
//                std::cout << line << std::endl;
                graphs[line] = std::unordered_set<std::string>();

                grph g = graph6_to_graph(line);
//                std::cout << "  generujem" << std::endl;
                generate_minimal_subgraphs(g, graphs[line], memo);
//                std::cout << "  hotovo" << std::endl;

//                std::cout << line << ": ";
//                print_vector(graphs[line], "");

                if (graphs[line].size() > 0) {
//                    std::cout << std::endl;
                    std::cout << line << ": ";
                    print_vector(graphs[line], "");

                    output_file << line << ": [";
                    for (auto elem: graphs[line]) {
//                        std::cout << "      ZAPISUJEM" << std::endl;
                        output_file << sparse6_to_graph6(elem) << ", ";
                    }
                    output_file << "]\n";
                }
            }
            graph_file.close();
        } else {
            std::cout << "Couldn't open file (iofiles/cub-gir5-allGraphsInOne.g6)." << std::endl;
        }
        output_file.close();
    } else {
        std::cout << "Couldn't open file (iofiles/origin.txt)." << std::endl;
    }

//    std::ifstream file;
//    file.open("iofiles/cub-gir5-noRedunAllInOne.g6");
//    if (file.is_open()){
//        std::string line;
//        while(getline(file, line)){
//            std::string graph6 = line;
//        }
//    }
//    else{
//        std::cout << "Couldn't open file (iofiles/cub-gir5-noRedunAllInOne.g6)." << std::endl;
//    }
}
