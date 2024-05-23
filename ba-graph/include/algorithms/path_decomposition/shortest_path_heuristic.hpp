#ifndef BA_GRAPH_PATH_DECOMPOSITION_SHORTEST_PATH_HEURISTICS_HPP
#define BA_GRAPH_PATH_DECOMPOSITION_SHORTEST_PATH_HEURISTICS_HPP

#ifdef BA_GRAPH_DEBUG
#include <iostream>
#endif

#include "path_decomposition.hpp"
#include <vector>
#include <queue>
#include <map>

namespace ba_graph {


    //works only for connected cubic bridgeless graphs (OK for g)
    inline PathDecomposition shortest_path_heuristic(const Graph &g, Edge startEdge) {
        PathDecomposition pd;
        EdgeLabeling<int> used(0);
        VertexLabeling<int> times_used(0);
        auto is_partialy_used = [&] (const Rotation& r) {int ti=times_used[r.v()]; return (ti==1) || (ti==2);};

        pd.ordered_edges.push_back(startEdge);
        times_used.set(startEdge.v1(), times_used[startEdge.v1()]+1);
        times_used.set(startEdge.v2(), times_used[startEdge.v2()]+1);
        used.set(startEdge, 1);

        while (1) {
            //we find a shortest path between used vertices
            std::vector<Edge> best;

            for(RotationIterator rit: g.list(is_partialy_used)) {
                 std::map<Vertex, IncidenceIterator> previous;
                 VertexLabeling<int> found(0);
                 found.set(rit->v(), 1);
                 std::queue<Rotation *> rq;
                 rq.push(&(*rit));

                 while(1) {
                     if (rq.empty()) break;
                     Rotation *r2=rq.front(); rq.pop();
                        
                     if (r2->v()!=rit->v() && times_used[r2->v()]) { //we found something
                         std::vector<Edge> mine;
                         while(1) {
                             if (r2->v()==rit->v()) break; //mine is complete
                             auto iit=previous[r2->v()];
                             mine.push_back(iit->e());
                             r2=&(iit->r1());
                         }
                         if (best.size()==0 || mine.size()<best.size()) 
                             best=std::move(mine);
                         break;
                     }

                     for (auto iit: r2->list(IP::all())) {
                         if (used[iit->e()]) continue;                         
                         if (found[iit->v2()]) continue;
                         found.set(iit->v2(), 1);
                         previous.insert_or_assign(iit->v2(), iit);
                         rq.push(&(iit->r2()));
                     }
                 }
                 
            }
            //we add best to the decomposition
            for (Edge e: best) {
                pd.ordered_edges.push_back(e);
                used.set(e,1);
                times_used.set(e.v1(), times_used[e.v1()]+1);
                times_used.set(e.v2(), times_used[e.v2()]+1);
            }
            //we might be done
            if (g.count(is_partialy_used)==0) break;
            
        }
         
            

        return pd;
    }



    //works only for connected cubic bridgeless graphs (OK for g)
    inline PathDecomposition shortest_path_heuristic(const Graph &g) {
        int best_width=g.order();
        PathDecomposition best_pd;
        int tries=0;
        for(auto &rot: g) {
            PathDecomposition pd=shortest_path_heuristic(g,rot[0].e());
            int pd_width=pd.width();
            if (pd_width<best_width) {best_pd=pd; best_width=pd_width;}
            tries++;
            long long unsigned int o=g.order();
            if (o*tries*tries>100000) break; //we end if we tried hard enough
            if (best_width<20 && o*tries*tries>10000) break; //or we end if we have good results
            if (best_width<18 && o*tries*tries>1000) break;
            if (best_width<16 && o*tries*tries>100) break;
        }
        return best_pd;
        
    }



} //end namespace ba_graph

#endif
