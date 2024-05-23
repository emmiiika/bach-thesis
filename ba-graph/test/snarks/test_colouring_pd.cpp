#include <cassert>
#include <iostream>
#include <sstream>

#include "implementation.h"
#include <graphs/stored_graphs.hpp>
#include <multipoles.hpp>
#include <operations.hpp>
#include <snarks/colouring_pd.hpp>
#include <io/graph6.hpp>

using namespace ba_graph;

int main() {
   PDColorizer pdc;

   Graph g(createG());
   addMultipleV(g,10,Number(0));

   //test initialize
   pdc.initialize(g);
   assert(pdc.v[0].size()==10);
   assert(pdc.v[1].size()+pdc.v[2].size()+pdc.v[3].size()==0);


   Vertex v0, v1;

   //test 0,0
   v0=g[0].v(), v1=g[1].v();
   pdc.process_state(v0, v1);
   //test sets
   assert(pdc.v[1][0]==g[0].v());
   assert(pdc.v[1][1]==g[1].v());
   //test state
   assert(pdc.state.size()==9);
   ColouringBitArray a=pdc.state[1];
   assert(a==pdc.state[1]);
   assert(a==pdc.state[2]);
   assert(a==pdc.state[3]);
   assert(a==pdc.state[5]);
   assert(a==pdc.state[6]);
   assert(a==pdc.state[7]);
   a =pdc.state[0];
   a&=pdc.state[4];
   a&=pdc.state[8];
   assert(a.size().to_int64()==1);
   assert(a[ColouringBitArray::Index(0,0)]==true);


   //test 1,0
   v0=g[0].v(), v1=g[2].v();
   pdc.process_state(v0, v1);
   //test sets
   assert(pdc.v[1][0]==g[2].v());
   assert(pdc.v[1][1]==g[1].v());
   assert(pdc.v[2][0]==g[0].v());
   //test state
   assert(pdc.state.size()==9);
   for(auto &s:pdc.state)
       assert(s.size()==ColouringBitArray::Index(0,3));
   assert(!pdc.state[0][ColouringBitArray::Index(0,0)] && !pdc.state[0][ColouringBitArray::Index(0,1)] && !pdc.state[0][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[1][ColouringBitArray::Index(0,0)] && !pdc.state[1][ColouringBitArray::Index(0,1)] &&  pdc.state[1][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[2][ColouringBitArray::Index(0,0)] &&  pdc.state[2][ColouringBitArray::Index(0,1)] && !pdc.state[2][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[3][ColouringBitArray::Index(0,0)] && !pdc.state[3][ColouringBitArray::Index(0,1)] &&  pdc.state[3][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[4][ColouringBitArray::Index(0,0)] && !pdc.state[4][ColouringBitArray::Index(0,1)] && !pdc.state[4][ColouringBitArray::Index(0,2)]);
   assert( pdc.state[5][ColouringBitArray::Index(0,0)] && !pdc.state[5][ColouringBitArray::Index(0,1)] && !pdc.state[5][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[6][ColouringBitArray::Index(0,0)] &&  pdc.state[6][ColouringBitArray::Index(0,1)] && !pdc.state[6][ColouringBitArray::Index(0,2)]);
   assert( pdc.state[7][ColouringBitArray::Index(0,0)] && !pdc.state[7][ColouringBitArray::Index(0,1)] && !pdc.state[7][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[8][ColouringBitArray::Index(0,0)] && !pdc.state[8][ColouringBitArray::Index(0,1)] && !pdc.state[8][ColouringBitArray::Index(0,2)]);


   //test 0,2
   v0=g[3].v(), v1=g[0].v();
   pdc.process_state(v0, v1);
   //test sets
   assert(pdc.v[1][0]==g[3].v());
   assert(pdc.v[1][1]==g[2].v());
   assert(pdc.v[1][2]==g[1].v());
   assert(pdc.v[2].size()==0);
   //test state
   assert(pdc.state.size()==27);
   for(auto &s:pdc.state)
       assert(s.size()==ColouringBitArray::Index(0,1));
   assert(!pdc.state[0][ColouringBitArray::Index(0,0)] && !pdc.state[1][ColouringBitArray::Index(0,0)] && !pdc.state[2][ColouringBitArray::Index(0,0)]);
   assert(!pdc.state[3][ColouringBitArray::Index(0,0)] && !pdc.state[4][ColouringBitArray::Index(0,0)] &&  pdc.state[5][ColouringBitArray::Index(0,0)]);
   assert(!pdc.state[6][ColouringBitArray::Index(0,0)] &&  pdc.state[7][ColouringBitArray::Index(0,0)] && !pdc.state[8][ColouringBitArray::Index(0,0)]);
   assert(!pdc.state[9][ColouringBitArray::Index(0,0)] && !pdc.state[10][ColouringBitArray::Index(0,0)] &&  pdc.state[11][ColouringBitArray::Index(0,0)]);
   assert(!pdc.state[12][ColouringBitArray::Index(0,0)] && !pdc.state[13][ColouringBitArray::Index(0,0)] && !pdc.state[14][ColouringBitArray::Index(0,0)]);
   assert( pdc.state[15][ColouringBitArray::Index(0,0)] && !pdc.state[16][ColouringBitArray::Index(0,0)] && !pdc.state[17][ColouringBitArray::Index(0,0)]);
   assert(!pdc.state[18][ColouringBitArray::Index(0,0)] &&  pdc.state[19][ColouringBitArray::Index(0,0)] && !pdc.state[20][ColouringBitArray::Index(0,0)]);
   assert( pdc.state[21][ColouringBitArray::Index(0,0)] && !pdc.state[22][ColouringBitArray::Index(0,0)] && !pdc.state[23][ColouringBitArray::Index(0,0)]);
   assert(!pdc.state[24][ColouringBitArray::Index(0,0)] && !pdc.state[25][ColouringBitArray::Index(0,0)] && !pdc.state[26][ColouringBitArray::Index(0,0)]);

  //test 1,1
   v0=g[1].v(), v1=g[2].v();
   pdc.process_state(v0, v1);
   //test sets
   assert(pdc.v[1][0]==g[3].v());
   assert(pdc.v[2][0]==g[1].v() || pdc.v[2][0]==g[2].v());
   assert(pdc.v[2][1]==g[2].v() || pdc.v[2][1]==g[1].v());
   assert(pdc.v[2].size()==2);
   //test state
   assert(pdc.state.size()==3);
   for(auto &s:pdc.state)
       assert(s.size()==ColouringBitArray::Index(0,9));
   assert(!pdc.state[0][ColouringBitArray::Index(0,0)] && !pdc.state[0][ColouringBitArray::Index(0,1)] && !pdc.state[0][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[0][ColouringBitArray::Index(0,3)] && !pdc.state[0][ColouringBitArray::Index(0,4)] &&  pdc.state[0][ColouringBitArray::Index(0,5)]);
   assert(!pdc.state[0][ColouringBitArray::Index(0,6)] &&  pdc.state[0][ColouringBitArray::Index(0,7)] && !pdc.state[0][ColouringBitArray::Index(0,8)]);
   assert(!pdc.state[1][ColouringBitArray::Index(0,0)] && !pdc.state[1][ColouringBitArray::Index(0,1)] &&  pdc.state[1][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[1][ColouringBitArray::Index(0,3)] && !pdc.state[1][ColouringBitArray::Index(0,4)] && !pdc.state[1][ColouringBitArray::Index(0,5)]);
   assert( pdc.state[1][ColouringBitArray::Index(0,6)] && !pdc.state[1][ColouringBitArray::Index(0,7)] && !pdc.state[1][ColouringBitArray::Index(0,8)]);
   assert(!pdc.state[2][ColouringBitArray::Index(0,0)] &&  pdc.state[2][ColouringBitArray::Index(0,1)] && !pdc.state[2][ColouringBitArray::Index(0,2)]);
   assert( pdc.state[2][ColouringBitArray::Index(0,3)] && !pdc.state[2][ColouringBitArray::Index(0,4)] && !pdc.state[2][ColouringBitArray::Index(0,5)]);
   assert(!pdc.state[2][ColouringBitArray::Index(0,6)] && !pdc.state[2][ColouringBitArray::Index(0,7)] && !pdc.state[2][ColouringBitArray::Index(0,8)]);
   //we need an extra test, this is a degenerate case

  //test 1,2
   v0=g[1].v(), v1=g[3].v();
   pdc.process_state(v0, v1);
   //test sets
   assert(pdc.v[1].size()==0);
   assert(pdc.v[2][0]==g[2].v());
   assert(pdc.v[2][1]==g[3].v());
   assert(pdc.v[2].size()==2);
   //test state
   assert(pdc.state.size()==1);
   for(auto &s:pdc.state)
       assert(s.size()==ColouringBitArray::Index(0,9));
   assert( pdc.state[0][ColouringBitArray::Index(0,0)] && !pdc.state[0][ColouringBitArray::Index(0,1)] && !pdc.state[0][ColouringBitArray::Index(0,2)]);
   assert(!pdc.state[0][ColouringBitArray::Index(0,3)] &&  pdc.state[0][ColouringBitArray::Index(0,4)] && !pdc.state[0][ColouringBitArray::Index(0,5)]);
   assert(!pdc.state[0][ColouringBitArray::Index(0,6)] && !pdc.state[0][ColouringBitArray::Index(0,7)] &&  pdc.state[0][ColouringBitArray::Index(0,8)]);

  //test 2,2
   v0=g[2].v(), v1=g[3].v();
   pdc.process_state(v0, v1);
   //test sets
   assert(pdc.v[1].size()==0);
   assert(pdc.v[2].size()==0);
   //test state
   assert(pdc.state.size()==1);
   for(auto &s:pdc.state)
       assert(s.size()==ColouringBitArray::Index(0,1));
   assert(pdc.state[0][ColouringBitArray::Index(0,0)]);




//second test
   pdc.initialize(g);
   pdc.process_state(g[0].v(), g[2].v());
   pdc.process_state(g[2].v(), g[4].v());
   pdc.process_state(g[4].v(), g[1].v());
   pdc.process_state(g[1].v(), g[3].v());
   pdc.process_state(g[0].v(), g[3].v());

   pdc.process_state(g[5].v(), g[6].v());
   pdc.process_state(g[6].v(), g[7].v());
   pdc.process_state(g[8].v(), g[9].v());

   pdc.process_state(g[4].v(), g[6].v());
   pdc.process_state(g[3].v(), g[7].v());
   pdc.process_state(g[2].v(), g[8].v());
   pdc.process_state(g[1].v(), g[9].v());
   pdc.process_state(g[0].v(), g[5].v());
   //test sets
   assert(pdc.v[1].size()==0);
   assert(pdc.v[2].size()==4);
   assert(pdc.v[2][0]==g[7].v());
   assert(pdc.v[2][1]==g[8].v());
   assert(pdc.v[2][2]==g[9].v());
   assert(pdc.v[2][3]==g[5].v());
   //test state
   assert(pdc.state.size()==1);
   for(auto &s:pdc.state)
       assert(s.size()==ColouringBitArray::Index(3,0));
   ColouringBitArray to_compare(ColouringBitArray::Index(3,0), false);
   to_compare.set(ColouringBitArray::Index(0,1*9+0*3+1*1), true);
   to_compare.set(ColouringBitArray::Index(0,1*9+1*3+0*1), true);
   to_compare.set(ColouringBitArray::Index(1,0*9+0*3+1*1), true);
   to_compare.set(ColouringBitArray::Index(1,0*9+1*3+0*1), true);

   to_compare.set(ColouringBitArray::Index(0,2*9+0*3+2*1), true);
   to_compare.set(ColouringBitArray::Index(0,2*9+2*3+0*1), true);
   to_compare.set(ColouringBitArray::Index(2,0*9+0*3+2*1), true);
   to_compare.set(ColouringBitArray::Index(2,0*9+2*3+0*1), true);

   to_compare.set(ColouringBitArray::Index(1,2*9+1*3+2*1), true);
   to_compare.set(ColouringBitArray::Index(1,2*9+2*3+1*1), true);
   to_compare.set(ColouringBitArray::Index(2,1*9+1*3+2*1), true);
   to_compare.set(ColouringBitArray::Index(2,1*9+2*3+1*1), true);

   assert(to_compare==pdc.state[0]);

//negator test

   pdc.initialize(g);
   pdc.process_state(g[0].v(), g[2].v());
   pdc.process_state(g[0].v(), g[3].v());
   pdc.process_state(g[4].v(), g[1].v());
   pdc.process_state(g[7].v(), g[8].v());

   pdc.process_state(g[3].v(), g[8].v());
   pdc.process_state(g[2].v(), g[4].v());
   pdc.process_state(g[3].v(), g[1].v());
   pdc.process_state(g[2].v(), g[7].v());



   //test sets
   assert(pdc.v[1].size()==0);
   assert(pdc.v[2].size()==5);
   assert(pdc.v[2][0]==g[0].v());
   assert(pdc.v[2][1]==g[8].v());
   assert(pdc.v[2][2]==g[4].v());
   assert(pdc.v[2][3]==g[1].v());
   assert(pdc.v[2][4]==g[7].v());
   //test state
   assert(pdc.state.size()==1);
   for(auto &s:pdc.state)
       assert(s.size()==ColouringBitArray::Index(9,0));
   for(ColouringBitArray::Index i(0,0); i<ColouringBitArray::Index(9,0); i++) {
       unsigned int i4=i.first()/3;
       unsigned int i3=i.first()%3;
       unsigned int i2=i.second()/9;
       unsigned int i1=(i.second()%9)/3;
       unsigned int i0=i.second()%3;    
       int a[3]= {0,0,0}; a[i0]++; a[i1]++; a[i2]++; a[i3]++; a[i4]++;
       if ( ((i1!=i2 && i3==i4) || (i1==i2 && i3!=i4)) && ((a[0]%2) && (a[1]%2) && (a[2]%2)) ) assert(pdc.state[0][i]);
       else assert(!pdc.state[0][i]);
   }



//third test - Isaacs block
   pdc.initialize(g);
   pdc.process_state(g[0].v(), g[5].v());
   pdc.process_state(g[2].v(), g[3].v());
   pdc.process_state(g[4].v(), g[1].v());
   pdc.process_state(g[6].v(), g[0].v());
   pdc.process_state(g[6].v(), g[1].v());
   pdc.process_state(g[6].v(), g[2].v());
   pdc.process_state(g[7].v(), g[5].v());
   pdc.process_state(g[7].v(), g[4].v());
   pdc.process_state(g[7].v(), g[3].v());


// to print the state....
/*   for(unsigned int i=0;i<pdc.v[1].size();i++) std::cout<<(pdc.v[1][i].to_int()-1)<<",";
   std::cout<<"\n";
   for(unsigned int i=0;i<pdc.v[2].size();i++) std::cout<<(pdc.v[2][i].to_int()-1)<<",";
   std::cout<<"\n";
   for(uint64_t j=0;j<pdc.state.size(); j++)
       for(ColouringBitArray::Index i(0,0); i<pdc.state[j].size(); i++) {
           auto i5=i.first()/9;
           auto i4=(i.first()%9)/3;
           auto i3=i.first()%3;
           auto i2=i.second()/9;
           auto i1=(i.second()%9)/3;
           auto i0=i.second()%3;    
           auto j5=j/243;
           auto j4=(j%243)/81;
           auto j3=(j%81)/27;
           auto j2=(j%27)/9;
           auto j1=(j%9)/3;
           auto j0=j%3;    
           if (pdc.state[j][i])
               std::cout<<j0<<j1<<j2<<j3<<j4<<j5<<"|"<<i0<<i1<<i2<<i3<<i4<<i5<<"|"<<pdc.state[j][i]<<"\n";
       }*/


   //test state
   assert(pdc.state.size()==1);
   for(auto &s:pdc.state)
       assert(s.size()==ColouringBitArray::Index(27,0));
   for(ColouringBitArray::Index i(0,0); i<ColouringBitArray::Index(9,0); i++) {
       unsigned int i5=i.first()/9;
       unsigned int i4=(i.first()%9)/3;
       unsigned int i3=i.first()%3;
       unsigned int i2=i.second()/9;
       unsigned int i1=(i.second()%9)/3;
       unsigned int i0=i.second()%3;    
       int a[3]= {0,0,0}; a[i0]++; a[i1]++; a[i2]++; a[i3]++; a[i4]++; a[i5]++;
       if ((a[0]%2) || (a[1]%2) || (a[2]%2)) {assert(!pdc.state[0][i]); continue;}
       if ((i0+i1+i2)%3 == 0) { //permutation or i0=i1=i2
           if ((i3+i4+i5)%3 != 0) {assert(!pdc.state[0][i]); continue;} //second triple has the same type
           if (i0 == i1) {assert(!pdc.state[0][i]); continue;} //i0=i1=i2 impossible
           if (i3 == i4) {assert(!pdc.state[0][i]); continue;} //i3=i4=i5 impossible
           if ((4*i0+2*i1+i2)%5 <= 1 && (4*i3+2*i4+i5)%5 <= 1) {assert(pdc.state[0][i]); continue;} //(4*i0+2*i1+i2) %5 <= 1 checks permutation parity - odd
           if ((4*i0+2*i1+i2)%5  > 1 && (4*i3+2*i4+i5)%5  > 1) {assert(pdc.state[0][i]); continue;} //(4*i0+2*i1+i2) %5 > 1 checks permutation parity - even
           assert(!pdc.state[0][i]); continue; //different permutation parities
       } else {
           if ((i3+i4+i5)%3 == 0) {assert(!pdc.state[0][i]); continue;} //second one is of the same type
           if (i0*i0+i1*i1+i2*i2==i3*i3+i4*i4+i5*i5) {assert(pdc.state[0][i]); continue;}; //this checks if the colours are used the same number of times
           assert(!pdc.state[0][i]);
       }

   }

/***************
Petersen graph and almost petersen graph
***************/

    Graph g1(createG());
    addMultipleV(g1,10,Number(0));
    addMultipleE(g1, Locs({{0,2},{2,4},{4,1},{1,3},{3,0}}));
    addMultipleE(g1, Locs({{5,6},{6,7},{7,8},{8,9},{9,5}}));
    addMultipleE(g1, Locs({{5,0},{6,1},{7,2},{8,3},{9,4}}));
    assert(!pdc.isColourable_arbitrary(g1));
    assert(!pdc.isColourable_shortest_path_heuristic(g1));
    assert(pdc.resistance_arbitrary(g1,4)==2);
    assert(pdc.strong_oddness_arbitrary(g1,4)==2);
    assert(pdc.weak_oddness_arbitrary(g1,4)==2);

    Graph g2(createG());
    addMultipleV(g2,10,Number(0));
    addMultipleE(g2, Locs({{0,2},{2,4},{4,1},{1,3},{3,0}}));
    addMultipleE(g2, Locs({{5,6},{6,7},{7,8},{8,9},{9,5}}));
    addMultipleE(g2, Locs({{5,0},{6,2},{7,1},{8,3},{9,4}}));
    assert(pdc.isColourable_arbitrary(g2));
    assert(pdc.isColourable_shortest_path_heuristic(g2));
    assert(pdc.resistance_arbitrary(g2,4)==0);
    assert(pdc.strong_oddness_arbitrary(g2,4)==0);
    assert(pdc.weak_oddness_arbitrary(g2,4)==0);


/***************
Small cubic graph test
***************/

const std::string correctAnswer = R"(
1
11
11111
0111111111111111110
0000111111111111111111111111111111111111111111111111111111111101111111111111111111111
00000000000000000000000111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111000011111111111111001111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111110111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111011111111111111101111111111101111111111111111111101111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
)";

    std::stringstream output;
    output << std::endl;
    for (int i = 4; i <= 14; i += 2) {
        std::string filename = "../../resources/graphs/"
                + internal::stored_cubic_path(1, 3, i);
        Factory f;
        auto graphs = read_graph6_file(filename, f).graphs();
        for (Graph &G : graphs)
            output << pdc.isColourable_arbitrary(G);
        output << std::endl;
    }
    assert(output.str() == correctAnswer);


/***************
Simple graph with resistance 3
***************/

    Graph g_res3(createG());
    addMultipleV(g_res3, 10, Number(0));
    addMultipleE(g_res3, Locs({{1,2},{1,2},{1,3},{2,3}}));
    addMultipleE(g_res3, Locs({{4,5},{4,5},{4,6},{5,6}}));
    addMultipleE(g_res3, Locs({{7,8},{7,8},{7,9},{8,9}}));
    addMultipleE(g_res3, Locs({{0,3},{0,6},{0,9}}));
    assert(pdc.resistance_arbitrary(g_res3,4)==3);
    assert(pdc.strong_oddness_arbitrary(g_res3,6)==6); //maximum reached
    assert(pdc.weak_oddness_arbitrary(g_res3,6)==4); 


/***************
44-vertex resistance 3 and oddness 4 graph and further graphs in teh series
***************/

    Graph P8(create_petersen());
    deleteE(P8, Loc(0,1));
    split_vertex(P8, 0, 10); //creates vertices 10, 11
    split_vertex(P8, 1, 12); //creates vertices 12, 13   
    Multipole MP8( {Connector({10, 11}), Connector({12, 13})});

    Graph P10(create_petersen());
    subdivide_edge(P10, Loc(0,1), 10);
    subdivide_edge(P10, Loc(3,8), 11);
    split_vertex(P10, 10, 12); //creates vertices 12, 13
    split_vertex(P10, 11, 14); //creates vertices 14, 15
    Multipole MP10({Connector({12, 13}), Connector({14, 15})});

    Graph G44(copy_disjoint(P8));
    Multipole M44=MP8;
    join(G44, M44, 1, P10, MP10, 0, 20);
    join(G44, M44, 1, P8, MP8, 0, 40);
    join(G44, M44, 1, P10, MP10, 0, 60);
    join(G44, M44, 1, P8, MP8, 0, 80);
    join(G44, M44, 0, 1);
    assert(G44.order()==44);
    assert(pdc.resistance_shortest_path_heuristic(G44,4)==3);
    assert(pdc.strong_oddness_shortest_path_heuristic(G44,5)==4);
    assert(pdc.weak_oddness_shortest_path_heuristic(G44,5)==4);


/***************
96 and 104-vertex graphs
***************/

    Graph G96(copy_disjoint(P8));
    Multipole M96=MP8;
    join(G96, M96, 1, P10, MP10, 0, 20);
    join(G96, M96, 1, P8, MP8, 0, 40);
    join(G96, M96, 1, P8, MP8, 0, 60);
    join(G96, M96, 1, P10, MP10, 0, 80);
    join(G96, M96, 1, P8, MP8, 0, 100);
    join(G96, M96, 1, P8, MP8, 0, 120);
    join(G96, M96, 1, P10, MP10, 0, 140);
    join(G96, M96, 1, P8, MP8, 0, 160);
    join(G96, M96, 1, P8, MP8, 0, 180);
    join(G96, M96, 1, P10, MP10, 0, 200);
    join(G96, M96, 0, 1);
    assert(G96.order()==96);
    assert(pdc.resistance_shortest_path_heuristic(G96,8)==7);
    assert(pdc.strong_oddness_shortest_path_heuristic(G96,9)==8);
    assert(pdc.weak_oddness_shortest_path_heuristic(G96,9)==8);

    Graph G104(copy_disjoint(P8));
    Multipole M104=MP8;
    join(G104, M104, 1, P10, MP10, 0, 20);
    join(G104, M104, 1, P8, MP8, 0, 40);
    join(G104, M104, 1, P8, MP8, 0, 60);
    join(G104, M104, 1, P10, MP10, 0, 80);
    join(G104, M104, 1, P8, MP8, 0, 100);
    join(G104, M104, 1, P8, MP8, 0, 120);
    join(G104, M104, 1, P10, MP10, 0, 140);
    join(G104, M104, 1, P8, MP8, 0, 160);
    join(G104, M104, 1, P8, MP8, 0, 180);
    join(G104, M104, 1, P10, MP10, 0, 200);
    join(G104, M104, 1, P8, MP8, 0, 220);
    join(G104, M104, 0, 1);
    assert(G104.order()==104);
    assert(pdc.resistance_shortest_path_heuristic(G104,9)==8);
    assert(pdc.strong_oddness_shortest_path_heuristic(G104,9)==8);
    assert(pdc.weak_oddness_shortest_path_heuristic(G104,9)==8);
   
/***************
I. Allie graph
***************/     

    Graph GAllieHalf(copy_disjoint(P8));
    Multipole MAllieHalf=MP8;
    join(GAllieHalf, MAllieHalf, 1, P8, MP8, 0, 20);
    join(GAllieHalf, MAllieHalf, 1, P8, MP8, 0, 40);
    join(GAllieHalf, MAllieHalf, 1, P8, MP8, 0, 60);
    join(GAllieHalf, MAllieHalf, 1, P8, MP8, 0, 80);
    join(GAllieHalf, MAllieHalf, 1, P8, MP8, 0,100);


    Graph GAllie(createG());
    addMultipleV(GAllie, 12, Number(0));
    addMultipleE(GAllie, Locs({{0,1}, {0,2}, {0,3}}));
    addMultipleE(GAllie, Locs({{4,5}, {4,6}, {4,3}}));
    addMultipleE(GAllie, Locs({{3,7}, {7,8}, {7,9}, {10,11}}));
    Multipole MAllie( {Connector({1, 2}), Connector({5, 6}), Connector({8, 10}), Connector({9, 11})});

    join(GAllie, MAllie, 0, GAllieHalf, MAllieHalf, 0, 200);
    join(GAllie, MAllie, 1, 3);
    join(GAllie, MAllie, 0, GAllieHalf, MAllieHalf, 0, 400);
    join(GAllie, MAllie, 0, 1);
    assert(pdc.strong_oddness_shortest_path_heuristic(GAllie,9)==8);
    assert(pdc.weak_oddness_shortest_path_heuristic(GAllie,5)==4);



}
