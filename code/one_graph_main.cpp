#include "gen.cpp"
#include "timer.cpp"
#include <iostream>

int main(){
    Timer timer;

    std::string filein = "iofiles/tmp.txt";
    std::string fileout = "iofiles/tmp_out.txt";
    std::ifstream input_file;
    std::ofstream output_file;
    std::string coded_graph;


    input_file.open(filein);
    std::getline(input_file, coded_graph);

    auto time = std::chrono::system_clock::now();
    std::time_t start_time = std::chrono::system_clock::to_time_t(time);
    std::cout << "      " << std::ctime(&start_time);

    std::cout << "      line: " << coded_graph << std::endl;
    std::unordered_set<std::string> subgraphs;
    std::unordered_set<std::string> memo;
    grph g = graph6_to_graph(coded_graph);

    timer.start();
    generate_minimal_subgraphs(g, subgraphs, memo);
    if(!subgraphs.empty()){
        remove_components_of_one_vertex(subgraphs);
    }
    timer.stop();

    std::cout << "          subgraphs count: " << subgraphs.size() << std::endl;
    std::cout << "          time [ms]: " << std::fixed << std::setprecision(0)
              << timer.elapsedMilliseconds() << std::endl;
    write_subgraphs_to_file(fileout, subgraphs);
}