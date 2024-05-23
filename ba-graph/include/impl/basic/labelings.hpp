#ifndef BA_GRAPH_BASIC_LABELINGS_HPP
#define BA_GRAPH_BASIC_LABELINGS_HPP
#include "basic_objects.hpp"
#include "graph.h"
#include <map>
#include <iterator>

namespace ba_graph {

template <class LabelType, class BaGraphObject> 
class BaGraphLabeling {
private:
    LabelType def;
    std::map<BaGraphObject,LabelType> l_;
public:
    using key_type=BaGraphObject;
    using mapped_type=LabelType;
    using value_type=std::pair<const key_type, mapped_type>;

    using const_iterator=typename std::map<BaGraphObject,LabelType>::const_iterator;
    using const_reverse_iterator=typename std::map<BaGraphObject,LabelType>::const_reverse_iterator;

    BaGraphLabeling() {}
    BaGraphLabeling(const LabelType &d) : def(d) {}
    ~BaGraphLabeling() {}

    LabelType operator[] (const BaGraphObject &v) const {
        return get(v);
    }

    LabelType get(const BaGraphObject &v) const {
#ifdef BA_GRAPH_DEBUG
        assert(!v.is_null());
#endif
        auto it=l_.find(v);
        if (it==l_.end()) return def;
        return l_.at(v);
    }

    LabelType set(const BaGraphObject &v, const LabelType &d) {
#ifdef BA_GRAPH_DEBUG
        assert(!v.is_null());
#endif
        if (d==def) {l_.erase(v); return d;}
        l_[v]=d;
        return d;
    }


    const_iterator begin() const {return std::cbegin(l_);}
    const_iterator end() const {return std::cend(l_);}

    const_reverse_iterator rbegin() const {return std::crbegin(l_);}
    const_reverse_iterator rend() const {return std::crend(l_);}
};


template <class LabelType>
using VertexLabeling=BaGraphLabeling<LabelType, Vertex>;

template <class LabelType>
using EdgeLabeling=BaGraphLabeling<LabelType, Edge>;

template <class LabelType>
using HalfEdgeLabeling=BaGraphLabeling<LabelType, HalfEdge>;

template <class LabelType>
using NumberLabeling=BaGraphLabeling<LabelType, Number>;

template <class LabelType>
using LocationLabeling=BaGraphLabeling<LabelType, Location>;

template <class LabelType>
using LocationPairLabeling=BaGraphLabeling<LabelType, LocationPair>;

template <class LabelType>
using DirectionLabeling=BaGraphLabeling<LabelType, Direction>;





//fast number labeling if the numbers used in graphs are almost consecutive
template<typename LabelType>
class GraphNumberLabeling {
private:
    LabelType def_;
    std::vector<std::pair<Number,LabelType>> l;
public:
    class NumberLabelingIterator;
    friend NumberLabelingIterator;

    class NumberLabelingIterator:public std::iterator<std::bidirectional_iterator_tag, std::pair<Number,LabelType>> {
    typedef typename std::vector<std::pair<Number,LabelType>>::iterator underlying_iterator;
    protected:
        underlying_iterator i_;
    private:
        friend GraphNumberLabeling;
        underlying_iterator end_;
        LabelType def_;
        NumberLabelingIterator(underlying_iterator i, underlying_iterator end, LabelType def): i_(i), end_(end), def_(def) {}
    public:
        NumberLabelingIterator() {}
        inline NumberLabelingIterator& operator++() { ++i_; while(i_!=end_ && (i_->first<0 || i_->second==def_)) ++i_; return *this; }
        inline NumberLabelingIterator operator++(int) { auto tmp = *this; ++i_; while((i_->first<0 || i_->second==def_)&& i_!=end_) ++i_; return tmp; }
        inline NumberLabelingIterator& operator--() { --i_; while(i_->first<0 || i_->second==def_) --i_; return *this; }
        inline NumberLabelingIterator operator--(int) { auto tmp = *this; --i_; while(i_->first<0 || i_->second==def_) --i_; return tmp; }
        inline bool operator==(NumberLabelingIterator j) const { return i_ == j.i_; }
        inline bool operator!=(NumberLabelingIterator j) const { return i_ != j.i_; }
        inline std::pair<Number,LabelType> & operator*() const { return *i_; }
        inline std::pair<Number,LabelType> * operator->() const { return &(*i_); }
    };


    GraphNumberLabeling(const LabelType &d, const Graph &g) : def_(d) {
        int maxN=0;
        for(const auto &r:g) if (r.n().to_int()>maxN) maxN=r.n().to_int();
        l.resize(maxN+1);
        for(auto &p:l) {p.first=-1;p.second=d;}
        for(const auto &r:g) l[r.n().to_int()].first=r.n().to_int();
    }
    ~GraphNumberLabeling() {}



    LabelType operator[] (const Number n) const {
#ifdef BA_GRAPH_DEBUG
        assert(n.to_int()>=0);
#endif
        return l[n.to_int()].second;
    }

    LabelType set(const Number n, LabelType v) {
#ifdef BA_GRAPH_DEBUG
        assert(n.to_int()>=0);
#endif
        return (l[n.to_int()].second=v);
    }

    auto begin() {
        auto n=NumberLabelingIterator(l.begin(),l.end(),def_);
        if (l.begin()==l.end()) return n;
        if (n->first<0 || n->second==def_) n++;
        return n;
    }
    auto end() {return NumberLabelingIterator(l.end(),l.end(),def_);}
};


} //end namespace ba_graph
#endif
