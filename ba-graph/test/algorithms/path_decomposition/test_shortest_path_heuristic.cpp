#include <impl/basic/include.hpp>
#include <algorithms/path_decomposition/shortest_path_heuristic.hpp>

#include <cassert>

using namespace ba_graph;

int main() {
    Graph g(createG());
    addMultipleV(g, 10, Number(0));
    addMultipleE(g, Locs({{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}}));
    addMultipleE(g, Locs({{5, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 5}}));
    addMultipleE(g, Locs({{5, 0}, {6, 1}, {7, 2}, {8, 3}, {9, 4}}));    

    auto pd=shortest_path_heuristic(g);
    assert(pd.ordered_edges.size()==15);
    for(auto it: g.list(RP::all(), IP::primary()))
        assert(std::find(pd.ordered_edges.begin(), pd.ordered_edges.end(),it->e())!=pd.ordered_edges.end());

    assert(pd.width()<=5);
}
