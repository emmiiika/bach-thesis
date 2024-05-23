#ifndef BA_GRAPH_ROTATION_CPP
#define BA_GRAPH_ROTATION_CPP
#include "graph.h"
#include "incidence_predicates.hpp"

#include <algorithm>
#include <set>


namespace ba_graph {


/*
IncidenceIterator
*/

inline IncidenceIterator::IncidenceIterator(std::vector<Incidence *>::const_iterator i) : i_(i) {}
inline IncidenceIterator::IncidenceIterator() {}

inline IncidenceIterator& IncidenceIterator::operator++() { ++i_; return *this; }
inline IncidenceIterator IncidenceIterator::operator++(int) { auto tmp = *this; ++i_; return tmp; }
inline IncidenceIterator& IncidenceIterator::operator--() { --i_; return *this; }
inline IncidenceIterator IncidenceIterator::operator--(int) { auto tmp = *this; --i_; return tmp; }
inline IncidenceIterator IncidenceIterator::operator+(int o) {return IncidenceIterator(i_+o);}
inline IncidenceIterator IncidenceIterator::operator-(int o) {return IncidenceIterator(i_-o);}
inline size_t IncidenceIterator::operator-(IncidenceIterator o) {return i_-o.i_;}
inline bool IncidenceIterator::operator==(IncidenceIterator j) const { return i_ == j.i_; }
inline bool IncidenceIterator::operator!=(IncidenceIterator j) const { return i_ != j.i_; }
inline bool IncidenceIterator::operator<(IncidenceIterator j) const { return i_ < j.i_; }
inline bool IncidenceIterator::operator>(IncidenceIterator j) const { return i_ > j.i_; }
inline bool IncidenceIterator::operator<=(IncidenceIterator j) const { return i_ <= j.i_; }
inline bool IncidenceIterator::operator>=(IncidenceIterator j) const { return i_ >= j.i_; }
inline Incidence & IncidenceIterator::operator*() const {return **i_; }
inline Incidence * IncidenceIterator::operator->() const {return *i_; }


/*
Rotation
*/

inline Rotation::Rotation(const Vertex &v, Number n): v_(v), n_(n) {}
inline Rotation::~Rotation() {
    for(Incidence *p:rr_)
        delete(p);
}

inline const Incidence & Rotation::operator[] (int i) const {return *rr_[i];}


inline Number Rotation::Rotation::n() const { return n_; }
inline const Vertex& Rotation::v() const { return v_; }

inline bool Rotation::isolated() const { return rr_.empty(); }
inline int Rotation::degree() const { return rr_.size(); }

inline IncidenceIterator Rotation::begin() const { return IncidenceIterator(rr_.begin()); }
inline IncidenceIterator Rotation::end() const  { return IncidenceIterator(rr_.end()); }

inline std::reverse_iterator<IncidenceIterator> Rotation::rbegin() const { return std::reverse_iterator<IncidenceIterator>(rr_.end());}
inline std::reverse_iterator<IncidenceIterator> Rotation::rend() const { return std::reverse_iterator<IncidenceIterator>(rr_.begin());}


template<typename T>
inline IncidenceIterator Rotation::find(T &&p) const
        requires IncidencePredicate<T> || RotationLocatable<T> {
   decltype(auto) ip=get_ip(std::forward<T>(p));
   IncidenceIterator it=begin();
   for(;it!=end();it++) if (ip(*it)) break;
   return it;
}



template<typename T>
inline int Rotation::count(T &&p) const
        requires IncidencePredicate<T> || RotationLocatable<T> {
    decltype(auto) ip=get_ip(std::forward<T>(p));
    int count=0;
    for(auto it=begin();it!=end();it++) if (ip(*it)) count++;
    return count;
}


template<typename T>
inline bool Rotation::contains(T &&p) const
        requires IncidencePredicate<T> || RotationLocatable<T> {
    decltype(auto) ip=get_ip(std::forward<T>(p));
    for(auto it=begin();it!=end();it++) if (ip(*it)) return true;
    return false;
}


template<typename T, IncidenceFunction F>
inline auto Rotation::list(T &&p, F &&f) const
        requires (IncidencePredicate<T> || RotationLocatable<T>) {
    decltype(auto) ip=get_ip(std::forward<T>(p));
    std::vector<decltype(f(begin()))> res;
    for(auto it=begin();it!=end();it++) if (ip(*it)) res.push_back(f(it));
    return res;
}

template<typename T>
inline std::vector<IncidenceIterator> Rotation::list(T &&p) const
        requires IncidencePredicate<T> || RotationLocatable<T> {
    return list(std::forward<T>(p),IT::iterator());
}

template<typename T, IncidenceComparator V, IncidenceFunction F>
inline auto Rotation::list(T &&p, const V& c, F &&f) const
        requires (IncidencePredicate<T> || RotationLocatable<T>) {
    return sort_and_transform(list(std::forward<T>(p)),c,std::forward<F>(f));
}

template<typename T, IncidenceComparator V>
inline std::vector<IncidenceIterator> Rotation::list(T &&p, const V& c) const
        requires IncidencePredicate<T> || RotationLocatable<T> {
    return copy_and_sort(list(std::forward<T>(p)),c);
}



inline std::set<Number> Rotation::neighbours() const {
     std::set<Number> result;
     for (auto inci : rr_) {
        result.insert(inci->r2().n());
     }
     return result;
}



}  // namespace end
#endif
