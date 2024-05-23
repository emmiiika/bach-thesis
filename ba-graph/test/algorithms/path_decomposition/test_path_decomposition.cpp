#include <impl/basic/include.hpp>
#include <algorithms/path_decomposition/path_decomposition.hpp>

#include <cassert>

using namespace ba_graph;

int main() {
    Graph g(createG());
    addMultipleV(g, 10, Number(0));
    addMultipleE(g, Locs({{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}}));
    addMultipleE(g, Locs({{5, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 5}}));
    addMultipleE(g, Locs({{5, 0}, {6, 1}, {7, 2}, {8, 3}, {9, 4}}));

    PathDecomposition pd;
    pd.ordered_edges.push_back(g[Loc(0,1)].e());
    assert(pd.width()==2);
    pd.ordered_edges.push_back(g[Loc(1,2)].e());
    assert(pd.width()==2);
    pd.ordered_edges.push_back(g[Loc(3,4)].e());
    assert(pd.width()==2);
    pd.ordered_edges.push_back(g[Loc(2,3)].e());
    assert(pd.width()==3);
    pd.ordered_edges.push_back(g[Loc(4,0)].e());
    assert(pd.width()==4);
    pd.ordered_edges.push_back(g[Loc(5,0)].e());
    assert(pd.width()==4);
    pd.ordered_edges.push_back(g[Loc(6,1)].e());
    assert(pd.width()==5);
    pd.ordered_edges.push_back(g[Loc(5,6)].e());
    assert(pd.width()==5);
    pd.ordered_edges.push_back(g[Loc(7,2)].e());
    assert(pd.width()==5);
    pd.ordered_edges.push_back(g[Loc(6,7)].e());
    assert(pd.width()==5);
    pd.ordered_edges.push_back(g[Loc(8,9)].e());
    assert(pd.width()==5);
    pd.ordered_edges.push_back(g[Loc(7,8)].e());
    assert(pd.width()==5);
    pd.ordered_edges.push_back(g[Loc(9,4)].e());
    assert(pd.width()==5);
    pd.ordered_edges.push_back(g[Loc(2,7)].e());
    assert(pd.width()==5);
    pd.ordered_edges.push_back(g[Loc(3,8)].e());
    assert(pd.width()==6);

    
}
