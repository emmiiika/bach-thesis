# ba-graph

###Classes

The following main classes are provided by ba_graph:

* **Id** - Id of a Vertex or an Edge.
* **Vertex** - A vertex. It existance is not bound to a graph.
* **Edge** - An Edge. It connects two vertices. It existence is not bound to a graph.
* **HalfEdge** - A Halfedge.
* **Number** - Vertices in a graph have their number.
* **Location** - Edges in a graph have a location, which consists of two number and index to
    distinguish multiple edges. The index may change if the graph changes, but the indices of
    multiple edges are guaranteed to be consecutive integers starting with 0.
* **Rotation** - Represents an edge within the graph. Factory stores the instances of this class.
    You should work with it using a (const) reference.
* **Incidence** - Represents an halfedge within the graph. Factory stores the instances of this class.
    You should work with it using a (const) reference.
* **Graph** - Represents a graph.
* **Factory** - Creates, stores and modifies all other classes. You need not call the methods of
    this class as there are functions providing the functionality. Tn these functions Factory is
    an optional argument, where an instance of Factory static_factory is used by default.


###Settings

You can define the following variables. For now write this down into ba_graph.hpp.

* BA_GRAPH_IMPL
Specifies the implementation used.
* BA_GRAPH_DEBUG
If defined the code compiles with additional assertion checks.
* BA_GRAPH_MAX_VERTICES
Sets maximal number of vertices.
* BA_GRAPH_MAX_EDGES
Sets maximal number of vertices.
* BA_GRAPH_MAX_VERTICES_IN_GRAPH
Sets maximal number of vertices.
* MAX_DEGREE
Sets maximal degree.


###Interface

To make things easier we consistently use the following short forms (capitalized, if it is second
word in the method name)
* Vertex - v/V
* Edge - e/E
* HalfEdge - h/H
* Number - n/N
* Rotation - r/R
* Incidence - i/I

#####Factory

The library provides the following functions to work with Factory. Besides the arguments present here
we may specify a specific Factory object as a last arguments (static_factory) is used as default.
Objects from different Factories live in distinct universes an you cannot mix them. Destroying Factory
destroys all objects it stores.

* inline Vertex createV();
* inline Edge createE(const Vertex& v1, const Vertex& v2);
* inline void returnV(Vertex &v);
* inline void returnE(Edge &e);
* inline Number addV(Graph &g);
* inline Number addV(Graph &g, const Vertex &v);
* inline Vertex addV(Graph &g, Number i);
* inline void addV(Graph &g, const Vertex &v, Number i);
* inline Number addMultipleV(Graph &g, int count);
* inline void addMultipleV(Graph &g, int count, Number i);
* inline void deleteV(Graph &g, Number i);
* inline void deleteV(Graph &g, Vertex v);
* inline Edge addE(Graph &g, const Location &l);
* inline Edge addE(Graph &g, const Vertex &v1, const Vertex &v2);
* inline void addE(Graph &g, const Edge &e);
* inline Edge deleteE(Graph &g, Edge e);



#####Id
TODO

#####Vertex
TODO

#####Edge
TODO

#####HalfEdge
TODO

#####Incidence
TODO

#####Rotation
TODO

#####Graph
TODO

#####CreatedGraph
TODO

#####SavedGraph
TODO


