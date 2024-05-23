#ifndef BA_GRAPH_ALGORITHMS_OBJECT_COLLECTION_GENERATION_HPP
#define BA_GRAPH_ALGORITHMS_OBJECT_COLLECTION_GENERATION_HPP

#include <string>

#include <algorithms/isomorphism/isomorphism.hpp>
#include <algorithms/isomorphism/isomorphism_nauty.hpp>
#include <io/graph6.hpp>
#include <operations/copies.hpp>

using namespace ba_graph;

// satisfies requirements for CollectibleObject
class CanonicalGraph
{
protected:
	bool minimised;
	int cachedHash;
	Factory f;
	Graph G = createG(f);

public:
	std::string serialisedG;

	CanonicalGraph(const Graph &g, bool do_inflate)
	{
		serialisedG = canonical_sparse6(g);
		cachedHash = std::hash<std::string>()(serialisedG);
		minimised = true;
		if (do_inflate)
			inflate();
	}

	CanonicalGraph(const std::string &s, bool do_inflate)
	{
		deserialise(s, do_inflate);
	}

	inline const Graph& graph() const {
		if (minimised)
			throw std::logic_error("minimised, graph is inaccessible");
		else
			return G;
	}

	inline int size() const {
		if (minimised)
			throw std::logic_error("minimised, graph is inaccessible");
		else
			return G.order();
	}

	inline int hash() const { return cachedHash; }

	inline void compute_property() const {}
	inline bool property_computed() const { return true; }
	inline int property_value() const { return 0; }

	inline bool operator==(const CanonicalGraph &other) const {
		return hash() == other.hash() && serialisedG == other.serialisedG;
	}

	inline bool is_minimised() const { return minimised; }

	// keep as small memory footprint as possible
	// useful for keeping failed candidates in ObjectCollection
	void minimise()
	{
		if (!minimised) {
			Graph H = createG(f);
			std::swap(G, H); // throw away contents of G
			minimised = true;
		}
	}

	// restores after minimization
	void inflate()
	{
		if (minimised) {
			Graph H = std::move(read_graph6_line(serialisedG, f));
			std::swap(G, H);
			minimised = false;
		}
	}

	std::string serialise() const
	{
		return serialisedG;
	}

	void deserialise(const std::string &s, bool do_inflate)
	{
		minimised = true;
		serialisedG = s;
		if (serialisedG.back() == '\n')
			serialisedG.pop_back();
		cachedHash = std::hash<std::string>()(serialisedG);

		if (do_inflate)
			inflate();
	}
};

// satisfies requirements for CollectibleObject
// prefer CanonicalGraph, which unfortunately does not work for graphs with parallel edges (and loops probably)
class GraphWithHash
{
protected:
	bool minimised;
	int cachedHash;
	Factory f;
	Graph G = createG(f);

public:
	std::string serialisedG;

	GraphWithHash(const Graph &g)
	{
		G = copy_other_factory(g, f);
		cachedHash = internal::isomorphism_graph_hash(G);
		serialisedG = write_sparse6(G, false);
		minimised = false;
	}

	GraphWithHash(const std::string &s, bool inflate)
	{
		deserialise(s, inflate);
	}

	inline const Graph& graph() const {
		if (minimised)
			throw std::logic_error("minimised, graph is inaccessible");
		else
			return G;
	}

	inline int size() const {
		if (minimised)
			throw std::logic_error("minimised, graph is inaccessible");
		else
			return G.order();
	}

	inline int hash() const { return cachedHash; }

	inline void compute_property() const {}
	inline bool property_computed() const { return true; }
	inline int property_value() const { return 0; }

	inline bool operator==(const GraphWithHash &other) const {
		if (hash() != other.hash())
			return false;		
		
		Factory localF;
		const Graph *g1;
		Graph g1storage = createG(localF);
		if (!minimised) {
			g1 = &G;
		} else {
			Graph H = read_graph6_line(serialisedG, localF);
			std::swap(g1storage, H);
			g1 = &g1storage;
		}
		const Graph *g2;
		Graph g2storage = createG(localF);
		if (!other.minimised) {
			g2 = &other.G;
		} else {
			Graph H = read_graph6_line(other.serialisedG, localF);
			std::swap(g2storage, H);
			g2 = &g2storage;
		}
		return are_isomorphic(*g1, *g2);
	}

	inline bool is_minimised() const { return minimised; }

	// keep as small memory footprint as possible
	// useful for keeping failed candidates in ObjectCollection
	void minimise()
	{
		if (!minimised) {
			Graph H = createG(f);
			std::swap(G, H); // throw away contents of G
			minimised = true;
		}
	}

	// restores after minimization
	void inflate()
	{
		if (minimised) {
			Graph H = std::move(read_graph6_line(serialisedG, f));
			std::swap(G, H);
			minimised = false;
		}
	}

	std::string serialise() const
	{
		std::stringstream ss;
		ss << cachedHash << " " << serialisedG;
		return ss.str();
	}

	void deserialise(const std::string &s, bool do_inflate)
	{
		minimised = true;

		std::stringstream ss(s);
		ss >> cachedHash;
		if (!ss.good())
			throw std::invalid_argument("cannot deserialise GraphWithHash from '" + s + "'");
		ss >> serialisedG;
		if (serialisedG.back() == '\n')
			serialisedG.pop_back();

		if (serialisedG.size() < 1)
			throw std::invalid_argument("cannot deserialise GraphWithHash from '" + s + "'");

		if (do_inflate)
			inflate();
	}
};

#endif
