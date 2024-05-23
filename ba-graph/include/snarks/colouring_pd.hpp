#ifndef BA_GRAPH_SNARKS_COLOURING_PD_HPP
#define BA_GRAPH_SNARKS_COLOURING_PD_HPP

#include "colouring_bit_array.hpp"

#include "../algorithms/path_decomposition/shortest_path_heuristic.hpp"

#include <vector>
#include <utility>
#include <set>

namespace ba_graph {

class PDColorizer {
public:
    std::vector<Vertex> v[4];
    

    std::vector<ColouringBitArray> state;
    uint_fast64_t exp1[41];
    ColouringBitArray::Index exp2[41];

    void initialize(const Graph &g) {
        v[0]=g.list(RP::all(),RT::v());
        v[1]=std::vector<Vertex>();
        v[2]=std::vector<Vertex>();
        v[3]=std::vector<Vertex>();
        state=std::vector<ColouringBitArray>();
        state.push_back(ColouringBitArray(1, true));
        exp1[0]=1; exp2[0]=ColouringBitArray::Index(0,1);
        exp1[1]=3; exp2[1]=ColouringBitArray::Index(0,3);
        exp1[2]=9; exp2[2]=ColouringBitArray::Index(0,9);
        exp1[3]=27; exp2[3]=ColouringBitArray::Index(1,0);
        for(unsigned int i=4; i<41; i++) {
            exp1[i]=3*exp1[i-1];
            exp2[i]=ColouringBitArray::Index(3*exp2[i].first(),0);
        }
    }


    void locate_and_swap(Vertex &va, Vertex &vb, int &posa, int &posb, int &posa2, int &posb2) {
        posa=0, posb=0; 
        posa2=0, posb2=0;  
        for(int k=0;k<=3;k++)
            for(unsigned int i=0; i<v[k].size(); i++) {
                if (v[k][i]==va) {posa=k; posa2=i;}
                if (v[k][i]==vb) {posb=k; posb2=i;}
            }

        if (std::pair(posa, posa2) > std::pair(posb, posb2)) {
            std::swap(va, vb);
            std::swap(posa, posb);
            std::swap(posa2, posb2);
        }
    }

    void process_sets(Vertex va, Vertex vb) {         
        int posa, posb, posa2, posb2;
        locate_and_swap(va, vb, posa, posb, posa2, posb2);
        process_sets(va, vb, posa, posb, posa2, posb2);
    }

    void process_sets(Vertex va, Vertex vb, int posa, int posb, int &posa2, int &posb2) {         
        if (posa==0 && posb==0) {
            v[0].erase(v[0].begin()+posb2);
            v[0].erase(v[0].begin()+posa2);
            v[1].insert(v[1].begin(), vb);
            v[1].insert(v[1].begin(), va);
        }


        if (posa==0 && posb==1) {
            v[0].erase(v[0].begin()+posa2);
            std::vector<Vertex> nv1;
            nv1.push_back(va);
            nv1.insert(nv1.end(),v[1].begin()+posb2+1,v[1].end());
            nv1.insert(nv1.end(),v[1].begin(),v[1].begin()+posb2);
            v[2].push_back(vb);
            v[1]=std::move(nv1);
        }


        if (posa==0 && posb==2) {
            v[0].erase(v[0].begin()+posa2);
            v[2].erase(v[2].begin()+posb2);
            v[1].insert(v[1].begin(), va);
            v[3].push_back(vb);
        }


        if (posa==1 && posb==1) {
            std::vector<Vertex> nv1;
            nv1.insert(nv1.end(),v[1].begin()+posb2+1,v[1].end());
            nv1.insert(nv1.end(),v[1].begin()+posa2+1,v[1].begin()+posb2);
            nv1.insert(nv1.end(),v[1].begin(),v[1].begin()+posa2);
            v[1]=std::move(nv1);
            v[2].push_back(vb);
            v[2].push_back(va);
        }


        if (posa==1 && posb==2) {
            std::vector<Vertex> nv1;
            nv1.insert(nv1.end(),v[1].begin()+posa2+1,v[1].end());
            nv1.insert(nv1.end(),v[1].begin(),v[1].begin()+posa2);
            v[1]=std::move(nv1);
            v[2].erase(v[2].begin()+posb2);
            v[2].push_back(va);
            v[3].push_back(vb);
        }


        if (posa==2 && posb==2) {
            v[2].erase(v[2].begin()+posb2);
            v[2].erase(v[2].begin()+posa2);
            v[3].push_back(va);
            v[3].push_back(vb);
        }
    }



/*****************
coloring
******************/

    void process_state(Vertex va, Vertex vb) {         
        int posa, posb, posa2, posb2;
        locate_and_swap(va, vb, posa, posb, posa2, posb2);
        process_state(va, vb, posa, posb, posa2, posb2);
    }

    
    void process_state(Vertex va, Vertex vb, int posa, int posb, int &posa2, int &posb2) {         
        process_sets(va, vb, posa, posb, posa2, posb2);
        process_just_state(posa, posb, posa2, posb2);
    }

    void process_just_state(int posa, int posb, int &posa2, int &posb2) {         
        if (posa==0 && posb==0) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()*9);
            for(uint_fast64_t i=0; i<state.size();i++) {
                //compute state
                newstate.push_back(state[i]);
                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(ColouringBitArray(state[i].size(), false));

                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(state[i]);
                newstate.push_back(ColouringBitArray(state[i].size(), false));

                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(std::move(state[i])); //we do not need the state any longer
            }
            state=std::move(newstate);
            return;
        }


