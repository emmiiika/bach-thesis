#ifndef BA_GRAPH_STRUCTURES_TRAIL_HPP
#define BA_GRAPH_STRUCTURES_TRAIL_HPP
#include<utility>
#include<deque>

namespace ba_graph {

class Trail;

class TrailIterator:public std::iterator<std::bidirectional_iterator_tag, std::pair<Vertex,Edge>> {
    friend Trail;
protected:
    std::deque<std::pair<Vertex,Edge>>::const_iterator i_;
    const Rotation *r_;
private:
    TrailIterator(std::deque<std::pair<Vertex,Edge>>::const_iterator i) : i_(i), r_(NULL) {};
    TrailIterator(std::deque<std::pair<Vertex,Edge>>::const_iterator i, const Rotation &r) : i_(i), r_(&r) {};
public:
    TrailIterator() : r_(NULL) {};
    inline TrailIterator& operator++() {
        if (r_!=NULL) {
            auto nit=r_->find(i_->second);
            if (nit ==r_->end()) r_=NULL;
            else r_=&(nit->r2());
        }
        ++i_; return *this;
    };
    inline TrailIterator operator++(int) {
        if (r_!=NULL) {
            auto nit=r_->find(i_->second);
            if (nit ==r_->end()) r_=NULL;
            else r_=&(nit->r2());
        }
        auto tmp = *this; ++i_; return tmp;
    }
    inline TrailIterator& operator--() {
        --i_;
        if (r_!=NULL) {
            auto nit=r_->find(i_->second);
            if (nit ==r_->end()) r_=NULL;
            else r_=&(nit->r2());
        }
        return *this;
    };
    inline TrailIterator operator--(int) {
        auto tmp = *this; --i_;
        if (r_!=NULL) {
            auto nit=r_->find(i_->second);
            if (nit ==r_->end()) r_=NULL;
            else r_=&(nit->r2());
        }
        return tmp;
    }
    inline TrailIterator operator+(int n) {
        auto it=*this;
        for(int i=0;i<n;i++) it++;
        return it;
    }
    inline TrailIterator operator-(int n) {
        auto it=*this;
        for(int i=0;i<n;i++) it--;
        return it;
    }
    inline bool operator==(TrailIterator j) const {return i_==j.i_;};
    inline bool operator!=(TrailIterator j) const {return i_!=j.i_;};
    inline const std::pair<Vertex,Edge> & operator*() const {return *i_;};
    inline const std::pair<Vertex,Edge> * operator->() const {return &(*i_);};
    inline const Rotation & r() {return *r_;}
};


class Trail {
    std::deque<std::pair<Vertex,Edge>> dq_;
public:
    Trail(std::vector<Vertex> vv, std::vector<Edge> ve) {
#ifdef BA_GRAPH_DEBUG
        assert(vv.size()==ve.size()+1);
#endif
        auto it1=vv.begin();
        auto it2=ve.begin();
        for(;it2!=ve.end();it1++,it2++)
            dq_.push_back(std::pair(*it1,*it2));
        dq_.push_back(std::pair(*it1,Edge()));
    }
    Trail(const Rotation &r, std::vector<Number> vn) {
        const Rotation *r2=&r;
        for(auto it=vn.begin();it!=vn.end();it++) {
            auto ii=r2->find(*it);
            dq_.push_back(std::pair(r2->v(),ii->e()));
            r2=&(ii->r2());
        }
        dq_.push_back(std::pair(r2->v(),Edge()));
    }

    TrailIterator begin() {return TrailIterator(dq_.begin());}
    TrailIterator begin(const Rotation &r) {return TrailIterator(dq_.begin(), r);}
    TrailIterator end() {return TrailIterator(dq_.end());}

    TrailIterator last_it() {return TrailIterator(--(dq_.end()));}
    TrailIterator last_it(const Rotation &r) {return TrailIterator(--(dq_.end()), r);}

    int length() const {return dq_.size()-1;}

    bool operator==(const Trail &t) const {
        if (length()!=t.length()) return false;
        auto it1=dq_.begin();
        auto it2=t.dq_.begin();
        for(;it1!=dq_.end();it1++,it2++)
           if(*it1!=*it2) return false;
        return true;
    };
    bool operator!=(const Trail &t) const {return !(*this==t);}

    bool is_path() {
        std::set<Vertex> s;
        for(auto it=begin();it!=last_it();it++)
            s.insert(it->first);
        return ((int)s.size()==length());
    }
    bool is_closed() {return begin()->first==last_it()->first;}


    TrailIterator replace_subtrail(TrailIterator first, TrailIterator last,
    TrailIterator first2, TrailIterator last2) {
#ifdef BA_GRAPH_DEBUG
        assert(first->first==first2->first);
        assert(last->first==last2->first);
#endif
        auto np=dq_.erase(first.i_, last.i_);
        dq_.insert(np,first2.i_,last2.i_);
        return TrailIterator(np,*first.r_);
    }

    TrailIterator replace_subtrail(TrailIterator first, TrailIterator last,
    Trail trail) {
        return replace_subtrail(first, last, trail.begin(), trail.last_it());
    }



};

}  // namespace end





#endif
