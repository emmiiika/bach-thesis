#ifndef FACTORY_H
#define FACTORY_H
#include "classes.h"
#include "mapping.hpp"
#include <vector>

namespace ba_graph {

class IncidenceRefPair: public std::pair<Incidence &, Incidence &> {
public:
    IncidenceRefPair(Incidence & i1, Incidence &i2): std::pair<Incidence &, Incidence &>(i1,i2) {}
    IncidenceRefPair(std::pair<IncidenceIterator, IncidenceIterator> p): std::pair<Incidence &, Incidence &>(*(p.first),*(p.second)) {}
    Edge e() {return first.e();};
};



class Factory {
private:
    int nextId_;
    Factory(const Factory& that) = delete;
    Factory(Factory &&) = delete;
    Factory& operator=(const Factory &) = delete;
    Factory& operator=(Factory &&) = delete;
public:
    inline Factory();
    inline ~Factory();


    inline Graph cG();

    inline Vertex cV();
    inline Edge cE(const Vertex& v1, const Vertex& v2);
    inline void returnV(const Vertex &) {}
    inline void returnE(const Edge &) {}

#ifdef BA_GRAPH_DEBUG    
    inline bool check(const Graph &g) const {return (this == g.my_factory);}
#endif

    inline Rotation& aV(Graph &g) {return aV(g, cV(), g.free_number++);}; 
    inline Rotation& aV(Graph &g, const Vertex &v) {return aV(g, v, g.free_number++);}; 
    inline Rotation& aV(Graph &g, Number i) {return aV(g, cV(), i);}; //specifies vertex number
    inline Rotation& aV(Graph &g, const Vertex& v, Number i); 

    inline Number aVs(Graph &g, int count); //returns first ID
    inline void aVs(Graph &g, int count, Number i);
    inline void aVs(Graph &g, const std::vector<Vertex> &, Number i);
    inline void aVs(Graph &g, const std::vector<Number> &);
    inline Number aVs(Graph &g, const std::vector<Vertex> &);
    inline void aVs(Graph &g, const std::vector<Vertex> &, const std::vector<Number> &);


    inline RotationIterator dV(Graph &g, RotationIterator i); //returns next element
    inline Vertex dV(Graph &g, Number i); //also destroys the neigbhbourhood


    inline std::pair<IncidenceIterator, IncidenceIterator> aE(Graph &g, Rotation &r1, Rotation &r2, 
            IncidenceIterator ii1, IncidenceIterator ii2, const Edge &e);
    inline std::pair<IncidenceIterator, IncidenceIterator> aE(Graph &g, Rotation &r1, Rotation &r2, 
            IncidenceIterator ii1, IncidenceIterator ii2) {return aE(g, r1, r2, ii1, ii2, cE(r1.v(),r2.v()));}

    inline IncidenceRefPair aE(Graph &g, Rotation &r1, Rotation &r2, const Edge &e);
    inline IncidenceRefPair aE(Graph &g, Rotation &v1r, Rotation &v2r) {return aE(g, v1r, v2r, cE(v1r.v(),v2r.v()));}
    inline IncidenceRefPair aE(Graph &g, Location l);
    inline IncidenceRefPair aE(Graph &g, Direction l);
    inline IncidenceRefPair aE(Graph &g, const Vertex &v1, const Vertex &v2);
    inline IncidenceRefPair aE(Graph &g, const Edge &e);

    inline void aEs(Graph &g, const std::vector<Location> &vl);
    inline void aEs(Graph &g, const std::vector<Direction> &vl);
    inline void aEs(Graph &g, const std::vector<std::pair<Vertex, Vertex>> &vpv);
    inline void aEs(Graph &g, const std::vector<Edge> &ve);

    inline IncidenceIterator dE(Graph &g, IncidenceIterator i); //returns next element
    inline void dE(Graph &g, const Rotation &r, Edge e);
    inline void dE(Graph &g, Edge e);
    inline Edge dE(Graph &g, Location l);
    inline Edge dE(Graph &g, Direction l);

    inline void dG(Graph &g); //deletes all edges and vertices


    inline void change_vertex_number(Graph &g, Vertex v, Number n);
    inline void change_vertex_number(Graph &g, Number n_old, Number n);
    inline void swap_vertices(Graph &g, Vertex v1, Vertex v2);
    inline void swap_vertices(Graph &g, Number n1, Number n2);
    inline void renumber_vertices(Graph &g, const Mapping<Number> &m);
    inline Mapping<Number> renumber_consecutive(Graph &g);
    inline Mapping<Number> renumber_consecutive_stable(Graph &g);

    inline void swap_incidence(Graph &g, IncidenceIterator it1, IncidenceIterator it2);
    inline void move_incidence(Graph &g, IncidenceIterator it1, IncidenceIterator it2);

};

}  // namespace end
#endif
