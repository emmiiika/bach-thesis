#ifndef BA_GRAPH_BASIC_OBJECTS_HPP
#define BA_GRAPH_BASIC_OBJECTS_HPP
#include "classes.h"
#include <utility>
#include <functional>
#include <vector>
#include <set>

#ifdef BA_GRAPH_DEBUG
#include <stdio.h>
#include <cassert>
#endif

namespace ba_graph {

/*
Id
*/

class Id {
    friend Edge;
    friend Factory;
private:
    int id_;
    Id(int i) : id_(i) {}
public:
    Id() : id_(0) {}
    ~Id() {}
    inline int to_int() const { return id_; }
    inline bool is_null() const { return id_==0; }

    friend inline bool operator==(const Id &i, const Id &j) { return i.id_==j.id_; }
    friend inline bool operator!=(const Id &i, const Id &j) { return !(i==j); }
    friend inline bool operator<(const Id &i, const Id &j) { return i.id_<j.id_; }
};


/*
Vertex
*/

class Vertex {
    friend Rotation;
    friend Graph;
    friend Factory;
private:
    Id id_;
    Vertex(const Id &id) : id_(id) {}
public:
    Vertex() {}
    ~Vertex() {}

    inline const Id& id() const { return id_; }
    inline int to_int() const { return id_.to_int(); }
    inline bool is_null() const { return to_int()==0; }

    friend inline bool operator==(const Vertex &v1, const Vertex &v2) { return v1.id_==v2.id_; }
    friend inline bool operator!=(const Vertex &v1, const Vertex &v2) { return !(v1==v2); }
    friend inline bool operator<(const Vertex &v1, const Vertex &v2) { return v1.id_<v2.id_; }
};



/*
Edge
*/

class Edge {
    friend HalfEdge;
    friend Factory;
    friend Graph;
    friend Rotation;
    friend Incidence;
private:
    Id id_;
    Vertex v1_,v2_;
    Edge(const Vertex& v1, const Vertex& v2, const Id& i): id_(i), v1_(v1), v2_(v2) {}
    Edge(const Vertex &v1, const Vertex &v2, int id_code) {
        if (id_code<0) {
            v1_=v2; v2_=v1; id_=Id(-id_code);
        } else {
            v1_=v1; v2_=v2; id_=Id(id_code);
        }
    }

public:
    Edge() {}
    ~Edge() {}

    inline Id id() const { return id_; }
    inline int to_int() const { return id_.to_int(); }
    inline bool is_null() const { return id_.to_int()==0; }
    inline bool is_loop() const { return v1_==v2_; }

    inline Vertex v1() const { return v1_; }
    inline Vertex v2() const { return v2_; }
    inline std::pair<Vertex, Vertex> v12() const { return std::pair<Vertex, Vertex>(v1_,v2_); }
    inline Vertex other_v(const Vertex& v) const {
        #ifdef BA_GRAPH_DEBUG
        assert(v==v1_ || v==v2_);
        #endif
        if (v==v1_) return v2_;
        return v1_;
    }
    inline bool has_v(const Vertex& v) const { return v1_==v || v2_==v; }

    inline HalfEdge h1() const;
    inline HalfEdge h2() const;
    inline std::pair<HalfEdge, HalfEdge> h12() const;

    friend inline bool operator==(const Edge &e1, const Edge &e2) { return e1.id_==e2.id_; }
    friend inline bool operator!=(const Edge &e1, const Edge &e2) { return !(e1==e2); }
    friend inline bool operator<(const Edge &e1, const Edge &e2) { return e1.id_<e2.id_; }
};


/*
HalfEdge
*/

class HalfEdge {
    friend Edge;
    friend Rotation;
    friend Incidence;
    friend Graph;
    friend Factory;
private:
    Vertex v1_;
    Vertex v2_;
    int id_code_;
    HalfEdge(const Vertex &v1, const Vertex &v2, int id_code) :
            v1_(v1),v2_(v2), id_code_(id_code) {}
public:
    HalfEdge() { id_code_=0; }
    ~HalfEdge() {}

    inline int to_int() const { return id_code_; }
    inline bool is_null() const { return id_code_==0; }
    inline bool is_loop() const { return v1_==v2_; }
    inline bool is_primary() const { return id_code_>0; }