        if (posa==0 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            for(uint_fast64_t i=0;i<exp1[posb2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posb2+1]) {
                    uint_fast64_t s0=j;
                    uint_fast64_t s1=j+exp1[posb2];
                    uint_fast64_t s2=j+2*exp1[posb2];
                    ColouringBitArray substate0=std::move(state[s0]);
                    ColouringBitArray substate1=std::move(state[s1]);
                    ColouringBitArray substate2=std::move(state[s2]);
                    ColouringBitArray false_state=ColouringBitArray(substate0.size(), false);                  

                    ColouringBitArray newsubstate0=false_state;
                    newsubstate0.concatenate_to_special(substate2);
                    newsubstate0.concatenate_to_special(substate1);
                    newstate.push_back(std::move(newsubstate0));

                    ColouringBitArray newsubstate1=std::move(substate2);
                    newsubstate1.concatenate_to_special(false_state);
                    newsubstate1.concatenate_to_special(substate0);
                    newstate.push_back(std::move(newsubstate1));

                    ColouringBitArray newsubstate2=std::move(substate1);
                    newsubstate2.concatenate_to_special(substate0);
                    newsubstate2.concatenate_to_special(false_state);
                    newstate.push_back(std::move(newsubstate2));
                }
            state=std::move(newstate);
            return;
        }



        if (posa==0 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()*3);
            for(uint_fast64_t k=0; k<state.size();k++) {
                auto newsubstates=state[k].split3(posb2);
                newstate.push_back(std::move(std::get<0>(newsubstates)));
                newstate.push_back(std::move(std::get<1>(newsubstates)));
                newstate.push_back(std::move(std::get<2>(newsubstates)));
            }
            state=std::move(newstate);
            return;
        }



        if (posa==1 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/9);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<exp1[posb2]; j+=exp1[posa2+1]) 
                    for(uint_fast64_t k=j;k<state.size(); k+=exp1[posb2+1]) {
                        ColouringBitArray helpsubstate00=std::move(state[k+0*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate11=std::move(state[k+1*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate22=std::move(state[k+2*exp1[posa2]+2*exp1[posb2]]);
                        ColouringBitArray newsubstate=helpsubstate11;
                        newsubstate|=helpsubstate22;
                        newsubstate.concatenate_to_special(state[k+1*exp1[posa2]+0*exp1[posb2]]);
                        newsubstate.concatenate_to_special(state[k+2*exp1[posa2]+0*exp1[posb2]]);
                        newsubstate.concatenate_to_special(state[k+0*exp1[posa2]+1*exp1[posb2]]);
                        helpsubstate22|=helpsubstate00; //we do not need helpsubstate22 any more
                        newsubstate.concatenate_to_special(helpsubstate22);
                        newsubstate.concatenate_to_special(state[k+2*exp1[posa2]+1*exp1[posb2]]);
                        newsubstate.concatenate_to_special(state[k+0*exp1[posa2]+2*exp1[posb2]]);
                        newsubstate.concatenate_to_special(state[k+1*exp1[posa2]+2*exp1[posb2]]);
                        helpsubstate00|=helpsubstate11;
                        newsubstate.concatenate_to_special(helpsubstate00);
                        newstate.push_back(std::move(newsubstate));
                    }
            state=std::move(newstate);
            return;
        }


        if (posa==1 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/3);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posa2+1]) {
                    auto [h00, h01, h02]=state[j].split3(posb2);
                    auto [h10, h11, h12]=state[j+exp1[posa2]].split3(posb2);
                    auto [h20, h21, h22]=state[j+2*exp1[posa2]].split3(posb2);
                    (void) h00; (void) h11; (void) h22;
                    h12|=h21;
                    h02|=h20;
                    h01|=h10;
                    ColouringBitArray newsubstate=std::move(h12);
                    newsubstate.concatenate_to_special(h02); 
                    newsubstate.concatenate_to_special(h01);                     
                    newstate.push_back(std::move(newsubstate));
                }
            state=std::move(newstate);
            return;
        }


        if (posa==2 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size());
            for(uint_fast64_t i=0;i<state.size();i++) {
                    auto [h0, h1, h2]=state[i].split3(posb2);
                    auto h00=std::get<0>(h0.split3(posa2));
                    auto h11=std::get<1>(h1.split3(posa2));
                    auto h22=std::get<2>(h2.split3(posa2));
                    h00|=h11;
                    h00|=h22;
                    newstate.push_back(std::move(h00)); 
            }
            state=std::move(newstate);
            return;
        }
     
    }




    bool isColourable_arbitrary(const Graph &g) {
        PathDecomposition pd;
        for(auto i:g.list(RP::all(), IP::primary()))
           pd.ordered_edges.push_back(i->e());
        return isColourable(g, pd);
    }    


    bool isColourable_shortest_path_heuristic(const Graph &g) {
        auto pd=shortest_path_heuristic(g);
        return isColourable(g, pd);
    }  

    bool isColourable(const Graph &g, const PathDecomposition &pd) {
        initialize(g);
        for(auto i: pd.ordered_edges)
           process_state(i.v1(),i.v2());
        for(unsigned int i=0; i<state.size();i++)
            if(!state[i].all_false()) return true;    
        return false;
    }    

    bool isColourable(const Graph &g) {
        return isColourable_arbitrary(g);
    }


