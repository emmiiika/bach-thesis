#include "gen.cpp"
#include "timer.cpp"
#include "statistics/stat_helpers.cpp"
#include <iostream>

/*
 * This program generates subgraph and time files.
 */
void generate(std::string type) {
    std::cout << std::endl << "==================== NOW COMPUTING: " << type << " ====================" << std::endl;

    std::vector<std::string> suffixes;
    std::vector<int> file_lenghts;
    int from;
    int to;

    if (type == "cubic") {
        suffixes = {"10", "12", "14", "16", "18", "20"};
        file_lenghts = {1, 2, 9, 49, 455, 5783};
        from = 0;
        to = 6;
    } else if (type == "snark") {
        suffixes = {"10", "18", "20", "22", "24", "26"};
        file_lenghts = {1, 2, 6, 20, 38, 280};
        from = 0;
        to = 6;
    } else if (type == "small-cubic") {
        suffixes = {"06", "08", "10", "12", "14", "16", "18", "20"};
        file_lenghts = {1, 2, 6, 22, 110, 792, 7805, 97546};
        from = 0;
        to = 8;
    } else if (type == "small-snark") {
        suffixes = {"10", "18", "20", "22", "24"};
        file_lenghts = {1, 2, 6, 31, 155};
        from = 4;
        to = 5;
    }

    Timer timer;
    timer.start();
    int subgraphs = 0;
    std::unordered_map<std::string, std::unordered_set<std::string>> memo;
    for (int i = from; i < to; i++) {
        std::cout << "i: " << i << std::endl;
//        subgraphs += run(suffixes, i, 0, file_lenghts[i], type);
        subgraphs += run_origin(suffixes, i, 0, file_lenghts[i], type, memo);
    }
    timer.stop();

    std::cout << std::endl;
    std::cout << "==================== OVERALL TIME [ms]: " << std::fixed << std::setprecision(0) << timer.elapsedMilliseconds()
              << " ====================" << std::endl;
    std::cout << "==================== NUMBER OF SUBGRAPHS: " << subgraphs << " ====================" << std::endl;
}

int main(){
//        all types are {"cubic", "snark", "small-cubic", "small-snark"};
    std::string types[] = {"cubic", "snark", "small-snark"};
    for(std::string type: types) {
        generate(type);
    }

    for(std::string type: types) {
        get_stats(type);
    }
}
