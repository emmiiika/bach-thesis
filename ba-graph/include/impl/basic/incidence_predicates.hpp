#ifndef BA_GRAPH_INCIDENCE_PREDICATES_H
#define BA_GRAPH_INCIDENCE_PREDICATES_H
#include "basic_objects.hpp"
#include "incidence.h"
#include "rotation.h"

namespace ba_graph {



//predicates
namespace IP {
    //to use std::find_if with this iterator we define UnaryPredicate
    template<typename T> class IP { public: IP(const T &h); };


    class all {
    public:
        inline bool operator() (const Incidence &a) {(void)a; return true;};
    };

    class primary {
    public:
        primary() {}
        inline bool operator() (const Incidence &a) {return a.is_primary();};
    };

    class loop {
    public:
        loop() {}
        inline bool operator() (const Incidence &a) {return a.is_loop();};
    };

    class ith {
        int i_;
    public:
        ith(int i): i_(i) {}
        inline bool operator() (const Incidence &) {if (i_--) return false; return true;};
    };



    template<> class IP<HalfEdge> {
        HalfEdge h_;
    public:
        IP(const HalfEdge &h): h_(h) {}
        bool operator() (const Incidence &a) { return a.h1()==h_; }
    };


    template<> class IP<Edge> {
        Edge e_;
    public:
        IP(const Edge &e): e_(e) {}
        bool operator() (const Incidence &a) { return a.e()==e_; }
    };

    template<> class IP<Vertex> {
        Vertex v_;
    public:
        IP(const Vertex &v): v_(v) {}
        bool operator() (const Incidence &a) { return a.r2().v()==v_; }
    };
    template<> class IP<Number> {
        Number n_;
    public:
        IP(const Number &n): n_(n) {}
        bool operator() (const Incidence &a) { return a.n2()==n_; }
    };

    template<typename T>
    class in {
    std::set<T> s_;
    public:
        in(const std::set<T> &s) : s_(s){};
        in(const std::set<T> &&s) : s_(s){};
        bool operator() (const Incidence &a) {
            for (const auto &e:s_)
                if (IP(e)(a)) return true;
            return false;
        }
    };

    template<typename T>
    class primary_in {
    std::set<T> s_;
    public:
        primary_in(const std::set<T> &s) : s_(s){};
        primary_in(const std::set<T> &&s) : s_(s){};
        bool operator() (const Incidence &a) {
            for (const auto &e:s_)
                if (primary()(a) && IP(e)(a)) return true;
            return false;
        }
    };


    class n2 {
        Number n_;
    public:
        n2(Number n) :n_(n) {}
        inline bool operator() (const Incidence &i) {return i.n2()==n_;};
    };
    class v2 {
        Vertex v_;
    public:
        v2(Vertex v) :v_(v) {}
        inline bool operator() (const Incidence &i) {return i.v2()==v_;};
    };
    class e {
        Edge e_;
    public:
        e(Edge e) :e_(e) {}
        bool operator() (const Incidence &i) {return i.e()==e_;};
    };
    class h1 {
        HalfEdge h_;
    public:
        h1(HalfEdge h) :h_(h) {}
        inline bool operator() (const Incidence &i) {return i.h1()==h_;};
    };
    class h2 {
        HalfEdge h_;
    public:
        h2(HalfEdge h) :h_(h) {}
        inline bool operator() (const Incidence &i) {return i.h2()==h_;};
    };

    class degree2_is {
        int cnt_;
    public:
        degree2_is(int degree) :cnt_(degree) {}
        inline bool operator() (const Incidence &i) {return i.r2().degree()==cnt_;};
    };


    template<> class IP<Direction> {
        Number n1_;
        Number n2_;
        int index_;
    public:
        IP(const Direction &l): n1_(l.n1()), n2_(l.n2()), index_(l.index()) {}
        bool operator() (const Incidence &a) {
#ifdef BA_GRAPH_DEBUG
            assert(n1_==a.n1());
#endif
            if (index_<0) return false;
            if (a.n2()!=n2_ || !a.is_primary()) return false;
            return (index_--==0);
        }
    };

