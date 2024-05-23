#ifndef BA_GRAPH_IO_PRINT_NICE_HPP
#define BA_GRAPH_IO_PRINT_NICE_HPP

#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace ba_graph
{

template <typename T>
std::string print_nice(const T &t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

template <typename T, typename U>
inline std::ostream & operator<<(std::ostream &out, const std::pair<T, U> &p)
{
	out << "(" << p.first << ", " << p.second << ")";
	return out;
}

template <typename T>
inline std::ostream & operator<<(std::ostream &out, const std::vector<T> &container)
{
	out << "[";
	std::string separator = "";
	for (auto &t : container) {
		out << separator << t;
		separator = " ";
	}
	out << "]";
	return out;
}

template <typename T>
inline std::ostream & operator<<(std::ostream &out, const std::set<T> &container)
{
	out << "{";
	std::string separator = "";
	for (auto &t : container) {
		out << separator << t;
		separator = " ";
	}
	out << "}";
	return out;
}

template <typename T>
inline std::ostream & operator<<(std::ostream &out, const std::multiset<T> &container)
{
	out << "{";
	std::string separator = "";
	for (auto &t : container) {
		out << separator << t;
		separator = " ";
	}
	out << "}";
	return out;
}

template <typename T>
inline std::ostream & operator<<(std::ostream &out, const std::unordered_set<T> &container)
{
	out << "{";
	std::string separator = "";
	for (auto &t : container) {
		out << separator << t;
		separator = " ";
	}
	out << "}";
	return out;
}

template <typename U, typename V>
inline std::ostream & operator<<(std::ostream &out, const std::map<U, V> &container)
{
	out << "{";
	std::string separator = "";
	for (auto &t : container) {
		out << separator << t.first << ":" << t.second;
		separator = ", ";
	}
	out << "}";
	return out;
}

inline std::ostream & operator<<(std::ostream &out, const Vertex v)
{
	out << v.to_int();
	return out;
}

inline std::ostream & operator<<(std::ostream &out, const Edge e)
{
	out << "(" << e.v1().to_int() << ", " << e.v2().to_int() << "; " << e.to_int() << ")";
	return out;
}

inline std::ostream & operator<<(std::ostream &out, const HalfEdge e)
{
	out << "(" << e.v1().to_int() << ", " << e.v2().to_int() << "; " << e.to_int() << ")";
	return out;
}

inline std::ostream & operator<<(std::ostream &out, const Number n)
{
	out << n.to_int();
	return out;
}

inline std::ostream & operator<<(std::ostream &out, const Location l)
{
	out << "L(" << l.n1() << ", " << l.n2() << ", " << l.index() << ")";
	return out;
}

inline std::ostream & operator<<(std::ostream &out, const Graph &G)
{
	std::vector<Number> vertices;
	vertices.reserve(G.order());
	for (auto &r : G)
		vertices.push_back(r.n());
	std::sort(vertices.begin(), vertices.end());

	for (auto v : vertices) {
		std::vector<Number> neighbours;
		neighbours.reserve(G[v].degree());
		for (auto &i : G[v])
			neighbours.push_back(i.r2().n());
		std::sort(neighbours.begin(), neighbours.end());

		out << v.to_int() << ":";
		std::string separator = " ";
		for (auto &u : neighbours)
			out << separator << u.to_int();
		out << std::endl;
	}
	return out;
}

inline std::string print_nice_only_primary(const Graph &G)
{
	std::stringstream ss;
	std::vector<Number> vertices;
	vertices.reserve(G.order());
	for (auto &r : G)
		vertices.push_back(r.n());
	std::sort(vertices.begin(), vertices.end());

	for (auto v : vertices) {
		std::vector<Number> neighbours;
		neighbours.reserve(G[v].degree());
		for (auto &i : G[v]) {
			if (i.is_primary())
				neighbours.push_back(i.r2().n());
		}
		std::sort(neighbours.begin(), neighbours.end());

		ss << v.to_int() << ":";
		std::string separator = " ";
		for (auto &u : neighbours)
			ss << separator << u.to_int();
		ss << std::endl;
	}
	return ss.str();
}

inline std::string print_nice_vertices(const Graph &G)
{
	std::stringstream ss;
	ss << G.order() << " vertices: ";
	std::vector<int> vertices;
	vertices.reserve(G.order());
	for (auto &r : G)
		vertices.push_back(r.n().to_int());
	std::sort(vertices.begin(), vertices.end());
	std::string separator = "";
	for (auto v : vertices) {
		ss << separator << v;
		separator = " ";
	}
	ss << std::endl;
	return ss.str();
}

template <typename T>
std::string print_nice(Mapping<T> mapping)
{
	std::stringstream ss;
	ss << "[";
	std::string separator = "";
	for (auto &p : mapping) {
		ss << separator << p.first << ": " << p.second;
		separator = ", ";
	}
	ss << "]";
	return ss.str();
}

template <typename T>
std::string print_nice(VertexLabeling<T> labeling, const Graph &G)
{
	std::vector<Vertex> vertices;
	vertices.reserve(G.order());
	for (auto &r : G)
		vertices.push_back(r.v());
	std::sort(vertices.begin(), vertices.end(), [](Vertex v1, Vertex v2) { return v1.to_int() < v2.to_int(); });
	std::stringstream ss;
	ss << "[";
	std::string separator = "";
	for (auto &v : vertices) {
		ss << separator << v << ": " << labeling[v];
		separator = ", ";
	}
	ss << "]";
	return ss.str();
}

template <typename T>
std::string print_nice(EdgeLabeling<T> labeling, const Graph &G) {
	std::stringstream ss;
	ss << "[";
	std::string separator = "";
	for (Edge e : G.list(RP::all(), IP::primary(), IC::e(), IT::e())) {
		ss << separator << e.to_int() << ": " << labeling[e];
		separator = ", ";
	}
	ss << "]";
	return ss.str();
}

// NL is NumberLabeling or GraphNumberLabeling
template <typename NL>
std::string print_nice(NL labeling, const Graph &G) {
	std::vector<Number> vertices;
	for (auto &r : G)
		vertices.push_back(r.n());
	std::sort(vertices.begin(), vertices.end());

	std::stringstream ss;
	ss << "[";
	std::string separator = "";
	for (Number v : vertices) {
		ss << separator << v << ": " << labeling[v];
		separator = ", ";
	}
	ss << "]";
	return ss.str();
}

namespace internal
{

inline std::string neato_representation(const Graph &G, std::vector<Number> &redVertices)
{
	std::stringstream ss;
	ss << "graph {\noverlap = false;\nsplines = false;\nsep=.3;\nnode[margin=0, fontsize=12, shape=circle, height=.3, width=.3];\n";
	if (redVertices.size() > 0) {
		ss << "{node [color=red, style=filled]";
		for (Number v : redVertices)
			ss << v.to_int() << " ";
		ss << ";}\n";
	}
	for (auto ii : G.list(RP::all(), IP::primary()))
		ss << ii->n1().to_int() << " -- " << ii->n2().to_int() << " [id=\"" << ii->l().index() << "\"];\n";
	ss << "}\n";
	return ss.str();
}

} // namespace internal

inline void visualise_neato(const std::string outputFileName, const Graph &G, std::vector<Number> redVertices = {})
{
	std::stringstream command;
	auto r = internal::neato_representation(G, redVertices);
	r.erase(std::remove(r.begin(), r.end(), '\n'), r.end());
	command << "echo '" << r << "' | neato -Tpdf > '" << outputFileName << "'";

	if (system(command.str().c_str()) == -1)
		throw std::runtime_error("Call of system() failed");
}

} // namespace ba_graph

#endif
