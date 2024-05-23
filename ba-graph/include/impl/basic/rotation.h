#ifndef BA_GRAPH_ROTATION_H
#define BA_GRAPH_ROTATION_H
#include "basic_objects.hpp"
#include <type_traits>
#include <vector>
#include <set>


namespace ba_graph {

/*
IncidenceIterator
*/

class IncidenceIterator:public std::iterator<std::bidirectional_iterator_tag, Rotation> {
    friend Factory;
    friend Graph;
    friend Rotation;
private:
    inline IncidenceIterator(std::vector<Incidence *>::const_iterator i);
protected:
    std::vector<Incidence *>::const_iterator i_;
public:
    inline IncidenceIterator();
    inline IncidenceIterator& operator++();
    inline IncidenceIterator operator++(int);
    inline IncidenceIterator& operator--();
    inline IncidenceIterator operator--(int);
    inline IncidenceIterator operator+(int);
    inline IncidenceIterator operator-(int);
    inline size_t operator-(IncidenceIterator);
    inline bool operator==(IncidenceIterator j) const;
    inline bool operator!=(IncidenceIterator j) const;
    inline bool operator>=(IncidenceIterator j) const;
    inline bool operator<=(IncidenceIterator j) const;
    inline bool operator<(IncidenceIterator j) const;
    inline bool operator>(IncidenceIterator j) const;
    inline Incidence & operator*() const;
    inline Incidence * operator->() const;
};

/*
Rotation
*/

class Rotation {
    friend Factory;
    friend Graph;
    friend Incidence;
    friend IncidenceIterator;
private:
    Vertex v_;
    Number n_;
    std::vector<Incidence *> rr_;
    inline Rotation(const Vertex &v, Number n);
    inline ~Rotation();
    Rotation(const Rotation& that) = delete;
public:
    inline const Incidence & operator[] (int i) const;


    inline Number n() const;
    inline const Vertex& v() const;
    inline bool isolated() const;
    inline int degree() const;

    inline IncidenceIterator begin() const;
    inline IncidenceIterator end() const;

    inline std::reverse_iterator<IncidenceIterator> rbegin() const;
    inline std::reverse_iterator<IncidenceIterator> rend() const;

    template<typename T>
    inline IncidenceIterator find(T &&p) const
        requires IncidencePredicate<T> || RotationLocatable<T>;
    template<typename T>
    inline int count(T &&p) const
        requires IncidencePredicate<T> || RotationLocatable<T>;
    template<typename T>
    inline bool contains(T &&p) const
        requires IncidencePredicate<T> || RotationLocatable<T>;


    template<typename T, IncidenceFunction F>
    inline auto list(T &&p, F &&f) const
        requires (IncidencePredicate<T> || RotationLocatable<T>);
    template<typename T>
    inline std::vector<IncidenceIterator> list(T &&p) const
        requires IncidencePredicate<T> || RotationLocatable<T>;
    template<typename T, IncidenceComparator V, IncidenceFunction F>
    inline auto list(T &&p, const V &c, F &&f) const
        requires (IncidencePredicate<T> || RotationLocatable<T>);
    template<typename T, IncidenceComparator V>
    inline std::vector<IncidenceIterator> list(T &&p, const V &c) const
        requires IncidencePredicate<T> || RotationLocatable<T>;


    inline std::set<Number> neighbours() const;

};


}  // namespace end
#endif
