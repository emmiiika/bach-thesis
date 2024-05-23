#include <impl/basic/include.hpp>

#include <io.hpp>

#include <fstream>
#include <iostream>

using namespace ba_graph;


void incorrectParameters() {
    std::cerr << "Incorrect command line parameters, use 'convert <inputFormat> <outputFormat> <inputFile> <outputFile>'\nAvailable conversions are g6 -> s6; s6 -> g6; s6/g6 -> oldBA; oldBA -> s6/g6\n";
    exit(1);
}

bool toS6(std::string &, Graph &G, Factory &, std::ostream *out) {
    write_sparse6_stream(G, *out);
    return true;
}

bool toG6(std::string &, Graph &G, Factory &, std::ostream *out) {
    write_graph6_stream(G, *out);
    return true;
}


int main(int argc, char **argv) {
    if (argc < 5)
        incorrectParameters();

    std::string inputFormat = argv[1];
    std::string outputFormat = argv[2];
    std::string inputFileName = argv[3];
    std::string outputFileName = argv[4];

    if (inputFormat == "g6" && outputFormat == "s6") {
        std::ofstream out(outputFileName);
        if (!out)
            throw std::runtime_error("cannot write output file");
        read_graph6_file<std::ostream>(inputFileName, toS6, &out, 0);
        return 0;
    }
    if (inputFormat == "s6" && outputFormat == "g6") {
        std::ofstream out(outputFileName);
        if (!out)
            throw std::runtime_error("cannot write output file");
        read_graph6_file<std::ostream>(inputFileName, toG6, &out, 0);
        return 0;
    }
    if (inputFormat == "oldBA" && outputFormat == "g6") {
        std::ofstream out(outputFileName);
        if (!out)
            throw std::runtime_error("cannot open output file");
        auto graphs = read_oldBA_file(inputFileName);
        write_graph6_file(outputFileName, graphs.begin(), graphs.end());
        return 0;
    }
    if (inputFormat == "oldBA" && outputFormat == "s6") {
        std::ofstream out(outputFileName);
        if (!out)
            throw std::runtime_error("cannot open output file");
        auto graphs = read_oldBA_file(inputFileName);
        write_sparse6_file(outputFileName, graphs.begin(), graphs.end());
        return 0;
    }
    if ((inputFormat == "g6" && outputFormat == "oldBA") ||
            (inputFormat == "s6" && outputFormat == "oldBA")) {
        auto graphs = read_graph6_file(inputFileName).graphs();
        write_oldBA_file(outputFileName, graphs.begin(), graphs.end(), graphs.size());
        return 0;
    }
    return 0;
}
