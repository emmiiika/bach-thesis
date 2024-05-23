#ifndef BA_GRAPH_OPERATIONS_ADD_GRAPH_HPP
#define BA_GRAPH_OPERATIONS_ADD_GRAPH_HPP

namespace ba_graph {

/*
 * Adds all vertices and edges from S to G;
 * they are only added if they are not present already
 * (basically, we are doing a union of sets).
 * (Each vertex and edge has identity given to it by the factory that created it.
 * This identity is used here for determining if two elements are the same or not.)
 */
template <typename X = NoMapper>
typename X::MappingType add_graph(Graph &T, const Graph &S, int offset, Factory &f=static_factory) {
#ifdef BA_GRAPH_DEBUG
    assert(check(T, f));
    assert(check(S, f));
#endif
    typename X::MappingType m;
    VertexLabeling<Number> l(0); //where the vertices of S are in T
    for(auto &rs: S) { //add vertices if necessary
        auto it=T.find(rs.v());
        if (it!=T.end()) { //vertex in T
            l.set(rs.v(), it->n());
            X::addNV(m, rs.n(), it->n(), rs.v(), rs.v());
        }
        else { //vertex not in t
            l.set(rs.v(), rs.n()+offset);
#ifdef BA_GRAPH_DEBUG
            assert(!T.contains(Number(rs.n()+offset)));
#endif
            addV(T, rs.v(), rs.n()+offset, f);
            X::addNV(m, rs.n(), rs.n()+offset, rs.v(), rs.v());
        }
    }

    for(auto ii: S.list(RP::all(), IP::primary())) {
        Number n1=l[ii->v1()];
        if (!T[n1].contains(ii->e())) {
            addE(T, ii->e(), f);
        }
    }

    return m;
}


/*
 * Adds disjoint copies all vertices and edges from S to T;
 */
template <typename X = NoMapper>
typename X::MappingType add_disjoint(Graph &T, const Graph &S, int offset, Factory &f=static_factory) {
#ifdef BA_GRAPH_DEBUG
    assert(check(T, f));
    assert(check(S, f));
#endif
    typename X::MappingType m;
    for(auto &rs: S) {
#ifdef BA_GRAPH_DEBUG
        assert(!T.contains(Number(rs.n()+offset)));
#endif
        Vertex v=addV(T, rs.n()+offset, f).v();
        X::addNV(m, rs.n(), rs.n()+offset, rs.v(), v);
    }

    for(auto ii: S.list(RP::all(), IP::primary())) {
        Edge e=addE(T, Location(ii->n1()+offset,ii->n2()+offset), f).e();
        X::addE(m, ii->e(), e);
    }

    return m;
}

}  // namespace end

#endif
