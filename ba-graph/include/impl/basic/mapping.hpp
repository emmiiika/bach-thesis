#ifndef BA_GRAPH_BASIC_MAPPING_HPP
#define BA_GRAPH_BASIC_MAPPING_HPP
#include <map>
#include <iterator>

namespace ba_graph {

template <typename Value> class Mapping {
private:
    std::map<Value,Value> m_;

    void remove_fixed_points() {
        for(auto it=m_.begin();it!=m_.end();) {if (it->first==it->second) it=m_.erase(it); else it++;}
    }

public:
    using key_type=Value;
    using mapped_type=Value;
    using value_type=std::pair<const key_type, mapped_type>;

    using const_iterator=typename std::map<Value, Value>::const_iterator;
    using const_reverse_iterator=typename std::map<Value, Value>::const_reverse_iterator;

    Mapping() {}
    ~Mapping() {}


    Value operator[](const Value &key) const{
        return get(key);
    }

    Value get(const Value &key) const{
        auto it=m_.find(key);
        if (it!=m_.end()) return it->second;
        return key;
    }

    Value set(const Value& key, const Value& v) {
        if (key==v) {m_.erase(key); return key;}
        return (m_[key]=v);
    }


    const_iterator begin() const {return std::cbegin(m_);}
    const_iterator end() const {return std::cend(m_);}

    const_reverse_iterator rbegin() const {return std::crbegin(m_);}
    const_reverse_iterator rend() const {return std::crend(m_);}


    void compose_with(const Mapping &m2) {
        for(auto it=m_.begin();it!=m_.end();it++)
            it->second=m2.get(it->second);
        for(auto it=m2.m_.begin();it!=m2.m_.end();it++)
            if (m_.find(it->first)==m_.end()) m_[it->first]=it->second;
        remove_fixed_points();
    }

};




} //end namespace ba_graph
#endif
