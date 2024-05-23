#ifndef BA_GRAPH_GRAPH_H
#define BA_GRAPH_GRAPH_H
#include "basic_objects.hpp"
#include "incidence.h"
#include "rotation.h"

#include <map>
#include <utility>

namespace ba_graph {

/*
RotationIterator
*/


class RotationIterator:public std::iterator<std::bidirectional_iterator_tag, Rotation> {
    friend Factory;
    friend Graph;
private:
    inline RotationIterator(std::map<int, Rotation *>::const_iterator i);
protected:
    std::map<int, Rotation *>::const_iterator i_;
public:
    inline RotationIterator();
    inline RotationIterator& operator++();
    inline RotationIterator operator++(int);
    inline bool operator==(RotationIterator j) const;
    inline bool operator!=(RotationIterator j) const;
    inline Rotation & operator*() const;
    inline Rotation * operator->() const;
};


/*
Graph
*/

class Graph {
public:
    friend Factory;
    friend RotationIterator;
private:
#ifdef BA_GRAPH_DEBUG
    Factory *my_factory;
#endif
    int edge_count;
    int free_number; //od tohoto cisla su vsetky cisla vrcholov volne
    std::map<int, Rotation *> rr_;

#ifdef BA_GRAPH_DEBUG
    inline Graph(Factory * f);
#else
    inline Graph();
#endif
    Graph(const Graph& that) = delete; //no copy constructor
public:
    inline Graph ( Graph && ) noexcept;
    inline Graph& operator=( Graph &&other);
    inline ~Graph();


    inline int order() const;
    inline int size() const;


    inline Rotation & operator[] (Number i) const;
    inline Incidence & operator[] (Location l) const;
    inline Incidence & operator[] (Direction d) const;

    inline RotationIterator begin() const;
    inline RotationIterator end() const;
    inline std::reverse_iterator<RotationIterator> rbegin() const;
    inline std::reverse_iterator<RotationIterator> rend() const;

    //rotation find functions
    template <typename T>
    inline RotationIterator find(T &&p) const
            requires RotationPredicate<T> || GraphLocatable<T>;
    template <typename T>
    inline int count(T &&p) const
            requires RotationPredicate<T> || GraphLocatable<T>;
    template <typename T>
    inline bool contains(T &&p) const
            requires RotationPredicate<T> || GraphLocatable<T>;
    template <typename T>
    inline std::vector<RotationIterator> list(T &&p) const
            requires RotationPredicate<T> || GraphLocatable<T>;
    template <typename T, RotationFunction U>
    inline auto list(T &&p, U &&f) const -> std::vector<decltype(f(begin()))>
            requires RotationPredicate<T> || GraphLocatable<T>;
    template <typename T, RotationComparator V>
    inline std::vector<RotationIterator> list(T &&p, const V &c) const
            requires RotationPredicate<T> || GraphLocatable<T>;
    template <typename T, RotationComparator V, RotationFunction U>
    inline auto list(T &&p, const V &c, U &&f) const -> std::vector<decltype(f(begin()))>
            requires RotationPredicate<T> || GraphLocatable<T>;

    //incidence find functions
    template<typename S, typename T>
    inline std::pair<RotationIterator, IncidenceIterator> find(S &&rp, T &&ip) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>);
    template<typename S, typename T>
    inline int count(S &&rp, T &&ip) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>);
    template<typename S, typename T>
    inline bool contains(S &&rp, T &&ip) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>);
    template<typename S, typename T>
    inline std::vector<IncidenceIterator> list(S &&rp, T &&ip) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>);
    template<typename S, typename T, IncidenceFunction U>
    inline auto list(S &&rp, T &&ip, U &&f) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>);
    template<typename S, typename T, IncidenceComparator V>
    inline std::vector<IncidenceIterator> list(S &&rp, T &&ip, const V& c) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>);
    template<typename S, typename T, IncidenceComparator V, IncidenceFunction U>
    inline auto list(S &&rp, T &&ip, const V& c, U &&f) const
            requires (RotationPredicate<S> || GraphLocatable<S>)
            && (IncidencePredicate<T> || RotationLocatable<T>);


    inline RotationIterator find(Number &&p) const;
    inline bool contains(Number &&p) const;

    //location find functions
    inline IncidenceIterator find(const Location &loc) const;
    inline bool contains(const Location &loc) const;

    //direction find functions
    inline IncidenceIterator find(const Direction &loc) const;
    inline bool contains(const Direction &loc) const;

};


}  // namespace end
#endif
