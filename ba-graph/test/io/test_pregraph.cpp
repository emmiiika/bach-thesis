#include "implementation.h"
#include <io/pregraph.hpp>
#include <invariants.hpp>
#include <io/print_nice.hpp>

#include <cassert>
#include <cstdio>

using namespace ba_graph;

int counter = 0;

void callback(Graph&, Factory&, std::string*) {
    counter++;
}

int main()
{
    // create test graphs
    std::vector<Graph> graphs;
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 4);
    for (int i = 1; i < 4; i++) {
        addE(graphs.back(), Location(0, i));
    }
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 4);
    addE(graphs.back(), Location(2, 0));
    addE(graphs.back(), Location(2, 1));
    addE(graphs.back(), Location(2, 3));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 4);
    addE(graphs.back(), Location(0, 1));
    addE(graphs.back(), Location(0, 2));
    addE(graphs.back(), Location(0, 3));
    addE(graphs.back(), Location(1, 2));
    addE(graphs.back(), Location(1, 3));
    addE(graphs.back(), Location(2, 3));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 4);
    addE(graphs.back(), Location(3, 1));
    addE(graphs.back(), Location(3, 2));
    addE(graphs.back(), Location(3, 0));
    addE(graphs.back(), Location(1, 2));
    addE(graphs.back(), Location(0, 2));
    addE(graphs.back(), Location(1, 0));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 4);
    addE(graphs.back(), Location(1, 2));
    addE(graphs.back(), Location(0, 2));
    addE(graphs.back(), Location(1, 0));
    addE(graphs.back(), Location(3, 1));
    addE(graphs.back(), Location(3, 2));
    addE(graphs.back(), Location(3, 0));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 6);
    addE(graphs.back(), Location(0, 1));
    addE(graphs.back(), Location(2, 1));
    addE(graphs.back(), Location(1, 3));
    addE(graphs.back(), Location(3, 4));
    addE(graphs.back(), Location(5, 3));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 8);
    addE(graphs.back(), Location(0, 4));
    addE(graphs.back(), Location(4, 5));
    addE(graphs.back(), Location(4, 1));
    addE(graphs.back(), Location(5, 3));
    addE(graphs.back(), Location(5, 1));
    addE(graphs.back(), Location(1, 7));
    addE(graphs.back(), Location(7, 6));
    addE(graphs.back(), Location(7, 2));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 8);
    addE(graphs.back(), Location(7, 2));
    addE(graphs.back(), Location(0, 4));
    addE(graphs.back(), Location(4, 5));
    addE(graphs.back(), Location(5, 1));
    addE(graphs.back(), Location(1, 7));
    addE(graphs.back(), Location(7, 6));
    addE(graphs.back(), Location(4, 1));
    addE(graphs.back(), Location(5, 3));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 12);
    addE(graphs.back(), Location(1, 5));
    addE(graphs.back(), Location(1, 8));
    addE(graphs.back(), Location(1, 6));
    addE(graphs.back(), Location(3, 5));
    addE(graphs.back(), Location(10, 8));
    addE(graphs.back(), Location(11, 10));
    addE(graphs.back(), Location(0, 10));
    addE(graphs.back(), Location(7, 8));
    addE(graphs.back(), Location(9, 5));
    addE(graphs.back(), Location(6, 4));
    addE(graphs.back(), Location(6, 2));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 12);
    addE(graphs.back(), Location(8, 7));
    addE(graphs.back(), Location(9, 5));
    addE(graphs.back(), Location(6, 4));
    addE(graphs.back(), Location(6, 1));
    addE(graphs.back(), Location(3, 5));
    addE(graphs.back(), Location(10, 8));
    addE(graphs.back(), Location(5, 1));
    addE(graphs.back(), Location(1, 8));
    addE(graphs.back(), Location(10, 0));
    addE(graphs.back(), Location(11, 10));
    addE(graphs.back(), Location(6, 2));
    
    graphs.push_back(createG());
    addMultipleV(graphs.back(), 1);
    
    // write pregraph file
    std::string file = "graphs";
    write_pregraph_code(file, graphs);

    // read written pregraphs
    read_pregraph_file<std::string>(file, callback, &file);
    assert(counter == 11); // correct number of loaded graphs

    std::vector<Graph> readGraphs = read_pregraphs_from_file(file);
    assert(readGraphs.size() == 11); // correct number of loaded graphs

    for(std::vector<Graph>::size_type i = 0; i < graphs.size(); i++) {
        assert(readGraphs[i].order() == graphs[i].order()); // graph order
        assert(readGraphs[i].size() == graphs[i].size()); // number of edges
        assert(min_deg(readGraphs[i]) == min_deg(graphs[i])); // minimal degree
        assert(max_deg(readGraphs[i]) == max_deg(graphs[i])); // maximal degree
    }

    // delete created file
    remove(file.c_str());
}
