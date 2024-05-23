#ifndef BA_GRAPH_GRAPH_CPP
#define BA_GRAPH_GRAPH_CPP
#include "graph.h"
#include "helpers.hpp"
#include "rotation_predicates.hpp"
#include "incidence_predicates.hpp"

#include <algorithm>

namespace ba_graph {

/*
RotationIterator
*/

inline RotationIterator::RotationIterator(std::map<int,Rotation *>::const_iterator i) : i_(i) {}
inline RotationIterator::RotationIterator() {}
inline RotationIterator& RotationIterator::operator++() { ++i_; return *this; }
inline RotationIterator RotationIterator::operator++(int) { auto tmp = *this; ++i_; return tmp; }
inline bool RotationIterator::operator==(RotationIterator j) const { return i_ == j.i_; }
inline bool RotationIterator::operator!=(RotationIterator j) const { return i_ != j.i_; }
inline Rotation & RotationIterator::operator*() const { return *(i_->second); }
inline Rotation * RotationIterator::operator->() const { return i_->second; }


/*
Graph
*/

#ifdef BA_GRAPH_DEBUG
inline Graph::Graph(Factory * f): my_factory(f), edge_count(0), free_number(0) {}
#else
inline Graph::Graph(): edge_count(0), free_number(0) {}
#endif
inline Graph::~Graph() {for(auto it=begin();it!=end();it++) delete(&(*it));}
inline Graph::Graph (Graph &&g) noexcept: edge_count(g.edge_count), free_number(g.free_number),
        rr_(std::move(g.rr_)) {
#ifdef BA_GRAPH_DEBUG
    my_factory=g.my_factory;
#endif
}
inline Graph& Graph::operator= (Graph &&g) {
#ifdef BA_GRAPH_DEBUG
    my_factory=g.my_factory;
#endif
    edge_count=g.edge_count;
    free_number=g.free_number;
    std::swap(rr_, g.rr_);
    return *this;
}


inline int Graph::order() const { return rr_.size(); }
inline int Graph::size() const { return edge_count; }

inline Rotation & Graph::operator[](Number i) const {
#ifdef BA_GRAPH_DEBUG
    assert(rr_.find(i.to_int())!=rr_.end());
#endif
    return *rr_.at(i.to_int());
}

inline Incidence & Graph::operator[] (Location l) const {
    if (l.n1()<=l.n2()) {
        #ifdef BA_GRAPH_DEBUG
            assert(rr_.find(l.n1().to_int())!=rr_.end());
            assert((*this)[l.n1()].count(l.n2())>l.index());
        #endif
        return *(rr_.at(l.n1().to_int())->find(l));
    }
    #ifdef BA_GRAPH_DEBUG
        assert(rr_.find(l.n2().to_int())!=rr_.end());
        assert((*this)[l.n2()].count(l.n1())>l.index());
    #endif
    return *(rr_.at(l.n1().to_int())->find(l));
}
inline Incidence & Graph::operator[] (Direction d) const {
        #ifdef BA_GRAPH_DEBUG
            assert(rr_.find(d.n1().to_int())!=rr_.end());
            assert((*this)[d.n1()].count(IP::n2(d.n2()) && IP::primary())>d.index());
        #endif    
        return *((*this)[d.n1()].find(d));
}


inline RotationIterator Graph::begin() const { return RotationIterator(rr_.begin()); }
inline RotationIterator Graph::end() const { return RotationIterator(rr_.end()); }
inline std::reverse_iterator<RotationIterator> Graph::rbegin() const { return std::reverse_iterator<RotationIterator>(rr_.end()); }
inline std::reverse_iterator<RotationIterator> Graph::rend() const { return std::reverse_iterator<RotationIterator>(rr_.begin()); }


//Rotation find functions
template<typename T>
inline RotationIterator Graph::find(T &&p) const
            requires RotationPredicate<T> || GraphLocatable<T> {
    decltype(auto) rp2=get_rp(std::forward<T>(p));
    auto it=begin();
    for(;it!=end();it++) if (rp2(*it)) return it;
    return it;
}
template<typename T>
inline int Graph::count(T &&p) const
            requires RotationPredicate<T> || GraphLocatable<T> {
    decltype(auto) rp2=get_rp(std::forward<T>(p));
    int count=0;
    for(auto it=begin();it!=end();it++) if (rp2(*it)) count++;
    return count;
}
template<typename T>
inline bool Graph::contains(T &&p) const
            requires RotationPredicate<T> || GraphLocatable<T> {
    decltype(auto) rp2=get_rp(std::forward<T>(p));
    for(auto it=begin();it!=end();it++) if (rp2(*it)) return true;
    return false;
}
template<typename T>
inline std::vector<RotationIterator> Graph::list(T &&p) const
            requires RotationPredicate<T> || GraphLocatable<T> {
    return list(std::forward<T>(p), RT::iterator());
}
template<typename T, RotationFunction U>
inline auto Graph::list(T &&p, U &&f) const -> std::vector<decltype(f(begin()))>
            requires (RotationPredicate<T> || GraphLocatable<T>) {
    decltype(auto) rp2=get_rp(std::forward<T>(p));
    std::vector<decltype(f(begin()))> res;
    for(auto it=begin();it!=end();it++)
        if (rp2(*it)) res.push_back(f(it));
    return res;
}
template <typename T, RotationComparator V>
inline std::vector<RotationIterator> Graph::list(T &&p, const V &c) const
        requires RotationPredicate<T> || GraphLocatable<T> {
    return copy_and_sort(list(std::forward<T>(p)),c);
}
template <typename T, RotationComparator V, RotationFunction U>
inline auto Graph::list(T &&p, const V &c, U &&f) const -> std::vector<decltype(f(begin()))>
        requires RotationPredicate<T> || GraphLocatable<T> {
    return sort_and_transform(list(std::forward<T>(p)),c,std::forward<U>(f));
}



//Incidence find functions
template<typename S, typename T>
inline std::pair<RotationIterator, IncidenceIterator> Graph::find(S &&rp, T &&ip) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>) {
    decltype(auto) rp2=get_rp(std::forward<S>(rp));
    decltype(auto) ip2=get_ip(std::forward<T>(ip));
    RotationIterator ri=begin();
    IncidenceIterator ii;
    for(;ri!=end();ri++) {
        if (!rp2(*ri)) continue;
        ii=ri->begin();
        for(;ii!=ri->end();ii++)
            if (ip2(*ii)) return std::pair(ri, ii);
    }
    return std::pair(ri, IncidenceIterator());
}

