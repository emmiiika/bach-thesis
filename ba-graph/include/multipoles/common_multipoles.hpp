#ifndef BA_GRAPH_MULTIPOLES_COMMON_MULTIPOLES_HPP
#define BA_GRAPH_MULTIPOLES_COMMON_MULTIPOLES_HPP

#include <cassert>

#include "../invariants/distance.hpp"
#include "../invariants/girth.hpp"
#include "../graphs/snarks.hpp"
#include "../graphs/trees.hpp"
#include "multipoles.hpp"
#include "../operations/basic.hpp"
#include "../util/math.hpp"

namespace ba_graph
{

/* ========================== splitting vertices or edges ====================== */

inline Multipole multipole_by_splitting(Graph &G, const std::vector<Number> &verticesToSplit,
		const std::vector<Location> &edgesToSplit, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(check(G, f));
	assert(firstTerminal >= 0);
	int terminalsCount = 0;
	for (auto &v : verticesToSplit) {
		assert(G.contains(v));
		terminalsCount += G[v].degree();
	}
	for (auto &l : edgesToSplit)
		assert(G.contains(l));
	terminalsCount += 2 * edgesToSplit.size();
	for (int i = 0; i < terminalsCount; ++i)
		assert(!G.contains(firstTerminal + i));
#endif
	std::vector<Connector> connectors;
	int next = firstTerminal.to_int();
	for (auto &v : verticesToSplit) {
		int d = G[v].degree();
		split_vertex(G, v, next, f);
		std::vector<Number> terminals(d);
		for (int i = 0; i < d; ++i)
			terminals[i] = next + i;
		connectors.push_back(Connector(terminals));
		next += d;
	}
	for (auto &l : edgesToSplit) {
		split_edge(G, l, next, f);
		std::vector<Number> terminals(2);
		for (int i = 0; i < 2; ++i)
			terminals[i] = next + i;
		connectors.push_back(Connector(terminals));
		next += 2;
	}
	return Multipole(connectors);
}

inline Multipole multipole_by_splitting(Graph &G, const std::set<Number> &verticesToSplit,
		const std::set<Location> &edgesToSplit, Number firstTerminal, Factory &f = static_factory)
{
	std::vector<Number> v(verticesToSplit.begin(), verticesToSplit.end());
	std::vector<Location> e(edgesToSplit.begin(), edgesToSplit.end());
	return multipole_by_splitting(G, v, e, firstTerminal, f);
}

inline Multipole multipole_by_splitting(Graph &G, const std::vector<Number> &verticesToSplit,
		const std::vector<Edge> &edgesToSplit, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(check(G, f));
#endif
	std::vector<Location> locationsToSplit;
	for (Edge e : edgesToSplit) {
#ifdef BA_GRAPH_DEBUG
		G.contains(e.v1(), e);
#endif
		locationsToSplit.push_back(G.find(e.v1(), e).second->l());
	}
	return multipole_by_splitting(G, verticesToSplit, locationsToSplit, firstTerminal, f);
}

/* ========================== removing vertices ====================== */

inline Multipole multipole_by_removing(Graph &G, std::vector<Number> verticesToRemove,
		Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(check(G, f));
	assert(firstTerminal >= 0);
	for (auto &v : verticesToRemove)
		assert(G.contains(v));
#endif
	std::set<Number> verticesToRemoveSet(verticesToRemove.begin(), verticesToRemove.end());
	std::set<Location> locationsToDelete;
	for (auto v : verticesToRemove) {
		for (auto &ii : G[v].list(IP::primary())) {
			if (verticesToRemoveSet.count(ii->n2()) > 0) {
				auto x = ii->l();
				locationsToDelete.insert(Location(x.n1(), x.n2()));
			}
		}
	}
	for (Location l : locationsToDelete)
		deleteE(G, l, f);
	Multipole m = multipole_by_splitting(G, verticesToRemove, std::vector<Location>({}), firstTerminal, f);
	m.flatten();
	return m;
}

/* ========================== paths and circuits  ====================== */

inline std::pair<Graph, Multipole> multipole_path(int length, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(length >= 1);
	assert(firstTerminal.to_int() >= length + 1);
#endif
	Graph G(path(length, f));
	std::vector<Number> terminals;
	std::vector<Connector> connectors;
	terminals.reserve(length + 2);
	int next = length + 1;
	for (int i = 0; i <= length; ++i) {
		int repeats = (i == 0 || i == length) ? 2 : 1;
		for (int j = 0; j < repeats; ++j) {
			Number v = next++;
			addV(G, v, f);
			addE(G, Location(v, G[i].n()), f);
			terminals.push_back(v);
		}
		if (repeats == 1)
			connectors.push_back(Connector({next - 1}));
		else
			connectors.push_back(Connector({next - 1, next - 2}));
	}
	return std::pair(std::move(G), Multipole(connectors));
}

inline Multipole multipole_path_complement(Graph &G, std::vector<Number> path, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(check(G, f));
	assert(firstTerminal >= 0);
	assert(path.size() >= 1);
	for (std::vector<Number>::size_type i = 0; i < path.size() - 1; ++i) {
		assert(G.contains(path[i]));
		assert(G.contains(path[i + 1]));
		assert(G.contains(Location(path[i], path[i + 1])));
	}
	for (Number i = firstTerminal; i < firstTerminal + path.size() + 2; i = i + 1)
		assert(!G.contains(i));
#endif
	std::deque<Connector> connectors;
	for (std::vector<Number>::size_type i = 0; i < path.size() - 1; ++i) {
		deleteE(G, Location(path[i], path[i + 1]));
		if (i > 0)
			connectors.push_back(Connector({path[i]}));
	}
	split_vertex(G, path[0], firstTerminal, f);
	Connector in({firstTerminal, firstTerminal + 1});
	connectors.push_front(in);
	split_vertex(G, path.back(), firstTerminal + 2, f);
	Connector out({firstTerminal + 2, firstTerminal + 3});
	connectors.push_back(out);
	return Multipole(std::vector<Connector>(connectors.begin(), connectors.end()));
}

inline std::pair<Graph, Multipole> multipole_circuit(int length, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(length >= 3);
	assert(firstTerminal.to_int() >= length);
#endif
	Graph G(circuit(length, f));
	std::vector<Number> dangling;
	dangling.reserve(length);
	for (int i = 0; i < length; ++i) {
		Number v = firstTerminal + i;
		addV(G, v, f);
		addE(G, Location(v, G[i].n()), f);
		dangling.push_back(v);
	}
	Connector all(dangling);
	return std::pair(std::move(G), Multipole({all}));
}

// we are removing the circuit from G, creating a multipole with single connector
// the original vertices on the circuit will become terminals
inline Multipole multipole_circuit_complement(Graph &G, std::vector<Number> circuit, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(check(G, f));
	assert(circuit.size() >= 1);
	for (std::vector<Number>::size_type i = 0; i < circuit.size(); i++) {
		unsigned i1 = (i + 1) % circuit.size();
		assert(G.contains(circuit[i]));
		assert(G.contains(circuit[i1]));
		assert(G.contains(Location(circuit[i], circuit[i1])));
	}
#endif
	for (std::vector<Number>::size_type i = 0; i < circuit.size(); i++) {
		unsigned i1 = (i + 1) % circuit.size();
		deleteE(G, Location(circuit[i], circuit[i1]), f);
	}
	Connector c(circuit);
	return Multipole({c});
}

/* ========================== trees  ====================== */

inline std::pair<Graph, Multipole> multipole_full_tree(int depth, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(depth >= 0);
	assert(firstTerminal.to_int() >= 3 * power(2, depth + 1) - 2);
#endif
	Graph G = regular_tree(3, depth + 1, f);
	auto leaves = G.list(RP::degree(1), RT::n());
	std::sort(leaves.begin(), leaves.end());
	Mapping<Number> m;
	Number next = firstTerminal;
	std::vector<Number> terminals;
	for (auto &v : leaves) {
		m.set(v, next);
		terminals.push_back(next);
		next = next + 1;
	}
	renumber_vertices(G, m, f);
	std::vector<Connector> connectors;
	if (depth == 0) {
		Connector c(terminals);
		connectors.push_back(c);
	} else {
		for (std::vector<Number>::size_type i = 0; i < terminals.size() / 2; ++i) {
			Connector c({ terminals[2 * i], terminals[2 * i + 1] });
			connectors.push_back(c);
		}
	}
	return std::pair(std::move(G), Multipole(connectors));
}

inline Multipole multipole_full_tree_complement(Graph &G, Number root, int depth, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(check(G, f));
	assert(G.contains(root));
	assert(girth(G) > 2 * depth);
	assert(firstTerminal >= 0);
#endif
	std::vector<Number> level = {root};
	std::vector<Number> nextLevel;
	for (int i = 0; i < depth; ++i) {
		for (auto v : level) {
			for (auto &rw : G[v]) {
				nextLevel.push_back(rw.n2());
			}
			deleteV(G, v, f);
		}
		level = nextLevel;
	}
	Number nextTerminal = firstTerminal;
	std::vector<Connector> connectors;
	for (auto v : level) {
		split_vertex(G, v, nextTerminal, f);
		if (depth == 0) {
			connectors.push_back(Connector({nextTerminal, nextTerminal + 1, nextTerminal + 2}));

		} else {
			connectors.push_back(Connector({nextTerminal, nextTerminal + 1}));
			nextTerminal = nextTerminal + 2;
		}
	}

	return Multipole(connectors);
}

/* ========================== typical multipoles contained in snarks ====================== */

// we are removing path uwv from G
inline Multipole multipole_negator(Graph &G, Number u, Number w, Number v,
							Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(check(G, f));
	assert(G.contains(u));
	assert(G.contains(w));
	assert(G.contains(v));
	assert(G.contains(Location(u, w)));
	assert(G.contains(Location(v, w)));
	assert(!G.contains(firstTerminal));
#endif
	return multipole_path_complement(G, {u, w, v}, firstTerminal, f);
}

// we are removing the edge given by l the vertex v from G, thus creating a (2, 3)-pole
inline Multipole multipole_23(Graph &G, Location l, Number v, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(firstTerminal >= 0);
	assert(G.contains(l) && G.contains(v));
	assert(!G.contains(firstTerminal));
	assert(!G.contains(firstTerminal + 1));
	assert(!G.contains(firstTerminal + 2));
	assert(!G.contains(firstTerminal + 3));
	assert(!G.contains(firstTerminal + 4));
#endif
	Number u1 = l.n1(), u2 = l.n2();
	deleteE(G, l, f);
	addV(G, firstTerminal, f);
	addE(G, Location(u1, firstTerminal), f);
	addV(G, firstTerminal + 1, f);
	addE(G, Location(u2, firstTerminal + 1), f);
	Connector c2({firstTerminal, firstTerminal + 1});

	split_vertex(G, v, firstTerminal + 2, f);
	Connector c3({firstTerminal + 2, firstTerminal + 3, firstTerminal + 4});

	return Multipole({c2, c3});
}

inline Multipole multipole_odd_222(Graph &G, Number v, Number firstTerminal, Factory &f = static_factory)
{
#ifdef BA_GRAPH_DEBUG
	assert(G.contains(v));
	assert(G[v].degree() == 3);
#endif
	auto nei = G[v].list(IP::all());
	std::vector<Number> u = {nei[0]->n2(), nei[1]->n2(), nei[2]->n2()};
#ifdef BA_GRAPH_DEBUG
	assert(u[0] != u[1] && u[1] != u[2] && u[2] != u[0]);
#endif
	deleteV(G, v);
	std::vector<Connector> c;
	for (std::vector<Number>::size_type i = 0; i < 3; ++i) {
		split_vertex(G, u[i], firstTerminal + 2 * i, f);
		c.push_back(Connector({firstTerminal + 2 * i, firstTerminal + 1 + 2 * i}));
	}
	return Multipole({c[0], c[1], c[2]});
}

/* ========================== typical multipoles from the Petersen graph ====================== */

inline std::pair<Graph, Multipole> multipole_negator_petersen(Number firstTerminal = 10, Factory &f = static_factory)
{
	Graph G(create_petersen(f));
	auto iv = G[0].list(IP::all());
	return std::pair(std::move(G), multipole_negator(G, iv[0]->n2(), 0, iv[1]->n2(), firstTerminal, f));
}

// distance: between the edge and vertex to be removed FIXME nie je ta vzdialenost vzdy rovna 1?
inline std::pair<Graph, Multipole> multipole_23_petersen(Number firstTerminal = 10, Factory &f = static_factory)
{
	Graph G(create_petersen(f));
	return std::pair(std::move(G), multipole_23(G, Location(2, 3), 0, firstTerminal, f));
}

inline std::pair<Graph, Multipole> multipole_odd_222_petersen(Factory &f = static_factory)
{
	auto [G, x] = multipole_circuit(6, 6, f);
	Connector y = x[0];
	Multipole m({Connector({y[0], y[3]}),
				 Connector({y[1], y[4]}),
				 Connector({y[2], y[5]})});
	return std::pair(std::move(G), m);
}

} // namespace ba_graph

#endif
