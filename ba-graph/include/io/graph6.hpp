#ifndef BA_GRAPH_IO_GRAPH6_HPP
#define BA_GRAPH_IO_GRAPH6_HPP

#include "../graphs/basic.hpp"
#include "../invariants/girth.hpp"
#include "../operations/copies.hpp"

#include <climits>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>

// for graph6, sparse6 and similar formats, see
// https://users.cecs.anu.edu.au/~bdm/data/formats.txt
// https://raw.githubusercontent.com/networkx/networkx/master/networkx/readwrite/sparse6.py


namespace ba_graph
{

namespace internal
{

const int bias6 = 63;
const int mask6 = 0b00111111;

inline int graph6_order_length(int order)
{
	if (order < 63) return 1;
	if (order < 258048) return 4;
	return 8;
}

inline int decode_graph6_order(std::string::iterator s, std::string::iterator sEnd)
{
	if (s >= sEnd) throw std::invalid_argument("incorrect graph6 string");
	int n = *s++ - bias6;

	if (n > 62) {
		if (s >= sEnd) throw std::invalid_argument("incorrect graph6 string");
		n = *s++ - bias6;
		if (n > 62) {
			if (sEnd - s < 6) throw std::invalid_argument("incorrect graph6 string");
			n = *s++ - bias6;
			n = (n << 6) | (*s++ - bias6);
			n = (n << 6) | (*s++ - bias6);
			n = (n << 6) | (*s++ - bias6);
			n = (n << 6) | (*s++ - bias6);
			n = (n << 6) | (*s++ - bias6);
		} else {
			if (sEnd - s < 2) throw std::invalid_argument("incorrect graph6 string");
			n = (n << 6) | (*s++ - bias6);
			n = (n << 6) | (*s++ - bias6);
		}
	}
	return n;
}

inline std::string encode_graph6_order(int n)
{
	std::stringstream s;
	if (n < 64) {
		s << static_cast<char>(n + bias6);
		return s.str();
	} else if (n < 258048) {
		s << static_cast<char>(126);
		s << static_cast<char>(bias6 + (n >> 12));
		s << static_cast<char>(bias6 + ((n >> 6) & mask6));
		s << static_cast<char>(bias6 + (n & mask6));
	} else {
		s << static_cast<char>(126);
		s << static_cast<char>(126);
		s << static_cast<char>(bias6 + (n >> 30));
		s << static_cast<char>(bias6 + ((n >> 24) & mask6));
		s << static_cast<char>(bias6 + ((n >> 18) & mask6));
		s << static_cast<char>(bias6 + ((n >> 12) & mask6));
		s << static_cast<char>(bias6 + ((n >> 6) & mask6));
		s << static_cast<char>(bias6 + (n & mask6));
	}
	return s.str();
}

} // namespace internal

/*
 * Read a graph from the ascii representation of its graph6 bits.
 * Assumes that a possible >>graph6<< header is not present.
 */
inline Graph read_graph6(std::string::iterator s, std::string::iterator sEnd, Factory &f = static_factory)
{
	int n = internal::decode_graph6_order(s, sEnd);
	s += internal::graph6_order_length(n);
	Graph G(empty_graph(n, f));

	int x;
	int unusedBits = 0;
	for (int j = 0; j < n; j++) {
		for (int i = 0; i < j; i++) {
			if (unusedBits == 0) {
				if (s >= sEnd) throw std::invalid_argument("incorrect graph6 string");
				x = *s++ - 63;
				unusedBits = 6;
			}
			if (x & 0b00100000)
				addE(G, Location(i, j), f);
			x <<= 1;
			--unusedBits;
		}
	}
	return G;
}

namespace internal {

inline std::pair<std::vector<int>, std::vector<int>>
parse_sparse6_bits(std::string::iterator s, std::string::iterator sEnd, int n)
{
	if (s > sEnd) throw std::invalid_argument("incorrect sparse6 string");

	std::vector<int> bb, xx;

	int k = 1; // number of bits to represent n-1
	while ((1 << k) < n)
		++k;

	int d; // data bits
	int dLength = 0;  // number of useful bits in d
	while (1) {
		if (dLength < 1) {
			if (s == sEnd)
				return std::pair(bb, xx);
			d = (*s++) - bias6;
			dLength = 6;
		}
		--dLength;
		int b = (d >> dLength) & 1;

		int x = d & ((1 << dLength) - 1);
		int xLength = dLength;
		while (xLength < k) {
			if (s == sEnd)
				return std::pair(bb, xx);
			d = (*s++) - bias6;
			dLength = 6;
			x = (x << 6) + d;
			xLength += 6;
		}
		x = (x >> (xLength - k));
		dLength = xLength - k;
		bb.push_back(b);
		xx.push_back(x);
	}
	return std::pair(bb, xx);
}

} // namespace internal

/*
 * Read a graph from the ascii representation of its sparse6 bits.
 * Incremental sparse6 is not supported.
 * Assumes that a possible >>sparse6<< header is not present.
 */
inline Graph read_sparse6(std::string::iterator s, std::string::iterator sEnd, Factory &f = static_factory)
{
	if (*s == ':')
		return read_sparse6(++s, sEnd, f);

	int n = internal::decode_graph6_order(s, sEnd);
	s += internal::graph6_order_length(n);
	Graph G(empty_graph(n, f));

	std::vector<Rotation *> n_to_r(n);
	for(auto &rot: G) n_to_r[rot.n().to_int()]=&rot;


	auto [bb, xx] = internal::parse_sparse6_bits(s, sEnd, n);
	int v = 0;
	for (std::vector<int>::size_type i = 0; i < bb.size(); ++i) {
		int b = bb[i], x = xx[i];
		if (b == 1)
			++v;
		if ((x >= n) || (v >= n))
			break;
		else if (x > v)
			v = x;
		else
			addE(G, *n_to_r[x], *n_to_r[v], f);
	}

	return G;
}

inline Graph read_digraph6(std::string::iterator s, std::string::iterator sEnd, Factory &f = static_factory)
{
	s = s; sEnd = sEnd; // TODO avoid warning
	throw std::invalid_argument("digraph6 not supported");
	Graph G(createG(f));
	return G;
}

/*
 * Read a graph in graph6 or sparse6 format from a single line.
 * Incremental sparse6 is not supported.
 */
inline Graph read_graph6_line(std::string line, Factory &f = static_factory)
{
	if (line[0] == '>') {
		std::vector<std::string> headers = { ">>graph6<<", ">>sparse6<<", ">>digraph6<<" };
		bool removed = false;
		for (auto header : headers) {
			if (line.substr(0, header.size()) == header) {
				line = line.substr(header.size());
				removed = true;
			}
		}		
		if (!removed)
			throw std::invalid_argument("incorrect graph6 file");
	}
	if (line.back() == '\n') line.pop_back();
	auto s = line.begin();
	auto sEnd = line.end();
	if (*s == ':') {
		return read_sparse6(++s, sEnd, f);
	} else if (*s == ';') {
		throw std::invalid_argument("incremental sparse6 not supported");
	} else if (*s == '&') {
		return read_digraph6(++s, sEnd, f);
	} else {
		return read_graph6(s, sEnd, f);
	}
}

class MinimizedGraphs
{
	std::vector<std::string> graphStrings;
	Factory &initialF;

public:
	MinimizedGraphs(Factory &f = static_factory) : initialF(f) {}