template<typename S, typename T>
inline int Graph::count(S &&rp, T &&ip) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>) {
    decltype(auto) rp2=get_rp(std::forward<S>(rp));
    decltype(auto) ip2=get_ip(std::forward<T>(ip));
    int count=0;
    for(const auto &rot: *this) if (rp2(rot))
        for(auto const &inc: rot) if (ip2(inc)) count++;
    return count;
}

template<typename S, typename T>
inline bool Graph::contains(S &&rp, T &&ip) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>) {
    decltype(auto) rp2=get_rp(std::forward<S>(rp));
    decltype(auto) ip2=get_ip(std::forward<T>(ip));
    for(auto const &rot: *this) if (rp2(rot))
        for(auto const &inc: rot) if (ip2(inc)) return true;
    return false;
}

template<typename S, typename T>
inline std::vector<IncidenceIterator> Graph::list(S &&rp, T &&ip) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>) {
    return list(std::forward<S>(rp),std::forward<T>(ip),IT::iterator());
}

template<typename S, typename T, IncidenceFunction U>
inline auto Graph::list(S &&rp, T &&ip, U &&f) const
        requires (RotationPredicate<S> || GraphLocatable<S>)
        && (IncidencePredicate<T> || RotationLocatable<T>) {
    decltype(auto) rp2=get_rp(std::forward<S>(rp));
    decltype(auto) ip2=get_ip(std::forward<T>(ip));
    std::vector<decltype(f(begin()->begin()))> res;
    for(auto ri=begin();ri!=end();ri++) if (rp2(*ri))
        for(auto ii=ri->begin();ii!=ri->end();ii++) if (ip2(*ii)) res.push_back(f(ii));
    return res;
}

template<typename S, typename T, IncidenceComparator V>
inline std::vector<IncidenceIterator> Graph::list(S &&rp, T &&ip, const V &c) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>) {
    return copy_and_sort(list(std::forward<S>(rp),std::forward<T>(ip)),c);
}

template<typename S, typename T, IncidenceComparator V, IncidenceFunction U>
inline auto Graph::list(S &&rp, T &&ip, const V &c, U &&f) const
        requires (RotationPredicate<S> || GraphLocatable<S>)
        && (IncidencePredicate<T> || RotationLocatable<T>) {
    return sort_and_transform(list(std::forward<S>(rp),std::forward<T>(ip)),c, std::forward<U>(f));
}

inline RotationIterator Graph::find(Number &&p) const {
    return RotationIterator(rr_.find(p.to_int()));
}
inline bool Graph::contains(Number &&p) const {
    return rr_.find(p.to_int()) != rr_.end();
}

//Location find functions
inline IncidenceIterator Graph::find(const Location &loc) const {return find(loc.n1(),loc).second;}
inline bool Graph::contains(const Location &loc) const {return contains(loc.n1(),loc);}

//Direction find functions
inline IncidenceIterator Graph::find(const Direction &loc) const {return find(loc.n1(),loc).second;}
inline bool Graph::contains(const Direction &loc) const {return contains(loc.n1(),loc);}



}  // namespace end
#endif
