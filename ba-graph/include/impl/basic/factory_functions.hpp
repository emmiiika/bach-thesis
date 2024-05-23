#ifndef BA_GRAPH_LOCAL_GRAPH_HPP
#define BA_GRAPH_LOCAL_GRAPH_HPP

namespace ba_graph {


inline Factory static_factory;


inline Graph createG(Factory &f=static_factory) { return f.cG(); }

#ifdef BA_GRAPH_DEBUG   
inline bool check(const Graph &g, const Factory &f=static_factory) {return f.check(g);}
#endif

inline Vertex createV(Factory &f=static_factory) { return f.cV(); }
inline Edge createE(const Vertex& v1, const Vertex& v2, Factory &f=static_factory) { return f.cE(v1,v2); }
inline void returnV(Vertex &v, Factory &f=static_factory) { f.returnV(v); }
inline void returnE(Edge &e, Factory &f=static_factory) { f.returnE(e); }


inline Rotation& addV(Graph &g, Factory &f=static_factory) { return f.aV(g); }
inline Rotation& addV(Graph &g, const Vertex &v, Factory &f=static_factory) { return f.aV(g,v); }
inline Rotation& addV(Graph &g, Number i, Factory &f=static_factory) { return f.aV(g,i); }
inline Rotation& addV(Graph &g, const Vertex &v, Number i, Factory &f=static_factory) { return f.aV(g,v,i); }

inline Number addMultipleV(Graph &g, int count, Factory &f=static_factory) { return f.aVs(g,count); }
inline void addMultipleV(Graph &g, int count, Number i, Factory &f=static_factory) { f.aVs(g,count, i); }
inline void addMultipleV(Graph &g, const std::vector<Vertex> &vv, Number i, Factory &f=static_factory) { f.aVs(g,vv, i); }
inline Number addMultipleV(Graph &g, const std::vector<Vertex> &vv, Factory &f=static_factory) { return f.aVs(g,vv); }
inline void addMultipleV(Graph &g, const std::vector<Number> &vn, Factory &f=static_factory) { f.aVs(g,vn); }
inline void addMultipleV(Graph &g, const std::vector<Vertex> &vv, const std::vector<Number> &vn, Factory &f=static_factory) { f.aVs(g,vv,vn); }

inline RotationIterator deleteV(Graph &g, RotationIterator i, Factory &f=static_factory) {return f.dV(g,i);}
inline Vertex deleteV(Graph &g, Number i, Factory &f=static_factory) { return f.dV(g,i); }
inline void deleteV(Graph &g, Vertex v, Factory &f=static_factory) { f.dV(g,g.find(v)->n()); }

inline IncidenceRefPair addE(Graph &g, Rotation& r1, Rotation& r2, Factory &f=static_factory) { return f.aE(g,r1,r2); }
inline IncidenceRefPair addE(Graph &g, Rotation& r1, Rotation& r2, const Edge &e, Factory &f=static_factory) { return f.aE(g,r1,r2,e); }
inline IncidenceRefPair addE(Graph &g, Location l, Factory &f=static_factory) { return f.aE(g,l); }
inline IncidenceRefPair addE(Graph &g, Direction l, Factory &f=static_factory) { return f.aE(g,l); }
inline IncidenceRefPair addE(Graph &g, const Vertex &v1, const Vertex &v2, Factory &f=static_factory) { return f.aE(g,v1,v2); }
inline IncidenceRefPair addE(Graph &g, const Edge &e, Factory &f=static_factory) { return f.aE(g,e); }
inline std::pair<IncidenceIterator,IncidenceIterator> addE(Graph &g, Rotation &v1r, Rotation &v2r, 
        IncidenceIterator ii1, IncidenceIterator ii2, Factory &f=static_factory) {return f.aE(g,v1r,v2r,ii1,ii2);}
inline std::pair<IncidenceIterator,IncidenceIterator> addE(Graph &g, Rotation &v1r, Rotation &v2r, 
        IncidenceIterator ii1, IncidenceIterator ii2, const Edge& e, Factory &f=static_factory) {return f.aE(g,v1r,v2r,ii1,ii2, e);}

inline void addMultipleE(Graph &g, std::vector<Location> vl, Factory &f=static_factory) { return f.aEs(g,vl); }
inline void addMultipleE(Graph &g, std::vector<Direction> vl, Factory &f=static_factory) { return f.aEs(g,vl); }
inline void addMultipleE(Graph &g, std::vector<std::pair<Vertex,Vertex>> vl, Factory &f=static_factory) { return f.aEs(g,vl); }
inline void addMultipleE(Graph &g, std::vector<Edge> vl, Factory &f=static_factory) { return f.aEs(g,vl); }

inline IncidenceIterator deleteE(Graph &g, IncidenceIterator i, Factory &f=static_factory) {return f.dE(g,i);}
inline void deleteE(Graph &g, const Rotation &r, Edge e, Factory &f=static_factory) {f.dE(g,r,e);}
inline Edge deleteE(Graph &g, Location l, Factory &f=static_factory) { return f.dE(g,l); }
inline Edge deleteE(Graph &g, Direction l, Factory &f=static_factory) { return f.dE(g,l); }
inline void deleteE(Graph &g, Edge e, Factory &f=static_factory) { f.dE(g,e); }

inline void deleteGraph(Graph &g, Factory &f=static_factory) { f.dG(g); }

inline void change_vertex_number(Graph &g, Vertex v, Number n, Factory &f=static_factory) {f.change_vertex_number(g,v,n);}
inline void change_vertex_number(Graph &g, Number n_old, Number n , Factory &f=static_factory) {f.change_vertex_number(g,n_old,n);}
inline void swap_vertices(Graph &g, Vertex v1, Vertex v2, Factory &f=static_factory) {f.swap_vertices(g,v1,v2);}
inline void swap_vertices(Graph &g, Number n1, Number n2, Factory &f=static_factory) {f.swap_vertices(g,n1,n2);}
inline void renumber_vertices(Graph &g, const Mapping<Number> &m, Factory &f=static_factory) {f.renumber_vertices(g,m);}
inline Mapping<Number> renumber_consecutive(Graph &g, Factory &f=static_factory) {return f.renumber_consecutive(g);}
inline Mapping<Number> renumber_consecutive_stable(Graph &g, Factory &f=static_factory) {return f.renumber_consecutive_stable(g);}


inline void swap_incidence(Graph &g, IncidenceIterator it1, IncidenceIterator it2, Factory &f=static_factory) {f.swap_incidence(g,it1,it2);}
inline void move_incidence(Graph &g, IncidenceIterator it1, IncidenceIterator it2, Factory &f=static_factory) {f.move_incidence(g,it1,it2);}




}
#endif
