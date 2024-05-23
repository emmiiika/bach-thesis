#ifndef BA_GRAPH_INCIDENCE_CPP
#define BA_GRAPH_INCIDENCE_CPP
#include "graph.h"
#include "incidence_predicates.hpp"
#include <algorithm>

namespace ba_graph {

/*
Incidence
*/

inline HalfEdge Incidence::h1() const { return HalfEdge(v1(), v2(), id_code_); }
inline HalfEdge Incidence::h2() const { return HalfEdge(v2(), v1(), -id_code_); }

inline Edge Incidence::e() const { return h1().e(); }

inline Rotation& Incidence::r1() const { return *r_; }
inline Rotation& Incidence::r2() const { return *(r2_); }

inline Vertex Incidence::v1() const { return r_->v();}
inline Vertex Incidence::v2() const { return r2_->v();}

inline Number Incidence::n1() const { return r_->n(); }
inline Number Incidence::n2() const { return r2_->n(); }
inline Location  Incidence::l() const {
    if (n1()<n2()) {
        IncidenceIterator i=r_->find(h1());
        return Location(n1(),n2(), std::count_if(r1().begin(),i, IP::IP(n2())));
    }
    if (n1()>n2()) {
        IncidenceIterator i=r2().find(h2());
        return Location(n1(),n2(), std::count_if(r2().begin(),i, IP::IP(n1())));
    }
    if (n1()==n2() && is_primary()) {        
        IncidenceIterator i=r1().find(h1());
        return Location(n1(),n2(), 2*std::count_if(r1().begin(),i, IP::IP(n2())));
    }
    IncidenceIterator i=r1().find(h2());
    return Location(n1(),n2(), 2*std::count_if(r1().begin(),i, IP::IP(n2()))+1);
}
inline Direction Incidence::d() const {
    if (is_primary()) {
        IncidenceIterator i=r_->find(h1());
        return Direction(n1(),n2(), std::count_if(r_->begin(),i, IP::IP(n2()) && IP::primary()));
    }
    IncidenceIterator i=r_->find(h2());
    return Direction(n1(),n2(), std::count_if(r_->begin(),i, IP::IP(n2()) && IP::primary()));
}


inline bool Incidence::is_primary() const { return id_code_>0; }
inline bool Incidence::is_loop() const { return r_==r2_; }


inline const Incidence& Incidence::jump() const { 
    IncidenceIterator it=std::find_if(r2_->begin(), r2_->end(), IP::IP(h2()));
#ifdef BA_GRAPH_DEBUG
    assert(it!=r2_->end());
#endif
    return *it;
}
inline const Incidence& Incidence::right() const {  //slow
    IncidenceIterator it=std::find_if(r_->begin(), r_->end(),IP::IP(h1()));
#ifdef BA_GRAPH_DEBUG
    assert(it!=r_->end());
#endif
    ++it;
    if (it==r_->end()) it=r_->begin();
    return *it;
}
inline const Incidence& Incidence::left() const {  //slow
    IncidenceIterator it=std::find_if(r_->begin(), r_->end(),IP::IP(h1()));
#ifdef BA_GRAPH_DEBUG
    assert(it!=r_->end());
#endif
    if (it==r_->begin()) it=r_->end();
    --it;
    return *it;
}



}  // namespace end
#endif
