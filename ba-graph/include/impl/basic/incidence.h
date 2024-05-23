#ifndef BA_GRAPH_INCIDENCE_H
#define BA_GRAPH_INCIDENCE_H
#include "basic_objects.hpp"

namespace ba_graph {


/*
Incidence
*/

class Incidence {
    friend Factory;
    friend Rotation;
private:
    int id_code_;
    Rotation * r_;
    Rotation * r2_;
    Incidence(const Rotation * const r, const Rotation * const o, int id_code) :
            id_code_(id_code), r_(const_cast<Rotation *>(r)), r2_(const_cast<Rotation *>(o)) {}
    Incidence(const Incidence& that) = delete;
public:
    inline HalfEdge h1() const;
    inline HalfEdge h2() const;
    inline Edge e() const;
    inline Rotation& r1() const;
    inline Rotation& r2() const;

    inline Vertex v1() const;
    inline Vertex v2() const;

    inline Number n1() const;
    inline Number n2() const;

    inline Location l() const;
    inline Direction d() const;

    inline bool is_primary() const;
    inline bool is_loop() const;

    inline const Incidence& right() const;
    inline const Incidence& left() const;
    inline const Incidence& jump() const;

};



}  // namespace end
#endif