	MinimizedGraphs(const std::vector<std::string> &graphStrings, Factory &f = static_factory)
			: graphStrings(graphStrings), initialF(f) {}

	MinimizedGraphs(const std::string &filename, Factory &f = static_factory) : initialF(f)
			{ load_from_file(filename); }

	inline void load_from_file(const std::string &filename)
	{
		std::ifstream fin(filename);
		if (!fin)
			throw std::runtime_error("cannot read file " + filename);
		std::string line;
		while (std::getline(fin, line))
			graphStrings.push_back(line);
	}

	inline void write_to_file(const std::string &file, bool append = false)
	{
		std::ofstream fout;
		auto flags = (append) ? (std::ios::app | std::ios::out) : std::ios::out;
		fout.open(file, flags);
		if (!fout)
			throw std::runtime_error("cannot write into file " + file);
		for (auto &s : graphStrings) {
			fout << s;
			if (s.back() != '\n')
				fout << std::endl;
		}
	}

	inline int size() { return graphStrings.size(); }

	inline Graph operator [](int i) { return std::move(read_graph6_line(graphStrings[i], initialF)); }

	inline Graph get(int i) {
		return get(i, initialF);
	}

	inline Graph get(int i, Factory &f) {
		return std::move(read_graph6_line(graphStrings[i], f));
	}

	inline std::string get_string(int i) {
		return graphStrings[i];
	}

	std::vector<std::string> & strings() {
		return graphStrings;
	}

	std::vector<Graph> graphs() {
		return graphs(initialF);
	}

