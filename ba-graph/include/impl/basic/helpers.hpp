#ifndef BA_GRAPH_BASIC_HELPERS_HPP
#define BA_GRAPH_BASIC_HELPERS_HPP

namespace ba_graph {

template<typename T, typename V>
std::vector<T> copy_and_sort(std::vector<T> vec, const V &c) {
    std::sort(vec.begin(), vec.end(), c);
    return vec;
}

template<typename T, typename V, typename U>
auto sort_and_transform(std::vector<T> vec, const V &c, U &&t) -> std::vector<decltype(t(*vec.begin()))>{
    std::sort(vec.begin(), vec.end(), c);
    std::vector<decltype(t(*vec.begin()))> res;
    res.reserve(vec.size());
    for(auto ii: vec) res.push_back(t(ii)); //if we used std::transform side-efects on t might be lost
    return res;
}






namespace IP {

template<int n, typename U, typename V>
class IP_and {
    U p1;
    V p2;
public:
    IP_and(U u, V v): p1(u), p2(v) {}
    bool operator()(const Incidence &i) {
        if constexpr (n==0)
            return p1(i) && p2(i);
        if constexpr (n==1)
            return p1(i) && (*p2)(i);
        if constexpr (n==2)
            return (*p1)(i) && p2(i);
        if constexpr (n==3)
            return (*p1)(i) && (*p2)(i);
    };
};


template<IncidencePredicate U, IncidencePredicate V>
auto operator &&(U &&u, V&&v) {
    if constexpr (std::is_lvalue_reference<U>::value) {
        if constexpr (std::is_lvalue_reference<V>::value)
            return IP_and<3, typename std::remove_reference<U>::type *, typename std::remove_reference<V>::type *>(&u,&v);
        else
            return IP_and<2, typename std::remove_reference<U>::type *, V>(&u,v);
    }
    else {
        if constexpr (std::is_lvalue_reference<V>::value)
            return IP_and<1, U, typename std::remove_reference<V>::type *>(u,&v);
        else
            return IP_and<0, U, V>(u,v);
    }
}



template<int n, typename U, typename V>
class IP_or {
    U p1;
    V p2;
public:
    IP_or(U u, V v): p1(u), p2(v) {}
    bool operator()(const Incidence &i) {
        if constexpr (n==0)
            return p1(i) || p2(i);
        if constexpr (n==1)
            return p1(i) || (*p2)(i);
        if constexpr (n==2)
            return (*p1)(i) || p2(i);
        if constexpr (n==3)
            return (*p1)(i) || (*p2)(i);
    };
};


template<IncidencePredicate U, IncidencePredicate V>
auto operator ||(U &&u, V&&v) {
    if constexpr (std::is_lvalue_reference<U>::value) {
        if constexpr (std::is_lvalue_reference<V>::value)
            return IP_or<3, typename std::remove_reference<U>::type *, typename std::remove_reference<V>::type *>(&u,&v);
        else
            return IP_or<2, typename std::remove_reference<U>::type *, V>(&u,v);
    }
    else {
        if constexpr (std::is_lvalue_reference<V>::value)
            return IP_or<1, U, typename std::remove_reference<V>::type *>(u,&v);
        else
            return IP_or<0, U, V>(u,v);
    }
}


template<int n, typename U>
class IP_not {
    U p1;
public:
    IP_not(U u): p1(u) {}
    bool operator()(const Incidence &i) {
        if constexpr (n==0)
            return !p1(i);
        if constexpr (n==1)
            return !(*p1)(i);
    };
};


template<IncidencePredicate U>
auto operator !(U &&u) {
    if constexpr (std::is_lvalue_reference<U>::value)
        return IP_not<1, typename std::remove_reference<U>::type *>(&u);
    else
        return IP_not<0, U>(u);
}

} //end namespace IP




namespace RP {

template<int n, typename U, typename V>
class RP_and {
    U p1;
    V p2;
public:
    RP_and(U u, V v): p1(u), p2(v) {}
    bool operator()(const Rotation &i) {
        if constexpr (n==0)
            return p1(i) && p2(i);
        if constexpr (n==1)
            return p1(i) && (*p2)(i);
        if constexpr (n==2)
            return (*p1)(i) && p2(i);
        if constexpr (n==3)
            return (*p1)(i) && (*p2)(i);
    };
};


template<RotationPredicate U, RotationPredicate V>
auto operator &&(U &&u, V&&v) {
    if constexpr (std::is_lvalue_reference<U>::value) {
        if constexpr (std::is_lvalue_reference<V>::value)
            return RP_and<3, typename std::remove_reference<U>::type *, typename std::remove_reference<V>::type *>(&u,&v);
        else
            return RP_and<2, typename std::remove_reference<U>::type *, V>(&u,v);
    }
    else {
        if constexpr (std::is_lvalue_reference<V>::value)
            return RP_and<1, U, typename std::remove_reference<V>::type *>(u,&v);
        else
            return RP_and<0, U, V>(u,v);
    }
}



template<int n, typename U, typename V>
class RP_or {
    U p1;
    V p2;
public:
    RP_or(U u, V v): p1(u), p2(v) {}
    bool operator()(const Rotation &i) {
        if constexpr (n==0)
            return p1(i) || p2(i);
        if constexpr (n==1)
            return p1(i) || (*p2)(i);
        if constexpr (n==2)
            return (*p1)(i) || p2(i);
        if constexpr (n==3)
            return (*p1)(i) || (*p2)(i);
    };
};


template<RotationPredicate U, RotationPredicate V>
auto operator ||(U &&u, V&&v) {
    if constexpr (std::is_lvalue_reference<U>::value) {
        if constexpr (std::is_lvalue_reference<V>::value)
            return RP_or<3, typename std::remove_reference<U>::type *, typename std::remove_reference<V>::type *>(&u,&v);
        else
            return RP_or<2, typename std::remove_reference<U>::type *, V>(&u,v);
    }
    else {
        if constexpr (std::is_lvalue_reference<V>::value)
            return RP_or<1, U, typename std::remove_reference<V>::type *>(u,&v);
        else
            return RP_or<0, U, V>(u,v);
    }
}


template<int n, typename U>
class RP_not {
    U p1;
public:
    RP_not(U u): p1(u) {}
    bool operator()(const Rotation &i) {
        if constexpr (n==0)
            return !p1(i);
        if constexpr (n==1)
            return !(*p1)(i);
    };
};


template<RotationPredicate U>
auto operator !(U &&u) {
    if constexpr (std::is_lvalue_reference<U>::value)
        return RP_not<1, typename std::remove_reference<U>::type *>(&u);
    else
        return RP_not<0, U>(u);
}

} //end namespace RP


}  //end namespace
#endif
