#ifndef BA_GRAPH_CLASSES_H
#define BA_GRAPH_CLASSES_H
#include <type_traits>

namespace ba_graph {
class Vertex;
class Edge;
class HalfEdge;

class Number;
class Location;

class Incidence;
class IncidenceIterator;
class Rotation;
class RotationIterator;
class Graph;

class Factory;

template<typename T>
concept bool IncidencePredicate = requires(T a, const Incidence &i) {
    {a(i)} -> bool;
};

template<typename T>
concept bool RotationPredicate = requires(T a, const Rotation &i) {
    {a(i)} -> bool;
};


template<typename T>
concept bool IncidenceFunction = requires(T a, IncidenceIterator it) {
    {a(it)} -> decltype(a(it));
};


template<typename T>
concept bool RotationFunction = requires(T a, RotationIterator it) {
    {a(it)} -> decltype(a(it));
};


template<typename T>
concept bool IncidenceComparator = requires(T a, IncidenceIterator i1, IncidenceIterator i2) {
    {a(i1,i2)} -> bool;
};

template<typename T>
concept bool RotationComparator = requires(T a, RotationIterator r1, RotationIterator r2) {
    {a(r1, r2)} -> bool;
};



namespace RP {
template<typename T> class RP;
}
namespace IP {
template<typename T> class IP;
}

template<typename T>
concept bool RotationLocatable = requires(T a, const Incidence &b) {
    {(IP::IP<typename std::decay<T>::type>(a))(b)} -> bool;
};

template<typename T>
concept bool GraphLocatable = requires(T a, const Rotation &b) {
    {(RP::RP<typename std::decay<T>::type>(a))(b)} -> bool;
};


}

#endif
