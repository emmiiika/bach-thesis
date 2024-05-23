#ifndef BA_GRAPH_OPERATIONS_MAPPERS_HPP
#define BA_GRAPH_OPERATIONS_MAPPERS_HPP

namespace ba_graph {

class NoMapper {
public:
    typedef int MappingType;
    static const int mappings=0;

    static void addNV(MappingType &, const Number &, const Number &, Vertex, Vertex) {}
    static void addH(MappingType &, HalfEdge, HalfEdge) {}
    static void addE(MappingType &, Edge, Edge) {}
};


class NumberMapper {
public:
    typedef Mapping<Number> MappingType;
    static const int mappings=1;

    static void addNV(MappingType &m, const Number &r1, const Number &r2, Vertex, Vertex) {
        m.set(r1,r2);
    }
    static void addH(MappingType &, HalfEdge, HalfEdge) {}
    static void addE(MappingType &, Edge, Edge) {}
};



class VertexMapper {
public:
    typedef Mapping<Vertex> MappingType;
    static const int mappings=1;

    static void addNV(MappingType &m, const Number, const Number, Vertex v1, Vertex v2) {m.set(v1,v2);}
    static void addH(MappingType &, HalfEdge, HalfEdge) {}
    static void addE(MappingType &, Edge, Edge) {}
};



class HalfEdgeMapper {
public:
    typedef Mapping<HalfEdge> MappingType;
    static const int mappings=1;

    static void addNV(MappingType &, const Number &, const Number &, Vertex, Vertex) {}
    static void addH(MappingType &m, HalfEdge from, HalfEdge to) { m.set(from,to); }
    static void addE(MappingType &m, Edge from, Edge to) {
        m.set(from.h1(),to.h1());
        m.set(from.h2(),to.h2());
    }
};

class EdgeMapper {
public:
    typedef Mapping<Edge> MappingType;
    static const int mappings=1;

    static void addNV(MappingType &, const Number &, const Number &, Vertex, Vertex) {}
    static void addH(MappingType &, HalfEdge, HalfEdge) {}
    static void addE(MappingType &m, Edge from, Edge to) { m.set(from,to);}
};


template<typename Mapper1, typename Mapper2>
class MapperPair {
public:
    typedef std::pair<typename Mapper1::MappingType, typename Mapper2::MappingType> MappingType;
    static const int mappings=1;

    static void addNV(MappingType &m, const Number &n1, const Number &n2, Vertex v1, Vertex v2) {
        Mapper1::addNV(m.first, n1, n2, v1, v2);
        Mapper2::addNV(m.second, n1, n2, v1, v2);
    }
    static void addH(MappingType &m, HalfEdge h1, HalfEdge h2) {
        Mapper1::addH(m.first, h1, h2);
        Mapper2::addH(m.second, h1, h2);
    }
    static void addE(MappingType &m, Edge e1, Edge e2) {
        Mapper1::addE(m.first, e1, e2);
        Mapper2::addE(m.second, e1, e2);
    }
};



//if you want to return one extra return valuesomething besides besides labeling
template<typename ReturnType, typename Mapper, int n=Mapper::mappings> class OneExtraReturnValue;

template<typename BasicReturnType, typename Mapper>
class OneExtraReturnValue<BasicReturnType,Mapper,0> {
public:
    typedef BasicReturnType ReturnType;
    static ReturnType ret(BasicReturnType &H, typename Mapper::MappingType) {
            return std::move(H);
    }
};

template<typename BasicReturnType, typename Mapper>
class OneExtraReturnValue<BasicReturnType,Mapper,1> {
public:
    typedef std::pair<BasicReturnType,typename Mapper::MappingType> ReturnType;
    static ReturnType ret(BasicReturnType &H, typename Mapper::MappingType l) {
            return std::move(ReturnType(std::move(H),l));
    }
};

}  // namespace end

#endif