/*****************
signed coloring
******************/

    void process_state_signed(Vertex va, Vertex vb) {         
        int posa, posb, posa2, posb2;
        locate_and_swap(va, vb, posa, posb, posa2, posb2);
        process_state_signed(va, vb, posa, posb, posa2, posb2);
    }

    
    void process_state_signed(Vertex va, Vertex vb, int posa, int posb, int &posa2, int &posb2) {         
        process_sets(va, vb, posa, posb, posa2, posb2);
        process_just_state_signed(posa, posb, posa2, posb2);
    }

    void process_just_state_signed(int posa, int posb, int &posa2, int &posb2) {         
        if (posa==0 && posb==0) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()*9);
            for(uint_fast64_t i=0; i<state.size();i++) {
                //compute state
                newstate.push_back(state[i]);
                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(ColouringBitArray(state[i].size(), false));

                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(state[i]);

                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(std::move(state[i])); //we do not need the state any longer
                newstate.push_back(ColouringBitArray(state[i].size(), false));
            }
            state=std::move(newstate);
            return;
        }


        if (posa==0 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            for(uint_fast64_t i=0;i<exp1[posb2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posb2+1]) {
                    uint_fast64_t s0=j;
                    uint_fast64_t s1=j+exp1[posb2];
                    uint_fast64_t s2=j+2*exp1[posb2];
                    ColouringBitArray substate0=std::move(state[s0]);
                    ColouringBitArray substate1=std::move(state[s1]);
                    ColouringBitArray substate2=std::move(state[s2]);
                    ColouringBitArray false_state=ColouringBitArray(substate0.size(), false);                  

                    ColouringBitArray newsubstate0=false_state;
                    newsubstate0.concatenate_to_special(substate2);
                    newsubstate0.concatenate_to_special(substate1);
                    newstate.push_back(std::move(newsubstate0));

                    ColouringBitArray newsubstate1=std::move(substate1);
                    newsubstate1.concatenate_to_special(substate0);
                    newsubstate1.concatenate_to_special(false_state);
                    newstate.push_back(std::move(newsubstate1));

                    ColouringBitArray newsubstate2=std::move(substate2);
                    newsubstate2.concatenate_to_special(false_state);
                    newsubstate2.concatenate_to_special(substate0);
                    newstate.push_back(std::move(newsubstate2));
                }
            state=std::move(newstate);
            return;
        }



        if (posa==0 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()*3);
            for(uint_fast64_t k=0; k<state.size();k++) {
                auto newsubstates=state[k].split3(posb2);
                newstate.push_back(std::move(std::get<0>(newsubstates)));
                newstate.push_back(std::move(std::get<2>(newsubstates)));
                newstate.push_back(std::move(std::get<1>(newsubstates)));
            }
            state=std::move(newstate);
            return;
        }



        if (posa==1 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/9);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<exp1[posb2]; j+=exp1[posa2+1]) 
                    for(uint_fast64_t k=j;k<state.size(); k+=exp1[posb2+1]) {
                        ColouringBitArray helpsubstate00=std::move(state[k+0*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate12=std::move(state[k+1*exp1[posa2]+2*exp1[posb2]]);
                        ColouringBitArray helpsubstate21=std::move(state[k+2*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray newsubstate=helpsubstate12;
                        newsubstate|=helpsubstate21;
                        newsubstate.concatenate_to_special(state[k+2*exp1[posa2]+0*exp1[posb2]]);
                        newsubstate.concatenate_to_special(state[k+1*exp1[posa2]+0*exp1[posb2]]);

                        newsubstate.concatenate_to_special(state[k+0*exp1[posa2]+2*exp1[posb2]]);
                        newsubstate.concatenate_to_special(state[k+2*exp1[posa2]+2*exp1[posb2]]);
                        helpsubstate21|=helpsubstate00; 
                        newsubstate.concatenate_to_special(helpsubstate21);

                        newsubstate.concatenate_to_special(state[k+0*exp1[posa2]+1*exp1[posb2]]);
                        helpsubstate12|=helpsubstate00;
                        newsubstate.concatenate_to_special(helpsubstate12);
                        newsubstate.concatenate_to_special(state[k+1*exp1[posa2]+1*exp1[posb2]]);
                        newstate.push_back(std::move(newsubstate));
                    }
            state=std::move(newstate);
            return;
        }


        if (posa==1 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/3);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posa2+1]) {
                    auto [h00, h01, h02]=state[j].split3(posb2);
                    auto [h10, h11, h12]=state[j+exp1[posa2]].split3(posb2);
                    auto [h20, h21, h22]=state[j+2*exp1[posa2]].split3(posb2);
                    (void) h00; (void) h12; (void) h21;
                    h11|=h22;
                    h01|=h20;
                    h02|=h10;
                    ColouringBitArray newsubstate=std::move(h11);
                    newsubstate.concatenate_to_special(h01); 
                    newsubstate.concatenate_to_special(h02);                     
                    newstate.push_back(std::move(newsubstate));
                }
            state=std::move(newstate);
            return;
        }


        if (posa==2 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size());
            for(uint_fast64_t i=0;i<state.size();i++) {
                    auto [h0, h1, h2]=state[i].split3(posb2);
                    auto h00=std::get<0>(h0.split3(posa2));
                    auto h12=std::get<2>(h1.split3(posa2));
                    auto h21=std::get<1>(h2.split3(posa2));
                    h00|=h12;
                    h00|=h21;
                    newstate.push_back(std::move(h00)); 
            }
            state=std::move(newstate);
            return;
        }
     
    }




    bool isColourable_arbitrary(const Graph &g, std::set<Edge> sign) {
        PathDecomposition pd;
        for(auto i:g.list(RP::all(), IP::primary()))
           pd.ordered_edges.push_back(i->e());
        return isColourable(g, sign, pd);
    }    


    bool isColourable_shortest_path_heuristic(const Graph &g, std::set<Edge> sign) {
        auto pd=shortest_path_heuristic(g);
        return isColourable(g, sign, pd);
    }  

    bool isColourable(const Graph &g, std::set<Edge> sign, const PathDecomposition &pd) {
        initialize(g);
        for(auto i: pd.ordered_edges)
            if (sign.count(i))
                process_state_signed(i.v1(),i.v2());
            else
                process_state(i.v1(),i.v2());
        for(unsigned int i=0; i<state.size();i++)
            if(!state[i].all_false()) return true;    
        return false;
    }    

    bool isColourable(const Graph &g, std::set<Edge> sign) {
        return isColourable_arbitrary(g, sign);
    }