    inline Vertex v1() const { return v1_; }
    inline Vertex v2() const { return v2_; }
    inline std::pair<Vertex, Vertex> v12() const { return std::pair<Vertex, Vertex>(v1_,v2_); }
    inline Edge e() const {
        if (id_code_<0) return Edge(v2_,v1_,-id_code_);
        else return Edge(v1_,v2_,id_code_);
    }
    inline HalfEdge h2() const { return HalfEdge(v2_,v1_,-id_code_); }

    friend inline bool operator==(const HalfEdge &e1, const HalfEdge &e2) { return e1.id_code_==e2.id_code_; }
    friend inline bool operator!=(const HalfEdge &e1, const HalfEdge &e2) { return !(e1==e2); }
    friend inline bool operator<(const HalfEdge &e1, const HalfEdge &e2) { return e1.id_code_<e2.id_code_; }
};


/*
Number
*/

class Number {
    int n_;
public:
    Number(): n_(-1) {}
    Number(int i): n_(i) {}

    inline bool is_null() const { return n_<0; }
    inline Number operator+(int k) const { return Number(n_ + k); }
    inline Number operator-(int k) const { 
        #ifdef BA_GRAPH_DEBUG
        assert(n_>=k);
        #endif
        return Number(n_ - k); 
    }
    int to_int() const { return n_;};

    friend inline bool operator==(const Number &e1, const Number &e2) { return e1.n_==e2.n_; }
    friend inline bool operator!=(const Number &e1, const Number &e2) { return !(e1==e2); }
    friend inline bool operator<(const Number &e1, const Number &e2)  { return e1.n_<e2.n_; }
    friend inline bool operator<=(const Number &e1, const Number &e2) { return e1.n_<=e2.n_; }
    friend inline bool operator>(const Number &e1, const Number &e2)  { return e1.n_>e2.n_; }
    friend inline bool operator>=(const Number &e1, const Number &e2) { return e1.n_>=e2.n_; }
};

typedef Number Num;


/*
Location
*/

class Location {
    Number n1_;
    Number n2_;
    int index_;
public:
    Location(): n1_(-1), n2_(-1), index_(-1) {}
    Location(int i1, int i2, int i3=0): n1_(i1), n2_(i2), index_(i3) {}
    Location(Number i1, Number i2, int i3=0): n1_(i1), n2_(i2), index_(i3) {}
    Location(std::tuple<Number, Number, int> v) :
            n1_(std::get<0>(v)), n2_(std::get<1>(v)), index_(std::get<2>(v)) {}

    Number n1() const {return n1_;}
    Number n2() const {return n2_;}
    int index() const {return index_;}
    inline bool is_null() const { return n1_.is_null(); }

    Location reverse() const {
        if (n1_!=n2_) return Location(n2_,n1_,index_); 
        return Location(n1_,n2_, index_ ^ 1);
    }

    friend inline bool operator==(const Location &e1, const Location &e2) {
        return std::tuple(e1.n1_, e1.n2_, e1.index_) == std::tuple(e2.n1_, e2.n2_, e2.index_);
    }
    friend inline bool operator!=(const Location &e1, const Location &e2) { return !(e1==e2); }
    friend inline bool operator<(const Location &e1, const Location &e2) {
        return std::tuple(e1.n1_, e1.n2_, e1.index_) < std::tuple(e2.n1_, e2.n2_, e2.index_);
    }

};
typedef Location Loc;

inline std::vector<Loc> Locs(const std::vector<Loc> locs) {return locs;}
inline std::vector<Loc> Locations(const std::vector<Loc> locs) {return locs;}

/*
LocationPair
*/

class LocationPair : public Location {
public:
    LocationPair() {}
    LocationPair(int i1, int i2, int i3=0): Location(i1, i2, i3) {}
    LocationPair(Number i1, Number i2, int i3=0): Location(i1, i2, i3) {}
    LocationPair(std::tuple<Number, Number, int> v) : Location(v) {}
    LocationPair(const Location &l): Location(l) {}