	std::vector<Graph> graphs(Factory &f) {
		std::vector<Graph> result;
		for (auto &s : graphStrings)
			result.push_back(std::move(read_graph6_line(s, f)));
		return result;
	}
};

inline MinimizedGraphs read_graph6_stream(std::istream &in,
		Factory &f = static_factory,
		int from = 0, int to = INT_MAX, int step = 1)
{
#ifdef BA_GRAPH_DEBUG
	assert((0 <= from) && (from <= to) && (step > 0));
#endif
	std::vector<std::string> lines;
	if (from >= to)
		return MinimizedGraphs(std::vector<std::string>({}), f);

	std::string line;
	int count = -1;
	while (std::getline(in, line)) {
		++count;
		if (count >= to) break;
		if (count < from) continue;
		if (count % step != from % step) continue;
		lines.push_back(line);
	}
	return MinimizedGraphs(lines, f);
}

template <typename P>
void read_graph6_stream(std::istream &in,
		std::function<void(std::string &, Graph &, Factory &, P *)> callback, P *callbackParam,
		Factory *f=&static_factory, int from = 0, int to = INT_MAX, int step = 1)
{
#ifdef BA_GRAPH_DEBUG
	assert((0 <= from) && (from <= to) && (step > 0));
#endif
	if (from >= to)
		return;

	std::string line;
	int count = -1;
	while (std::getline(in, line)) {
		++count;
		if (count >= to) break;
		if (count < from) continue;
		if (count % step != from % step) continue;
		if (f != NULL) {
			Graph G = read_graph6_line(line, *f);
			callback(line, G, *f, callbackParam);
		} else {
			Factory localF;
			Graph G = read_graph6_line(line, localF);
			callback(line, G, localF, callbackParam);
		}
	}
}

/*
 * count the number of lines in the file, each line corresponds to a graph in a correct graph6/sparse6 file
 */
inline int read_graph6_file_count(const std::string &fileName, int from = 0, int to = INT_MAX)
{
	std::ifstream fin(fileName);
	if (!fin)
		throw std::runtime_error("cannot read file " + fileName);
	std::string line;
	int lineCount = -1;
	int graphCount = 0;
	while (std::getline(fin, line)) {		
		lineCount++;
		if (lineCount >= to) break;
		if (from <= lineCount)
			graphCount++;
	}
	return graphCount;
}

/*
	Reads a file containing graphs in graph6 or sparse6.
 */
inline MinimizedGraphs read_graph6_file(const std::string &fileName,
		Factory &f = static_factory, int from = 0, int to = INT_MAX, int step = 1)
{
#ifdef BA_GRAPH_DEBUG
	assert((0 <= from) && (from <= to) && (step > 0));
#endif
	std::ifstream fin(fileName);
	if (!fin)
		throw std::runtime_error("cannot read file " + fileName);
	return read_graph6_stream(fin, f, from, to, step);
}

/*
	Reads a file containing graphs in graph6 or sparse6 one line at a time.
	Only the graphs described by from, to, step are parsed.
	After parsing, the graph is constructed via the supplied factory
	(if no factory is suppied, an individual one will be created for each graph).
	For each constructed graph, the callback is called.
 */
template <typename P>
void read_graph6_file(const std::string &fileName,
		std::function<void(std::string &, Graph &, Factory &, P *)> callback, P *callbackParam,
		Factory *f=&static_factory, int from = 0, int to = INT_MAX, int step = 1)
{
#ifdef BA_GRAPH_DEBUG
	assert((0 <= from) && (from <= to) && (step > 0));
#endif
	std::ifstream fin(fileName);
	if (!fin)
		throw std::runtime_error("cannot read file " + fileName);
	read_graph6_stream<P>(fin, callback, callbackParam, f, from, to, step);
}

// graph6 only supports simple graphs
inline void write_graph6_stream(const Graph &G, std::ostream &out, bool includeNewline = true)
{
#ifdef BA_GRAPH_DEBUG
	assert(!has_loop(G));
	assert(!has_parallel_edge(G));
#endif
	const int n = G.order();
	Factory f;
	Graph H(copy_other_factory(G, f));
	renumber_consecutive(H, f);

	out << internal::encode_graph6_order(n);
	int remainingBits = 6;
	int x = 0;
	for (int j = 0; j < n; ++j) {
		for (int i = 0; i < j; ++i) {
			x <<= 1;
			if (H[i].find(Number(j)) != H[i].end())
				x |= 1;
			if (--remainingBits == 0) {
				out << static_cast<char>(internal::bias6 + x);
				remainingBits = 6;
				x = 0;
			}
		}
	}
	if (remainingBits != 6)
		out << static_cast<char>(internal::bias6 + (x << remainingBits));
	if (includeNewline)
		out << std::endl;
}

inline std::string write_graph6(const Graph &G, bool includeNewline = true)
{
	std::ostringstream s;
	write_graph6_stream(G, s, includeNewline);
	return s.str();
}

template <class GraphIterator>
void write_graph6_file(const std::string fileName, GraphIterator begin, GraphIterator end, bool append = false)
{
	std::ofstream fout;
	if (append)
		fout.open(fileName, std::ios_base::app);
	else
		fout.open(fileName);
	while (begin != end) {
		write_graph6_stream(*begin, fout);
		begin++;
	}
}

inline void write_graph6_file(const std::string fileName, const Graph &G, bool append = false)
{
	std::ofstream fout;
	if (append)
		fout.open(fileName, std::ios_base::app);
	else
		fout.open(fileName);
	write_graph6_stream(G, fout);
}

namespace internal {

inline std::vector<int> write_sparse6_enc(int k, int x)
{
	std::vector<int> result;
	for (int i = 0; i < k; ++i)
		result.push_back((x & 1 << (k - 1 - i)) ? 1 : 0);
	return result;
}

} // namespace internal

inline void write_sparse6_stream(const Graph &G, std::ostream &out, bool includeNewline = true)
{
	const int n = G.order();
	std::vector<Number> vertices;
	vertices.reserve(n);
	std::map<Vertex, int> vertexIndices;
	for (auto &r : G) {
		vertices.push_back(r.n());
	}
	std::sort(vertices.begin(), vertices.end());
	for (std::vector<Number>::size_type i = 0; i < vertices.size(); ++i) {
		vertexIndices[G[vertices[i]].v()] = i;
	}

	std::vector<std::pair<int, int>> edges;
	for (auto ii : G.list(RP::all(), IP::primary())) {
		int ui = vertexIndices[ii->v1()], vi = vertexIndices[ii->v2()];
		if (ui < vi)
			edges.push_back(std::pair(vi, ui));
		else
			edges.push_back(std::pair(ui, vi));
	}
	std::sort(edges.begin(), edges.end());

	out << ':' << internal::encode_graph6_order(n);

	unsigned int k = 1; // number of bits to represent n-1 in binary
	while ((1 << k) < n)
		++k;

	std::vector<int> bits;
	int currentV = 0;
	for (auto e : edges) {
		int v = e.first;
		int u = e.second;
		if (v == currentV) {
			bits.push_back(0);
			auto bb = internal::write_sparse6_enc(k, u);
			bits.insert(bits.end(), bb.begin(), bb.end());
		} else if (v == currentV + 1) {
			++currentV;
			bits.push_back(1);
			auto bb = internal::write_sparse6_enc(k, u);
			bits.insert(bits.end(), bb.begin(), bb.end());
		} else {
			currentV = v;
			bits.push_back(1);
			auto bb = internal::write_sparse6_enc(k, v);
			bits.insert(bits.end(), bb.begin(), bb.end());
			bits.push_back(0);
			bb = internal::write_sparse6_enc(k, u);
			bits.insert(bits.end(), bb.begin(), bb.end());
		}
	}
	unsigned int deficit = (bits.size() % 6 == 0) ? 0 : (6 - bits.size() % 6);
	if ((k < 6) && (n == (1 << k)) && (deficit >= k) && (currentV < n - 1)) {
		bits.push_back(0);
		for (std::vector<int>::size_type j = 0; j < deficit-1; ++j)
			bits.push_back(1);
	} else {
		for (std::vector<int>::size_type j = 0; j < deficit; ++j)
			bits.push_back(1);
	}

	for (std::vector<int>::size_type i = 0; i < bits.size(); i += 6) {
		int c = (bits[i] << 5) + (bits[i + 1] << 4) +
			(bits[i + 2] << 3) + (bits[i + 3] << 2) +
			(bits[i + 4] << 1) + (bits[i + 5] << 0);
		out << static_cast<char>(internal::bias6 + c);
	}
	if (includeNewline)
		out << std::endl;
}

inline std::string write_sparse6(const Graph &G, bool includeNewline = true)
{
	std::ostringstream s;
	write_sparse6_stream(G, s, includeNewline);
	return s.str();
}

template <class GraphIterator>
void write_sparse6_file(const std::string fileName, GraphIterator begin, GraphIterator end, bool append = false)
{
	std::ofstream fout;
	if (append)
		fout.open(fileName, std::ios_base::app);
	else
		fout.open(fileName);
	while (begin != end) {
		write_sparse6_stream(*begin, fout);
		begin++;
	}
}

inline void write_sparse6_file(const std::string fileName, const Graph &G, bool append = false)
{
	std::ofstream fout;
	if (append)
		fout.open(fileName, std::ios_base::app);
	else
		fout.open(fileName);
	write_sparse6_stream(G, fout);
}

} // namespace ba_graph

#endif
