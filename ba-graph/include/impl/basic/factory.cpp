#ifndef BA_GRAPH_FACTORY_CPP
#define BA_GRAPH_FACTORY_CPP
#include "factory.h"
#include <algorithm>
#include <iterator>

namespace ba_graph {

inline Factory::Factory() { nextId_=1; }
inline Factory::~Factory() {}

inline Graph Factory::cG() { 
#ifdef BA_GRAPH_DEBUG    
    return Graph(this); 
#else
    return Graph(); 
#endif
}

inline Vertex Factory::cV() { return Vertex(Id(nextId_++)); }

inline Edge Factory::cE(const Vertex& v1, const Vertex& v2) { return Edge(v1, v2, Id(nextId_++)); }


inline void Factory::dG(Graph &g) {
    //delete Incedences and Rotations
    for (const auto &i : g.rr_) {
        for(auto j : i.second->rr_)
            delete &j;
        delete i.second;
    }
    g.rr_=std::map<int, Rotation *>();
    g.edge_count=0;
}


/*
Adding and deleting vertices
*/

inline Rotation& Factory::aV(Graph &g, const Vertex& v, Number i) { //specifies vertex number
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    assert(!g.contains(v));
    assert(!g.contains(i));
#endif
    if (g.free_number<=i.to_int()) g.free_number=i.to_int()+1; //optimizer should take care of this sometimes
    Rotation *r=new Rotation(v, i);
    g.rr_.insert(std::pair<const int,Rotation *>(i.to_int(),r));
    return *r;
}

inline Number Factory::aVs(Graph &g, int count) { //returns first ID
    int j=g.free_number;
    for(int i=0;i<count;i++) aV(g, g.free_number++);
    return j;
}

inline void Factory::aVs(Graph &g, int count, Number i) {
    for(int j=0;j<count;j++) aV(g,i+j);
}

inline void Factory::aVs(Graph &g, const std::vector<Vertex> &vv, Number i) {
    for(auto v:vv) {aV(g,v,i);i=i+1;}
}

inline Number Factory::aVs(Graph &g, const std::vector<Vertex> &vv) {
    int j=g.free_number;
    for(auto v:vv) aV(g,v,g.free_number++);
    return j;
}

inline void Factory::aVs(Graph &g, const std::vector<Number> &vn) {
    for(auto n:vn) aV(g,n);
}

inline void Factory::aVs(Graph &g, const std::vector<Vertex> &vv, const std::vector<Number> &vn) {
#ifdef BA_GRAPH_DEBUG
    assert(vn.size()==vv.size());
#endif
    auto it1=vv.begin();
    auto it2=vn.begin();
    for(;it1!=vv.end();it1++,it2++) aV(g,*it1, *it2);
}


inline Vertex Factory::dV(Graph &g, Number i) { //also destroys the neigbhbourhood
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    assert(g.contains(i));
#endif
    Rotation &r=*g.rr_.at(i.to_int());
    std::vector<Edge> to_delete;
    for (auto it=r.begin();it!=r.end();it++)
        if (!it->is_loop() || it->is_primary())
            to_delete.push_back(it->e());
    for (const auto &e :to_delete) dE(g,e);
    g.rr_.erase(i.to_int());
    Vertex v=r.v();
    delete &r;
    return v;
}


/*
Adding and deleting edges
*/

inline IncidenceRefPair Factory::aE(Graph &g, Rotation &r1, Rotation &r2, const Edge &e) {
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    int found=0;
    for(auto &rot: g) {
        if (&rot==&r1) found++;
        if (&rot==&r2) found++;
    }
    assert(found==2);
    assert(r1.v()==e.v1());
    assert(r2.v()==e.v2());
    assert(!r1.contains(e));
#endif
    Incidence *ip1=new Incidence(&r1, &r2, e.to_int());
    Incidence *ip2=new Incidence(&r2, &r1, -e.to_int());
    g.edge_count++;
    r1.rr_.push_back(ip1);
    r2.rr_.push_back(ip2);
    return IncidenceRefPair(*ip1,*ip2);
}

inline std::pair<IncidenceIterator, IncidenceIterator> Factory::aE(Graph &g,
        Rotation &r1, Rotation &r2,
        IncidenceIterator ii1, IncidenceIterator ii2, const Edge &e) {
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    int found=0;
    for(auto &rot: g) {
        if (&rot==&r1) found++;
        if (&rot==&r2) found++;
    }
    assert(found==2);
    assert((ii1.i_>=r1.begin().i_) && (ii1.i_<=r1.end().i_));
    assert(ii2.i_>=r2.begin().i_);
    assert(ii2.i_<=r2.end().i_);
    assert(r1.v()==e.v1());
    assert(r2.v()==e.v2());
    assert(!r1.contains(e));
#endif
    Incidence *ip1=new Incidence(&r1, &r2, e.to_int());
    Incidence *ip2=new Incidence(&r2, &r1, -e.to_int());
    g.edge_count++;
    //the erase does nothing, it is just a trick to remove const_ from ii1.i/ii2.i
    if (!e.is_loop()) {
        IncidenceIterator rii1(r1.rr_.insert(r1.rr_.erase(ii1.i_, ii1.i_), ip1));
        IncidenceIterator rii2(r2.rr_.insert(r2.rr_.erase(ii2.i_, ii2.i_), ip2));
        return std::pair(rii1,rii2);
    }
    if (ii1.i_<=ii2.i_) {
        size_t diff=ii2.i_-ii1.i_;
        IncidenceIterator rii(r1.rr_.insert(r1.rr_.erase(ii2.i_, ii2.i_), ip2));
        IncidenceIterator rii2(r1.rr_.insert(r1.rr_.erase(rii.i_-diff, rii.i_-diff), ip1));
        return std::pair(rii2,rii2+(diff+1)); //so that the iterator is valid
    }
    size_t diff=ii1.i_-ii2.i_;
    IncidenceIterator rii(r1.rr_.insert(r1.rr_.erase(ii1.i_, ii1.i_), ip1));
    IncidenceIterator rii2(r2.rr_.insert(r1.rr_.erase(rii.i_-diff, rii.i_-diff), ip2));
    return std::pair(rii2+(diff+1),rii2); //so that the iterator is valid
}



inline IncidenceRefPair  Factory::aE(Graph &g, Location l)  {
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    assert(g.contains(l.n1()));
    assert(g.contains(l.n2()));
    if (l.n1() != l.n2())
        assert(g.count(l.n1(),l.n2())>=l.index());
    else
        assert(g.count(l.n1(),l.n2())/2>=l.index()/2);
#endif
    auto &rot1=g[l.n1()];
    auto &rot2=g[l.n2()];
    if (l.n1()<l.n2()) {        
        auto it=rot1.find(l); //if the location is new we are at rot1.end()
        return aE(g, rot1, rot2, it, rot2.end());
    }
    if (l.n1()>l.n2()) {
        auto it=rot2.find(l); //if the location is new we are at rot1.end()
        return aE(g, rot1, rot2, rot1.end(), it);
    }
    auto it=rot1.find(l); //if the location is new we are at rot1.end()
    return aE(g, rot1, rot2, it, rot1.end());
}


inline IncidenceRefPair  Factory::aE(Graph &g, Direction l)  {
#ifdef BA_GRAPH_DEBUG
    assert(g.contains(l.n1()));
    assert(g.contains(l.n2()));
    assert(g.count(l.n1(),IP::n2(l.n2()) && IP::primary())<=l.index());
#endif
    auto &rot1=g[l.n1()];
    auto &rot2=g[l.n2()];
    auto it=rot1.find(l); //if the location is new we are at rot1.end()
    return aE(g, rot1, rot2, it, rot2.end());
}


inline IncidenceRefPair  Factory::aE(Graph &g, const Vertex &v1, const Vertex &v2) {
#ifdef BA_GRAPH_DEBUG
    assert(g.contains(v1));
    assert(g.contains(v2));
#endif
    return aE(g, *g.find(v1), *g.find(v2));
}

inline IncidenceRefPair Factory::aE(Graph &g, const Edge &e) {
#ifdef BA_GRAPH_DEBUG
    assert(g.contains(e.v1()));
    assert(g.contains(e.v2()));
    assert(!g.contains(e.v1(),e));
#endif
    Rotation &r1=*g.find(e.v1());
    Rotation &r2=*g.find(e.v2());
    return aE(g, r1, r2, e);
}


inline void Factory::aEs(Graph &g, const std::vector<Location> &vl) {
    for(const auto &loc:vl) {aE(g,loc);}
}
inline void Factory::aEs(Graph &g, const std::vector<Direction> &vl) {
    for(const auto &loc:vl) {aE(g,loc);}
}
inline void Factory::aEs(Graph &g, const std::vector<std::pair<Vertex, Vertex>> &vpv) {
    for(const auto &pv:vpv) {aE(g,pv.first,pv.second);}
}
inline void Factory::aEs(Graph &g, const std::vector<Edge> &ve) {
    for(const auto &e:ve) {aE(g,e);}
}


inline void Factory::dE(Graph &g, const Rotation &r, Edge e) {
#ifdef BA_GRAPH_DEBUG
    int found=0;
    for(auto &rot: g)
        if (&rot==&r) found++;
    assert(found==1);
    assert(r.contains(e));
#endif
    dE(g,r.find(e));
}

inline void Factory::dE(Graph &g, Edge e) {
#ifdef BA_GRAPH_DEBUG
    assert(g.contains(e.v1(),e));
#endif
    dE(g,g.find(e.v1(),e).second);
}

inline Edge Factory::dE(Graph &g, Location l) {
#ifdef BA_GRAPH_DEBUG
    assert(g.contains(l));
#endif
    auto it=g.find(l);
    Edge e=it->e();
    dE(g, it);
    return e;
}


inline Edge Factory::dE(Graph &g, Direction l) {
#ifdef BA_GRAPH_DEBUG
    assert(g.contains(l));
#endif
    auto it=g.find(l);
    Edge e=it->e();
    dE(g, it);
    return e;
}



inline void Factory::change_vertex_number(Graph &g, Vertex v, Number n) {
    auto it=g.find(v);
#ifdef BA_GRAPH_DEBUG
    assert(it!=g.end());
#endif
    change_vertex_number(g, it->n(), n);
}

inline void Factory::change_vertex_number(Graph &g, Number n_old, Number n) {
    if (n_old==n) return;
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    assert(g.contains(n_old));
    assert(!g.contains(n));
#endif
    if (n.to_int()>=g.free_number) g.free_number=n.to_int()+1;
    g.rr_.insert(std::pair<const int,Rotation *>(n.to_int(),g.rr_[n_old.to_int()]));
    g[n].n_=n;
    g.rr_.erase(n_old.to_int());
}

inline void Factory::swap_vertices(Graph &g, Vertex v1, Vertex v2) {
    if (v1==v2) return;
    auto it1=g.find(v1);
    auto it2=g.find(v2);
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    assert(it1!=g.end());
    assert(it2!=g.end());
#endif
    swap_vertices(g, it1->n(), it2->n());
}
inline void Factory::swap_vertices(Graph &g, Number n1, Number n2) {
    if (n1==n2) return;
#ifdef BA_GRAPH_DEBUG
    assert(g.contains(n1));
    assert(g.contains(n2));
#endif
    Rotation *r1=g.rr_[n1.to_int()];
    Rotation *r2=g.rr_[n2.to_int()];
    std::swap(r1->n_,r2->n_);
    g.rr_[n1.to_int()]=r2;
    g.rr_[n2.to_int()]=r1;
}

inline void Factory::renumber_vertices(Graph &g, const Mapping<Number> &m) {
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    for(auto it=std::cbegin(m);it!=std::cend(m);it++)
        assert(g.contains(it->first));
#endif
    for(auto it=std::cbegin(m);it!=std::cend(m);it++)
        if (it->second.to_int()>=g.free_number) g.free_number=it->second.to_int()+1;
    int oldfree=g.free_number;
    int n=oldfree;
    for(auto it=std::cbegin(m);it!=std::cend(m);it++)
        change_vertex_number(g, it->first, n++);
    n=oldfree;
    for(auto it=std::cbegin(m);it!=std::cend(m);it++) {
#ifdef BA_GRAPH_DEBUG
        assert(!g.contains(it->second));
#endif
        change_vertex_number(g, n++,it->second);
    }
    g.free_number=oldfree;
}

inline Mapping<Number> Factory::renumber_consecutive(Graph &g) {
    Mapping<Number> m;
    int n=0;
    for(const auto &rot:g) m.set(rot.n(),n++);
    renumber_vertices(g,m);
    return m;
}

inline Mapping<Number> Factory::renumber_consecutive_stable(Graph &g) {
    std::vector<Number> v;
    for(const auto &rot:g) v.push_back(rot.n());
    std::sort(v.begin(),v.end());
    Mapping<Number> m;
    int n=0;
    for(const auto &e:v) m.set(g[e].n(),n++);
    renumber_vertices(g,m);
    return m;
}


inline void Factory::swap_incidence(Graph &g, IncidenceIterator it1, IncidenceIterator it2) {
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    assert(it1->n1()==it2->n1());
    Rotation &r1=g[it1->n1()];    
    assert(&r1==&(it1->r1()));
    assert(r1.contains(it1->h1()));
    Rotation &r2=g[it2->n1()];    
    assert(&r2==&(it2->r1()));
    assert(r2.contains(it2->h1()));
#endif
    if (it1==it2) return;
    // const cast is ok here, we could get the rotation from G if we wanted, but it would take longer
    (void)g;
    Rotation &r=const_cast<Rotation &>(it1->r1());
    size_t i1o=it1-r.begin();
    size_t i2o=it2-r.begin();
    std::swap(r.rr_[i1o],r.rr_[i2o]);
}

inline void Factory::move_incidence(Graph &g, IncidenceIterator it1, IncidenceIterator it2) {
#ifdef BA_GRAPH_DEBUG
    assert(check(g));
    assert(it1->n1()==it2->n1());
    Rotation &r1=g[it1->n1()];    
    assert(&r1==&(it1->r1()));
    assert(r1.contains(it1->h1()));
    Rotation &r2=g[it2->n1()];    
    assert(&r2==&(it2->r1()));
    assert(r2.contains(it2->h1()));
#endif
    if (it1==it2) return;
    // const cast is ok here, we could get the rotation from G if we wanted, but it would take longer
    (void)g;
    Rotation &r=const_cast<Rotation &>(it1->r1());
    size_t i1o=it1-r.begin();
    size_t i2o=it2-r.begin();
    if (i1o>i2o) {
        for(size_t i=i1o; i>i2o; i--)
            std::swap(r.rr_[i],r.rr_[i-1]);
    }
    else {
        for(size_t i=i1o; i<i2o; i++)
            std::swap(r.rr_[i],r.rr_[i+1]);
    }
}



inline RotationIterator Factory::dV(Graph &g, RotationIterator i) {
#ifdef BA_GRAPH_DEBUG
    Rotation &rx=g[i->n()];    
    assert(&rx==&(*i));
#endif
    Rotation &r=*i;
    std::vector<Edge> to_delete;
    for (auto it=r.begin();it!=r.end();it++)
        if (!it->is_loop() || it->is_primary())
            to_delete.push_back(it->e());
    for (const auto &e :to_delete) dE(g,e);
    delete &r;
    return RotationIterator(g.rr_.erase(i.i_));
}

inline IncidenceIterator Factory::dE(Graph &g, IncidenceIterator i) {
#ifdef BA_GRAPH_DEBUG
    Rotation &r=g[i->n1()];    
    assert(&r==&(i->r1()));
    assert(r.contains(i->h1()));
#endif
    Rotation &r1=i->r1();
    //calculate the new iterator position
    size_t offset=i-r1.begin();
    if (i->is_loop())
        for(auto it2=r1.begin(); it2!=i; it2++)
            if (it2->e()==i->e()) {
                offset--;
                break;
            }
    Rotation &r2=i->r2();
    int second_id=-i->h1().to_int();
    //delete first incidence
    Incidence *p1=*(i.i_);
    r1.rr_.erase(i.i_);
    delete p1;

    //delete second incidence
    auto i2=r2.rr_.begin();
    for (;(*i2)->id_code_!=second_id;i2++);
    Incidence *p2=*i2;
    r2.rr_.erase(i2);
    delete p2;

    g.edge_count--;
    return IncidenceIterator(r1.begin()+offset);
}



}  // namespace end
#endif