    friend inline bool operator==(const LocationPair &e1, const LocationPair &e2) {
        int index1=e1.index();
        int index2=e2.index();
        if (e1.n1()==e1.n2()) index1/=2;
        if (e2.n1()==e2.n2()) index2/=2;
        std::set<int> s1 = { e1.n1().to_int(), e1.n2().to_int()};
        std::set<int> s2 = { e2.n1().to_int(), e2.n2().to_int()};
        return std::tuple(s1, index1) == std::tuple(s2, index2);
    }
    friend inline bool operator!=(const LocationPair &e1, const LocationPair &e2) { return !(e1==e2); }
    friend inline bool operator<(const LocationPair &e1, const LocationPair &e2) {
        int index1=e1.index();
        int index2=e2.index();
        if (e1.n1()==e1.n2()) index1/=2;
        if (e2.n1()==e2.n2()) index2/=2;
        std::set<int> s1 = { e1.n1().to_int(), e1.n2().to_int()};
        std::set<int> s2 = { e2.n1().to_int(), e2.n2().to_int()};
        return std::tuple(s1, index1) < std::tuple(s2, index2);
    }
};

/*
Direction
*/

class Direction {
    Number n1_;
    Number n2_;
    int index_;
public:
    Direction(): n1_(-1), n2_(-1), index_(-1) {}
    Direction(int i1, int i2, int i3=0): n1_(i1), n2_(i2), index_(i3) {}
    Direction(Number i1, Number i2, int i3=0): n1_(i1), n2_(i2), index_(i3) {}
    Direction(std::tuple<Number, Number, int> v) :
            n1_(std::get<0>(v)), n2_(std::get<1>(v)), index_(std::get<2>(v)) {}

    Number n1() const {return n1_;}
    Number n2() const {return n2_;}
    int index() const {return index_;}

    inline bool is_null() const { return n1_.is_null(); }
    friend inline bool operator==(const Direction &e1, const Direction &e2) {
        return std::tuple(e1.n1_, e1.n2_, e1.index_) == std::tuple(e2.n1_, e2.n2_, e2.index_);
    }
    friend inline bool operator!=(const Direction &e1, const Direction &e2) { return !(e1==e2); }
    friend inline bool operator<(const Direction &e1, const Direction &e2) {
        return std::tuple(e1.n1_, e1.n2_, e1.index_) < std::tuple(e2.n1_, e2.n2_, e2.index_);
    }    
};

typedef Direction Dir;

inline std::vector<Dir> Dirs(const std::vector<Dir> locs) {return locs;}
inline std::vector<Dir> Directions(const std::vector<Dir> locs) {return locs;}


/*
Edge Implementation after HalfEdge is complete
*/

    inline HalfEdge Edge::h1() const { return HalfEdge(v1_,v2_,to_int()); }
    inline HalfEdge Edge::h2() const { return HalfEdge(v2_,v1_,-to_int()); }
    inline std::pair<HalfEdge, HalfEdge> Edge::h12() const {
        return std::pair<HalfEdge,HalfEdge>(HalfEdge(v1_,v2_,to_int()), HalfEdge(v2_,v1_,-to_int()));
    }

} //end namespace ba_graph



namespace std {
    template <>
    struct hash<ba_graph::Id> {
         size_t operator()(const ba_graph::Id& k) const {return std::hash<int>()(k.to_int());}
    };

    template <>
    struct hash<ba_graph::Vertex> {
        std::size_t operator()(const ba_graph::Vertex& k) const {return std::hash<int>()(k.to_int());}
    };

    template <>
    struct hash<ba_graph::Edge> {
        std::size_t operator()(const ba_graph::Edge& k) const {return std::hash<int>()(k.to_int());}
    };

    template <>
    struct hash<ba_graph::HalfEdge> {
        std::size_t operator()(const ba_graph::HalfEdge& k) const {return std::hash<int>()(k.to_int());}
    };

    template <>
    struct hash<ba_graph::Number> {
        std::size_t operator()(const ba_graph::Number& k) const {return std::hash<int>()(k.to_int());}
    };

    template <>
    struct hash<ba_graph::Location> {
        std::size_t operator()(const ba_graph::Location& k) const {
            return std::hash<int>()(k.n1().to_int()+k.n2().to_int()%41+k.index());
        }
    };

    template <>
    struct hash<ba_graph::LocationPair> {
        std::size_t operator()(const ba_graph::LocationPair& k) const {
            int index1=k.index();
            if (k.n1()==k.n2()) index1/=2;            
            return std::hash<int>()((k.n1().to_int()+k.n2().to_int())%41+(k.n1().to_int()&k.n2().to_int())+index1);
        }
    };

    template <>
    struct hash<ba_graph::Direction> {
        std::size_t operator()(const ba_graph::Direction& k) const {
            return std::hash<int>()(k.n1().to_int()+k.n2().to_int()%41+k.index());
        }
    };

}


#endif
