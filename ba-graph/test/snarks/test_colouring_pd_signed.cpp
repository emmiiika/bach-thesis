#include <cassert>
#include <iostream>
#include <sstream>

#include "implementation.h"
#include <multipoles.hpp>
#include <operations.hpp>
#include <snarks/colouring_pd.hpp>
#include <io/graph6.hpp>

using namespace ba_graph;

int main() {
   PDColorizer pdc;


/***************
Petersen graph and almost petersen graph
***************/

    Graph g1(createG());
    addMultipleV(g1,10,Number(0));
    addMultipleE(g1, Locs({{0,2},{2,4},{4,1},{1,3},{3,0}}));
    addMultipleE(g1, Locs({{5,6},{6,7},{7,8},{8,9},{9,5}}));
    addMultipleE(g1, Locs({{5,0},{6,1},{7,2},{8,3},{9,4}}));
    std::set<Edge> sign = {};
    assert(!pdc.isColourable_arbitrary(g1, sign));
    assert(!pdc.isColourable_shortest_path_heuristic(g1, sign));
    sign = {g1[Loc(8,3)].e(), g1[Loc(8,7)].e(), g1[Loc(8,9)].e()};
    assert(!pdc.isColourable_arbitrary(g1, sign));
    assert(!pdc.isColourable_shortest_path_heuristic(g1, sign));
    sign = {g1[Loc(5,6)].e(), g1[Loc(7,8)].e()};
    assert(pdc.isColourable_arbitrary(g1, sign));
    PathDecomposition pd = {{g1[Loc(0,2)].e(), g1[Loc(2,4)].e(), g1[Loc(4,1)].e(), g1[Loc(1,3)].e(), g1[Loc(3,0)].e(),
                             g1[Loc(6,1)].e(), g1[Loc(5,6)].e(), g1[Loc(5,0)].e(), g1[Loc(8,9)].e(), g1[Loc(9,5)].e(), 
                             g1[Loc(6,7)].e(), g1[Loc(7,2)].e(), g1[Loc(8,3)].e(), g1[Loc(7,8)].e(), g1[Loc(9,4)].e()}};
    assert(pdc.isColourable(g1, sign, pd));
    assert(pdc.isColourable_shortest_path_heuristic(g1, sign));
    sign = {g1[Loc(8,3)].e()};
    assert(!pdc.isColourable_arbitrary(g1, sign));
    assert(!pdc.isColourable_shortest_path_heuristic(g1, sign));

    Graph g2(createG());
    addMultipleV(g2,10,Number(0));
    addMultipleE(g2, Locs({{0,2},{2,4},{4,1},{1,3},{3,0}}));
    addMultipleE(g2, Locs({{5,6},{6,7},{7,8},{8,9},{9,5}}));
    addMultipleE(g2, Locs({{5,0},{6,2},{7,1},{8,3},{9,4}}));
    sign = {g1[Loc(8,3)].e(), g1[Loc(8,7)].e(), g1[Loc(8,9)].e()};
    assert(pdc.isColourable_arbitrary(g2, sign));
    assert(pdc.isColourable_shortest_path_heuristic(g2, sign));




    Graph P8N(create_petersen());
    split_vertex(P8N, 0, 10); //creates vertices 10, 11, 12
    split_vertex(P8N, 2, 13); //creates vertices 13, 14, 15   
    Multipole M8N( {Connector({10, 11, 12}), Connector({13, 14, 15})});
    Graph GG5(copy_disjoint(P8N));
    Multipole MG5=M8N;
    join(GG5, MG5, 1, P8N, M8N, 0, 20);
    join(GG5, MG5, 1, P8N, M8N, 0, 40);
    join(GG5, MG5, 1, P8N, M8N, 0, 60);
    join(GG5, MG5, 1, P8N, M8N, 0, 80);
    join(GG5, MG5, 1, 0);
    assert(GG5.order()==40);
    sign = {};
    assert(!pdc.isColourable_shortest_path_heuristic(GG5, sign));
    sign = {GG5[Loc(3, 4)].e(), GG5[Loc(7, 5)].e()};
//    std::cout<<GG5[Loc(3, 4)].e().to_int()<<",";
//    std::cout<<GG5[Loc(7, 5)].e().to_int() << "\n";
    assert(!pdc.isColourable_shortest_path_heuristic(GG5, sign));
    sign = {GG5[Loc(3, 4)].e()};
    assert(!pdc.isColourable_shortest_path_heuristic(GG5, sign));






}