    template<> class IP<Location> {
        Location l_;
        int status;
        int halfedge_id;
    public:
        IP(const Location &l): l_(l), status(0) {}
        bool operator() (const Incidence &a) {
            if (!status) {
                int index=l_.index();
                if (l_.n2()==a.n1()) l_=l_.reverse();
#ifdef BA_GRAPH_DEBUG
                assert(l_.n1()==a.n1());
#endif                       
                if (l_.n1()<l_.n2()) {
                    for(auto &loc:a.r1()) 
                        if (loc.n2()==l_.n2()) {
                            if (index==0) {status=1; halfedge_id=loc.h1().to_int(); break;}
                            index--;
                        }                    
                }
                if (l_.n1()>l_.n2()) {
                    auto it=a.r1().find(l_.n2());
                    if (it==a.r1().end()) {status=-1; return false;};
                    for(auto &loc: it->r2()) 
                        if (loc.n2()==l_.n1()) {
                            if (index==0) {status=1; halfedge_id=loc.h2().to_int(); break;}
                            index--;
                        }                    
                }
                if (l_.n1()==l_.n2()) {
                    for(auto &loc:a.r1()) 
                        if (loc.n2()==l_.n2() && loc.is_primary()) {
                            if (index==0) {status=1; halfedge_id=loc.h1().to_int(); break;}
                            if (index==1) {status=1; halfedge_id=loc.h2().to_int(); break;}
                            index-=2;
                        }                                       
                }
                if (!status) status=-1;
            }
            return status==1 && a.h1().to_int()==halfedge_id;
        }
    };


    //spies
    class count {
        int cnt_;
    public:
        count() :cnt_(0) {}
        inline bool operator() (const Incidence &) {cnt_++; return true;};
        inline int get_count() const {return cnt_;}
    };


}  //namespace IP end


namespace IT {
    class iterator {
    public:
        iterator() {}
        inline IncidenceIterator operator() (IncidenceIterator it) { return it; }
    };

    class n2 {
    public:
        n2() {}
        inline Number operator() (IncidenceIterator it) { return it->n2(); }
    };

    class v2 {
    public:
        v2() {}
        inline Vertex operator() (IncidenceIterator it) { return it->v2(); }
    };

    class e {
    public:
        e() {}
        inline Edge operator() (IncidenceIterator it) { return it->e(); }
    };

    class h1 {
    public:
        h1() {}
        inline HalfEdge operator() (IncidenceIterator it) { return it->h1(); }
    };

    class h2 {
    public:
        h2() {}
        inline HalfEdge operator() (IncidenceIterator it) { return it->h2(); }
    };

    class l {
    public:
        l() {}
        Location operator() (IncidenceIterator it) { return it->l(); }
    };

    class r1 {
    public:
        r1() {}
        inline Rotation& operator() (IncidenceIterator it) { return it->r1(); }
    };
    class r2 {
    public:
        r2() {}
        inline Rotation& operator() (IncidenceIterator it) { return it->r2(); }
    };

} //namespace IT end

namespace IC {
    class n1 {
    public:
        n1() {}
        inline bool operator() (IncidenceIterator it1, IncidenceIterator it2) { return it1->n1()<it2->n1(); }
    };
    class n2 {
    public:
        n2() {}
        inline bool operator() (IncidenceIterator it1, IncidenceIterator it2) { return it1->n2()<it2->n2(); }
    };
    class degree1 {
    public:
        degree1() {}
        inline bool operator() (IncidenceIterator it1, IncidenceIterator it2) { return it1->r1().degree()<it2->r1().degree(); }
    };
    class degree2 {
    public:
        degree2() {}
        inline bool operator() (IncidenceIterator it1, IncidenceIterator it2) { return it1->r2().degree()<it2->r2().degree(); }
    };

    class e {
    public:
        e() {}
        inline bool operator() (IncidenceIterator it1, IncidenceIterator it2) { return it1->e()<it2->e(); }
    };


    template<IncidenceComparator C1, IncidenceComparator C2>
    class pair {
        C1 c1_;
        C2 c2_;
    public:
        pair(const C1 &c1, const C2 &c2) : c1_(c1), c2_(c2) {}
        inline bool operator() (IncidenceIterator it1, IncidenceIterator it2) {
            if (c1_(it1,it2)) return true;
            if (c1_(it2,it1)) return false;
            if (c2_(it1,it2)) return true;
            return false;
        }
    };
}


template<typename T>
inline decltype(auto) get_ip(T &&l)
        requires IncidencePredicate<T> || RotationLocatable<T> {
    if constexpr (IncidencePredicate<T>) return std::forward<T>(l);
    else return IP::IP(l);
}







}  // namespace end
#endif