/*****************
resistance
******************/

    void process_just_state_with_resistance_error(int posa, int posb, int &posa2, int &posb2) {         
        if (posa==0 && posb==0) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()*9);
            for(uint_fast64_t i=0; i<state.size();i++) {
                //compute state
                newstate.push_back(state[i]);
                newstate.push_back(state[i]);
                newstate.push_back(state[i]);

                newstate.push_back(state[i]);
                newstate.push_back(state[i]);
                newstate.push_back(state[i]);

                newstate.push_back(state[i]);
                newstate.push_back(state[i]);
                newstate.push_back(std::move(state[i])); //we do not need the state any longer
            }
            state=std::move(newstate);
            return;
        }


        if (posa==0 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            for(uint_fast64_t i=0;i<exp1[posb2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posb2+1]) {
                    uint_fast64_t s0=j;
                    uint_fast64_t s1=j+exp1[posb2];
                    uint_fast64_t s2=j+2*exp1[posb2];
                    ColouringBitArray substate0=std::move(state[s0]);
                    ColouringBitArray substate1=std::move(state[s1]);
                    ColouringBitArray substate2=std::move(state[s2]);

                    ColouringBitArray substate0or1=substate0; substate0or1|=substate1;
                    ColouringBitArray substate0or2=std::move(substate0); substate0or2|=substate2;
                    ColouringBitArray substate1or2=std::move(substate1); substate1or2|=substate2;
                    

                    ColouringBitArray newsubstate=substate1or2;
                    newsubstate.concatenate_to_special(substate0or2);
                    newsubstate.concatenate_to_special(substate0or1);
                    newstate.push_back(newsubstate);
                    newstate.push_back(newsubstate);
                    newstate.push_back(std::move(newsubstate));
                }
            state=std::move(newstate);
            return;
        }



        if (posa==0 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()*3);
            for(uint_fast64_t k=0; k<state.size();k++) {
                auto newsubstates=state[k].split3(posb2);
                std::get<0>(newsubstates)|=std::get<1>(newsubstates);
                std::get<0>(newsubstates)|=std::get<2>(newsubstates);
                
                newstate.push_back(std::get<0>(newsubstates));
                newstate.push_back(std::get<0>(newsubstates));
                newstate.push_back(std::move(std::get<0>(newsubstates)));
            }
            state=std::move(newstate);
            return;
        }



        if (posa==1 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/9);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<exp1[posb2]; j+=exp1[posa2+1]) 
                    for(uint_fast64_t k=j;k<state.size(); k+=exp1[posb2+1]) {
                        ColouringBitArray helpsubstate00=std::move(state[k+0*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate01=std::move(state[k+0*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate02=std::move(state[k+0*exp1[posa2]+2*exp1[posb2]]);
                        ColouringBitArray helpsubstate10=std::move(state[k+1*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate11=std::move(state[k+1*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate12=std::move(state[k+1*exp1[posa2]+2*exp1[posb2]]);
                        ColouringBitArray helpsubstate20=std::move(state[k+2*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate21=std::move(state[k+2*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate22=std::move(state[k+2*exp1[posa2]+2*exp1[posb2]]);

                        ColouringBitArray newsubstate=helpsubstate11;
                        newsubstate|=helpsubstate12;
                        newsubstate|=helpsubstate21;
                        newsubstate|=helpsubstate22;
                        ColouringBitArray h;
                        h =helpsubstate10;
                        h|=helpsubstate12;
                        h|=helpsubstate20;
                        h|=helpsubstate22;
                        newsubstate.concatenate_to_special(h);
                        h =helpsubstate10;
                        h|=helpsubstate11;
                        h|=helpsubstate20;
                        h|=helpsubstate21;
                        newsubstate.concatenate_to_special(h);
                        h =helpsubstate01;
                        h|=helpsubstate02;
                        h|=helpsubstate21;
                        h|=helpsubstate22;
                        newsubstate.concatenate_to_special(h);
                        h =helpsubstate00;
                        h|=helpsubstate02;
                        h|=helpsubstate20;
                        h|=helpsubstate22;
                        newsubstate.concatenate_to_special(h);
                        h =helpsubstate00;
                        h|=helpsubstate01;
                        h|=helpsubstate20;
                        h|=helpsubstate21;
                        newsubstate.concatenate_to_special(h);
                        h =helpsubstate01;
                        h|=helpsubstate02;
                        h|=helpsubstate11;
                        h|=helpsubstate12;
                        newsubstate.concatenate_to_special(h);
                        h =helpsubstate00;
                        h|=helpsubstate02;
                        h|=helpsubstate10;
                        h|=helpsubstate12;
                        newsubstate.concatenate_to_special(h);
                        h =helpsubstate00;
                        h|=helpsubstate01;
                        h|=helpsubstate10;
                        h|=helpsubstate11;
                        newsubstate.concatenate_to_special(h);
                        newstate.push_back(std::move(newsubstate));
                    }
            state=std::move(newstate);
            return;
        }


        if (posa==1 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/3);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posa2+1]) {
                    auto [h00, h01, h02]=state[j].split3(posb2);
                    auto [h10, h11, h12]=state[j+exp1[posa2]].split3(posb2);
                    auto [h20, h21, h22]=state[j+2*exp1[posa2]].split3(posb2);
                    h00|=h01;
                    h00|=h02;
                    h11|=h10;
                    h11|=h12;
                    h22|=h20;
                    h22|=h21;
                    ColouringBitArray newsubstate=h11;
                    newsubstate|=h22;
                    h22|=h00;
                    h11|=h00;
                    newsubstate.concatenate_to_special(h22); 
                    newsubstate.concatenate_to_special(h11);                     
                    newstate.push_back(std::move(newsubstate));
                }
            state=std::move(newstate);
            return;
        }


        if (posa==2 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size());
            for(uint_fast64_t i=0;i<state.size();i++) {
                    auto [h0, h1, h2]=state[i].split3(posb2);
                    auto [h00, h01, h02]=h0.split3(posa2);
                    auto [h10, h11, h12]=h1.split3(posa2);
                    auto [h20, h21, h22]=h2.split3(posa2);
                    h00|=h01;
                    h00|=h02;
                    h00|=h10;
                    h00|=h11;
                    h00|=h12;
                    h00|=h20;
                    h00|=h21;
                    h00|=h22;
                    newstate.push_back(std::move(h00)); 
            }
            state=std::move(newstate);
            return;
        }
     
    }


    int resistance_arbitrary(const Graph &g, int max_resistance) {
        PathDecomposition pd;
        for(auto i:g.list(RP::all(), IP::primary()))
           pd.ordered_edges.push_back(i->e());
        return resistance(g, pd, max_resistance);
    }    


    int resistance_shortest_path_heuristic(const Graph &g, int max_resistance) {
        auto pd=shortest_path_heuristic(g);
        return resistance(g, pd, max_resistance);
    }  

    int resistance(const Graph &g, const PathDecomposition &pd, int max_resistance) {
        initialize(g);
        std::vector<std::vector<ColouringBitArray>> rstate(max_resistance);
        for(int i=0;i<max_resistance;i++)
            rstate[i]=state;

        for(auto e: pd.ordered_edges) {
            int posa, posb, posa2, posb2;
            Vertex v1=e.v1();
            Vertex v2=e.v2();
            locate_and_swap(v1, v2, posa, posb, posa2, posb2);
            for(int i=max_resistance-1;i>=0;i--) {
                std::swap(state, rstate[i]);              
                process_just_state(posa, posb, posa2, posb2);
                std::swap(state, rstate[i]);              
                if (i!=0) {
                    state=rstate[i-1];
                    process_just_state_with_resistance_error(posa, posb, posa2, posb2);
                    for(unsigned int j=0;j<state.size();j++)
                        rstate[i][j]|=state[j];
                }
            }
            process_sets(v1, v2, posa, posb, posa2, posb2);
        }

        for(int i=0;i<max_resistance;i++)
            if (rstate[i][0][ColouringBitArray::Index(0,0)]) return i;
        return max_resistance;    
    }

    int resistance(const Graph &g, int max_resistance) {
        return resistance_arbitrary(g, max_resistance);
    }





/*****************
strong oddness
******************/

    void process_just_state_with_strong_oddness_error(int posa, int posb, int &posa2, int &posb2) {         
        if (posa==0 && posb==0) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()*9);
            for(uint_fast64_t i=0; i<state.size();i++) {
                //compute state
                newstate.push_back(state[i]);
                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(ColouringBitArray(state[i].size(), false));

                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(state[i]);
                newstate.push_back(state[i]);

                newstate.push_back(ColouringBitArray(state[i].size(), false));
                newstate.push_back(state[i]);
                newstate.push_back(std::move(state[i])); //we do not need the state any longer
            }
            state=std::move(newstate);
            return;
        }


        if (posa==0 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            for(uint_fast64_t i=0;i<exp1[posb2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posb2+1]) {
                    uint_fast64_t s0=j;
                    uint_fast64_t s1=j+exp1[posb2];
                    uint_fast64_t s2=j+2*exp1[posb2];
                    ColouringBitArray substate0=std::move(state[s0]);
                    ColouringBitArray substate1=std::move(state[s1]);
                    ColouringBitArray substate2=std::move(state[s2]);
                    

                    ColouringBitArray newsubstate=ColouringBitArray(substate0.size(), false);
                    newsubstate.concatenate_to_special(substate2);
                    newsubstate.concatenate_to_special(substate1);
                    newstate.push_back(newsubstate);

                    newsubstate=std::move(substate1); substate1|=substate2;
                    newsubstate.concatenate_to_special(substate0);
                    newsubstate.concatenate_to_special(substate0);
                    newstate.push_back(newsubstate);
                    newstate.push_back(std::move(newsubstate));
                }
            state=std::move(newstate);
            return;
        }



        if (posa==0 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()*3);
            for(uint_fast64_t k=0; k<state.size();k++) {
                auto newsubstates=state[k].split3(posb2);
                newstate.push_back(std::get<0>(newsubstates));
                std::get<1>(newsubstates)|=std::get<2>(newsubstates);
                newstate.push_back(std::get<1>(newsubstates));
                newstate.push_back(std::move(std::get<1>(newsubstates)));
            }
            state=std::move(newstate);
            return;
        }



        if (posa==1 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/9);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<exp1[posb2]; j+=exp1[posa2+1]) 
                    for(uint_fast64_t k=j;k<state.size(); k+=exp1[posb2+1]) {
                        ColouringBitArray helpsubstate00=std::move(state[k+0*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate01=std::move(state[k+0*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate02=std::move(state[k+0*exp1[posa2]+2*exp1[posb2]]);
                        ColouringBitArray helpsubstate10=std::move(state[k+1*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate11=std::move(state[k+1*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate12=std::move(state[k+1*exp1[posa2]+2*exp1[posb2]]);
                        ColouringBitArray helpsubstate20=std::move(state[k+2*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate21=std::move(state[k+2*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate22=std::move(state[k+2*exp1[posa2]+2*exp1[posb2]]);

                        ColouringBitArray newsubstate=helpsubstate11;
                        newsubstate|=helpsubstate12;
                        newsubstate|=helpsubstate21;
                        newsubstate|=helpsubstate22;
                        ColouringBitArray h;
                        h =std::move(helpsubstate10);
                        h|=helpsubstate20;
                        newsubstate.concatenate_to_special(h);
                        newsubstate.concatenate_to_special(h);
                        helpsubstate01|=helpsubstate02;
                        newsubstate.concatenate_to_special(helpsubstate01);
                        h =std::move(helpsubstate22);
                        h|=helpsubstate00;
                        newsubstate.concatenate_to_special(h);
                        h =std::move(helpsubstate21);
                        h|=helpsubstate00;
                        newsubstate.concatenate_to_special(h);
                        newsubstate.concatenate_to_special(helpsubstate01);
                        h =std::move(helpsubstate12);
                        h|=helpsubstate00;
                        newsubstate.concatenate_to_special(h);
                        h =std::move(helpsubstate11);
                        h|=helpsubstate00;
                        newsubstate.concatenate_to_special(h);
                        newstate.push_back(std::move(newsubstate));
                    }
            state=std::move(newstate);
            return;
        }


        if (posa==1 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/3);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posa2+1]) {
                    auto [h00, h01, h02]=state[j].split3(posb2);
                    auto [h10, h11, h12]=state[j+exp1[posa2]].split3(posb2);
                    auto [h20, h21, h22]=state[j+2*exp1[posa2]].split3(posb2);
                    (void) h00;
                    ColouringBitArray newsubstate=std::move(h11);
                    newsubstate|=h22;
                    newsubstate|=h12;
                    newsubstate|=h21;
                    h01|=h02;
                    h20|=h01;
                    h10|=h01;
                    newsubstate.concatenate_to_special(h20); 
                    newsubstate.concatenate_to_special(h10);                     
                    newstate.push_back(std::move(newsubstate));
                }
            state=std::move(newstate);
            return;
        }


        if (posa==2 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size());
            for(uint_fast64_t i=0;i<state.size();i++) {
                    auto [h0, h1, h2]=state[i].split3(posb2);
                    auto h00=std::get<0>(h0.split3(posa2));
                    auto [h10, h11, h12]=h1.split3(posa2);
                    auto [h20, h21, h22]=h2.split3(posa2);
                    (void) h10; (void) h20;
                    h00|=h11;
                    h00|=h12;
                    h00|=h21;
                    h00|=h22;
                    newstate.push_back(std::move(h00)); 
            }
            state=std::move(newstate);
            return;
        }
     
    }


    int strong_oddness_arbitrary(const Graph &g, int max_oddness) {
        PathDecomposition pd;
        for(auto i:g.list(RP::all(), IP::primary()))
           pd.ordered_edges.push_back(i->e());
        return strong_oddness(g, pd, max_oddness);
    }    


    int strong_oddness_shortest_path_heuristic(const Graph &g, int max_oddness) {
        auto pd=shortest_path_heuristic(g);
        return strong_oddness(g, pd, max_oddness);
    }  



    int strong_oddness(const Graph &g, const PathDecomposition &pd, int max_oddness) {
        initialize(g);
        std::vector<std::vector<ColouringBitArray>> rstate(max_oddness);
        for(int i=0;i<max_oddness;i++)
            rstate[i]=state;

        for(auto e: pd.ordered_edges) {
            int posa, posb, posa2, posb2;
            Vertex v1=e.v1();
            Vertex v2=e.v2();
            locate_and_swap(v1, v2, posa, posb, posa2, posb2);

/*            std::cout<<"V1:"<<v1.to_int()-11<<" V2:"<<v2.to_int()-11;
            std::cout<<" Pos"<<posa<<posb<<posa2<<posb2<<" Poradie:";
            for(unsigned int j=0;j<v[1].size();j++) std::cout<<v[1][j].to_int()-11;
            std::cout<<" ";
            for(unsigned int j=0;j<v[2].size();j++) std::cout<<v[2][j].to_int()-11;
            std::cout<<" Velkost stavu:"<<rstate[0].size()<<","<<rstate[0][0].size().to_int64()<<"\n";*/

            for(int i=max_oddness-1;i>=0;i--) {
                std::swap(state, rstate[i]);              
                process_just_state(posa, posb, posa2, posb2);
                std::swap(state, rstate[i]);              
                if (i!=0) {
                    state=rstate[i-1];
                    process_just_state_with_strong_oddness_error(posa, posb, posa2, posb2);
                    for(unsigned int j=0;j<state.size();j++)
                        rstate[i][j]|=state[j];
                }
/*                if (i<=1) {
                    std::cout<<"Pocet chyb:"<<i<<"    ";
                    for(unsigned int j=0;j<rstate[i].size();j++) {
                        for(ColouringBitArray::Index k(0,0); k<rstate[i][j].size();k++)
                           if (rstate[i][j][k]) std::cout<<k.to_int64()<<",";
                        std::cout<<"|";
                    }
                    std::cout<<"\n";
                }*/
            }
            process_sets(v1, v2, posa, posb, posa2, posb2);

/*            std::cout<<"Nove poradie:";
            for(unsigned int j=0;j<v[1].size();j++) std::cout<<v[1][j].to_int()-11;
            std::cout<<" ";
            for(unsigned int j=0;j<v[2].size();j++) std::cout<<v[2][j].to_int()-11<<"";
            std::cout<<"\n--------------------------------\n";*/
        }

        for(int i=0;i<max_oddness;i++)
            if (rstate[i][0][ColouringBitArray::Index(0,0)]) return i;
        return max_oddness;    
    }    

    int strong_oddness(const Graph &g, int max_oddness) {
        return strong_oddness(g, max_oddness);
    }


/*****************
weak oddness
******************/

    //we introduce edge colouring errors when vertex moves from v[1] to v[2], here we may make two errors
    void process_just_state_with_weak_oddness_two_errors(int posa, int posb, int &posa2, int &posb2) {         
        if (posa==1 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/9);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<exp1[posb2]; j+=exp1[posa2+1]) 
                    for(uint_fast64_t k=j;k<state.size(); k+=exp1[posb2+1]) {
                        ColouringBitArray helpsubstate00=std::move(state[k+0*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate01=std::move(state[k+0*exp1[posa2]+1*exp1[posb2]]);
                        helpsubstate01 |= state[k+0*exp1[posa2]+2*exp1[posb2]];
                        helpsubstate01 |= state[k+1*exp1[posa2]+0*exp1[posb2]];
                        helpsubstate00 |= state[k+1*exp1[posa2]+1*exp1[posb2]];
                        helpsubstate00 |= state[k+1*exp1[posa2]+2*exp1[posb2]];
                        helpsubstate01 |= state[k+2*exp1[posa2]+0*exp1[posb2]];
                        helpsubstate00 |= state[k+2*exp1[posa2]+1*exp1[posb2]];
                        helpsubstate00 |= state[k+2*exp1[posa2]+2*exp1[posb2]];


                        ColouringBitArray newsubstate=helpsubstate00;
                        newsubstate.concatenate_to_special(helpsubstate01);
                        newsubstate.concatenate_to_special(helpsubstate01);

                        newsubstate.concatenate_to_special(helpsubstate01);
                        newsubstate.concatenate_to_special(helpsubstate00);
                        newsubstate.concatenate_to_special(helpsubstate00);

                        newsubstate.concatenate_to_special(helpsubstate01);
                        newsubstate.concatenate_to_special(helpsubstate00);
                        newsubstate.concatenate_to_special(helpsubstate00);
                        newstate.push_back(std::move(newsubstate));
                    }
            state=std::move(newstate);
            return;
        }
     
    }

    void process_just_state_with_weak_oddness_error(int posa, int posb, int &posa2, int &posb2) {         
        if (posa!=1 && posb!=1)  //no error introduced
            process_just_state(posa, posb, posa2, posb2);


        if (posa==0 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            for(uint_fast64_t i=0;i<exp1[posb2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posb2+1]) {
                    ColouringBitArray substate0=std::move(state[j]);
                    ColouringBitArray substate1=std::move(state[j+exp1[posb2]]);
                    substate1|=state[j+2*exp1[posb2]];
                    

                    ColouringBitArray newsubstate=substate0;
                    newsubstate.concatenate_to_special(substate1);
                    newsubstate.concatenate_to_special(substate1);
                    newstate.push_back(newsubstate);

                    newsubstate=std::move(substate1);
                    newsubstate.concatenate_to_special(substate0);
                    newsubstate.concatenate_to_special(substate0);
                    newstate.push_back(newsubstate);
                    newstate.push_back(std::move(newsubstate));
                }
            state=std::move(newstate);
            return;
        }


        if (posa==1 && posb==1) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/9);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<exp1[posb2]; j+=exp1[posa2+1]) 
                    for(uint_fast64_t k=j;k<state.size(); k+=exp1[posb2+1]) {
                        ColouringBitArray helpsubstate00=std::move(state[k+0*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate01=std::move(state[k+0*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate02=std::move(state[k+0*exp1[posa2]+2*exp1[posb2]]);
                        ColouringBitArray helpsubstate10=std::move(state[k+1*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate11=std::move(state[k+1*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate12=std::move(state[k+1*exp1[posa2]+2*exp1[posb2]]);
                        ColouringBitArray helpsubstate20=std::move(state[k+2*exp1[posa2]+0*exp1[posb2]]);
                        ColouringBitArray helpsubstate21=std::move(state[k+2*exp1[posa2]+1*exp1[posb2]]);
                        ColouringBitArray helpsubstate22=std::move(state[k+2*exp1[posa2]+2*exp1[posb2]]);

                        ColouringBitArray newsubstate=helpsubstate11;
                        newsubstate|=helpsubstate12;
                        newsubstate|=helpsubstate21;
                        newsubstate|=helpsubstate22; //0,0
                        ColouringBitArray h;
                        h =helpsubstate10;
                        h|=helpsubstate20;
                        h|=helpsubstate02;
                        newsubstate.concatenate_to_special(h); //0,1
                        h =helpsubstate20;
                        h|=helpsubstate10;
                        h|=helpsubstate01;
                        newsubstate.concatenate_to_special(h); //0,2
                        h =helpsubstate01;
                        h|=helpsubstate20;
                        h|=helpsubstate02;
                        newsubstate.concatenate_to_special(h); //1,0
                        h =helpsubstate00;
                        h|=helpsubstate12;
                        h|=helpsubstate21;
                        h|=helpsubstate22;
                        newsubstate.concatenate_to_special(h); //1,1
                        h =helpsubstate00;
                        h|=helpsubstate11;
                        h|=helpsubstate21;
                        h|=helpsubstate22;
                        newsubstate.concatenate_to_special(h); //1,2
                        h =helpsubstate02;
                        h|=helpsubstate10;
                        h|=helpsubstate01;
                        newsubstate.concatenate_to_special(h); //2,0
                        h =helpsubstate00;
                        h|=helpsubstate11;
                        h|=helpsubstate12;
                        h|=helpsubstate22;
                        newsubstate.concatenate_to_special(h); //2,1
                        h =helpsubstate00;
                        h|=helpsubstate12;
                        h|=helpsubstate21;
                        h|=helpsubstate11;
                        newsubstate.concatenate_to_special(h); //2,2
                        newstate.push_back(std::move(newsubstate));
                    }
            state=std::move(newstate);
            return;
        }


        if (posa==1 && posb==2) {
            std::vector<ColouringBitArray> newstate;
            newstate.reserve(state.size()/3);
            for(uint_fast64_t i=0;i<exp1[posa2];i++)
                for(uint_fast64_t j=i;j<state.size(); j+=exp1[posa2+1]) {
                    auto [h00, h01, h02]=state[j].split3(posb2);
                    auto [h10, h11, h12]=state[j+exp1[posa2]].split3(posb2);
                    auto [h20, h21, h22]=state[j+2*exp1[posa2]].split3(posb2);
                    ColouringBitArray newsubstate=std::move(h00);
                    newsubstate|=h11;
                    newsubstate|=h22;
                    newsubstate|=h12;
                    newsubstate|=h21;
                    h01|=h02;
                    h01|=h10;
                    h01|=h20;
                    newsubstate.concatenate_to_special(h01); 
                    newsubstate.concatenate_to_special(h01);                     
                    newstate.push_back(std::move(newsubstate));
                }
            state=std::move(newstate);
            return;
        }
    }


    int weak_oddness_arbitrary(const Graph &g, int max_oddness) {
        PathDecomposition pd;
        for(auto i:g.list(RP::all(), IP::primary()))
           pd.ordered_edges.push_back(i->e());
        return weak_oddness(g, pd, max_oddness);
    }    


    int weak_oddness_shortest_path_heuristic(const Graph &g, int max_oddness) {
        auto pd=shortest_path_heuristic(g);
        return weak_oddness(g, pd, max_oddness);
    }  

    int weak_oddness(const Graph &g, const PathDecomposition &pd, int max_oddness) {
        initialize(g);
        std::vector<std::vector<ColouringBitArray>> rstate(max_oddness);
        for(int i=0;i<max_oddness;i++)
            rstate[i]=state;

        for(auto e: pd.ordered_edges) {
            int posa, posb, posa2, posb2;
            Vertex v1=e.v1();
            Vertex v2=e.v2();
            locate_and_swap(v1, v2, posa, posb, posa2, posb2);
            for(int i=max_oddness-1;i>=0;i--) {
                std::swap(state, rstate[i]);              
                process_just_state(posa, posb, posa2, posb2);
                std::swap(state, rstate[i]);              
                if (i!=0) {
                    state=rstate[i-1];
                    process_just_state_with_weak_oddness_error(posa, posb, posa2, posb2);
                    for(unsigned int j=0;j<state.size();j++)
                        rstate[i][j]|=state[j];
                }
                if (i>1 && posa==1 && posb==1) {
                    state=rstate[i-2];
                    process_just_state_with_weak_oddness_two_errors(posa, posb, posa2, posb2);
                    for(unsigned int j=0;j<state.size();j++)
                        rstate[i][j]|=state[j];
                }
            }
            process_sets(v1, v2, posa, posb, posa2, posb2);
        }

        for(int i=0;i<max_oddness;i++)
            if (rstate[i][0][ColouringBitArray::Index(0,0)]) return i;
        return max_oddness;    
    }    

    int weak_oddness(const Graph &g, int max_oddness) {
        return strong_oddness(g, max_oddness);
    }

};



} //end namespace ba_graph

#endif
