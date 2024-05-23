#ifndef BA_GRAPH_PATH_DECOMPOSITION_PATH_DECOMPOSITION_HPP
#define BA_GRAPH_PATH_DECOMPOSITION_PATH_DECOMPOSITION_HPP

#ifdef BA_GRAPH_DEBUG
#include <iostream>
#endif

#include <vector>

namespace ba_graph {

class PathDecomposition {
public:
   std::vector<Edge> ordered_edges;
   int width() const {
      int cnt=0;
      int cntmax=0;
      for(auto it=std::cbegin(ordered_edges); it!=std::cend(ordered_edges); it++) {
          Vertex v1=it->v1();
          Vertex v2=it->v2();
          auto has_v1 = [=](Edge e) {return e.v1()==v1 || e.v2()==v1;};
          auto has_v2 = [=](Edge e) {return e.v1()==v2 || e.v2()==v2;};
          if (std::find_if(std::cbegin(ordered_edges), it,  has_v1) == it) cnt++;
          if (v1!=v2)
              if (std::find_if(std::cbegin(ordered_edges), it, has_v2) == it) cnt++;
          if (cntmax<cnt) cntmax=cnt;
          if (std::find_if(it+1, std::cend(ordered_edges), has_v1) == std::cend(ordered_edges)) cnt--;
          if (v1!=v2)
              if (std::find_if(it+1, std::cend(ordered_edges), has_v2) == std::cend(ordered_edges)) cnt--;
      }
      return cntmax;
   }
};






} //end namespace ba_graph

#endif
