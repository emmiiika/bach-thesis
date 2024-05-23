#ifndef BA_GRAPH_ALGORITHMS_ISOMORPHISM_HPP
#define BA_GRAPH_ALGORITHMS_ISOMORPHISM_HPP

#include "../../invariants/degree.hpp"
#include "../../invariants/distance.hpp"

#include <map>
#include <set>
#include <vector>

namespace ba_graph
{

namespace internal
{

// some magic copied from random internet sources...
inline int vector_hash(std::vector<int> v, int seed=-1)
{
	std::sort(v.begin(), v.end());
	int h = (seed != -1) ? seed : v.size();
	for (auto &n : v)
		h ^= n + 0x9e3779b9 + (h << 6) + (h >> 2);
	return h;
}

// degree, sorted distances
typedef std::pair<int, std::vector<int>> IsomorphismVertexHash;

inline IsomorphismVertexHash isomorphism_vertex_hash(const Graph &G, Number v)
{
	auto d = distances(G, v);
	std::vector<int> dist;
	dist.reserve(G.order());
	for (auto &v : G)
		dist.push_back(d[v.n()]);
	std::sort(dist.begin(), dist.end());
	return std::pair(G[v].degree(), dist);
}

inline int isomorphism_vertex_hash_int(const Graph &G, Number v)
{
	auto h = isomorphism_vertex_hash(G, v);
	return vector_hash(h.second, h.first * h.second.size());
}

inline int isomorphism_graph_hash(const Graph &G)
{
	std::vector<int> hashes;
	for (auto &r : G)
		hashes.push_back(isomorphism_vertex_hash_int(G, r.n()));
	return vector_hash(hashes);
}

} // namespace internal

class IsomorphismFinder
{
	const Graph &G, &H;

	std::map<Number, Number> isomorphism;
	std::map<Number, Number> inverseIsomorphism;

	std::vector<Number> ordering;
	std::map<Number, std::set<Number>> candidates;

	std::map<Number, internal::IsomorphismVertexHash> hashG, hashH;

	void setI(Number from, Number to)
	{
		isomorphism[from] = to;
		inverseIsomorphism[to] = from;
	}

	void unsetI(Number from, Number to=-1)
	{
		inverseIsomorphism.erase((to != -1) ? to : isomorphism[from]);
		isomorphism.erase(from);
	} 

	void clearI()
	{
		isomorphism.clear();
		inverseIsomorphism.clear();
	}

	std::map<Number, internal::IsomorphismVertexHash> computeHashes(const Graph &X)
	{
		std::map<Number, internal::IsomorphismVertexHash> result;
		for (auto &r : X)
			result[r.n()] = internal::isomorphism_vertex_hash(X, r.n());
		return result;
	}

	void computeHashes()
	{
		hashG = computeHashes(G);
		hashH = computeHashes(H);
	}

	// returns the number of vertices with images assigned directly based on hashes
	// or -1, if hashes are incompatible
	int computeOrdering()
	{
		ordering.reserve(G.order());

		for (auto &vG : G.list(RP::all(), RT::n())) {
			for (auto &vH : H.list(RP::all(), RT::n())) {
				if (hashH[vH] == hashG[vG])
					candidates[vG].insert(vH);
			}
			if (candidates[vG].size() == 0)
				return -1;
			if (candidates[vG].size() == 1) {
				Number candidate = *candidates[vG].begin();
				if (inverseIsomorphism.count(candidate) > 0)
					return -1;                
				// the candidate has not been used as an image yet
				setI(vG, candidate);
			}
		}

		std::map<Number, int> assignedNeighboursCounts;
		for (auto &vG : G)
			assignedNeighboursCounts[vG.n()] = 0;

		std::set<Number> unused;
		for (auto &vG : G.list(RP::all(), RT::n())) {
			if (isomorphism.count(vG) == 0) {
				unused.insert(vG);
			} else {
				ordering.push_back(vG); // these already assigned will be skipped in r()
				for (auto u : G[vG].neighbours()) {
					++assignedNeighboursCounts[u];
				}
			}
		}

		// next vertex is one that has maximum number of neighbours among those already assigned
		while (unused.size() > 0) {
			Number v;
			int max = -1;
			for (auto x : unused) {
				if (assignedNeighboursCounts[x] > max) {
					v = x;
					max = assignedNeighboursCounts[x];
				}
			}
			if (max == 0) {
				int maxDeg = G[v].degree();
				// nothing to guide us, we choose a large-degree vertex
				for (auto x : unused) {
					if (G[x].degree() > maxDeg) {
						v = x;
						max = G[x].degree();
					}
				}
			}
			ordering.push_back(v);
			unused.erase(v);
			for (auto u : G[v].neighbours())
				++assignedNeighboursCounts[u];
		}

		// check if the assignment forced by hashes is consistent
		for (auto &pair : isomorphism) {
			auto multiplicitiesG = multiplicities(G, pair.first, isomorphism);
			auto multiplicitiesH = multiplicities(H, pair.second, inverseIsomorphism);

			if (!checkMultiplicities(multiplicitiesG, multiplicitiesH))
				return -1;
		}

		return isomorphism.size();
	}

	bool done() { return isomorphism.size() == static_cast<unsigned>(G.order()); }

	void r(int next)
	{
		if (done()) return; // already found an isomorphism

		Number vG = ordering[next];
		// if vG has an assigned neighbour wG,
		// only try candidates vH that are neighbours of isomorphism(wG)
		Number wG = -1;
		for (auto &i : G[vG]) {
			if (isomorphism.count(i.n2()) > 0) {
				wG = i.n2();
				break;
			}
		}
		std::set<Number> potentialImages;
		if (wG != -1) {
			for (auto vH : H[isomorphism[wG]].neighbours()) {
				if (candidates[vG].count(vH) > 0)
					potentialImages.insert(vH);
			}
		} else {
			potentialImages = candidates[vG];
		}
		for (auto vH : potentialImages) {
			if (inverseIsomorphism.count(vH) > 0) continue;
			// try to assign the candidate and check for consistency
			setI(vG, vH);
	 
			auto multiplicitiesG = multiplicities(G, vG, isomorphism);
			auto multiplicitiesH = multiplicities(H, vH, inverseIsomorphism);

			if (checkMultiplicities(multiplicitiesG, multiplicitiesH)) {
				r(next + 1);
				if (done())
					return;
			}
			unsetI(vG, vH);
		}
	}

	std::map<Number, int> multiplicities(const Graph &G, Number v, std::map<Number, Number> isomorphism)
	{
		std::map<Number, int> result; // number of edges between v and each of its neighbours which have images already assigned
		for (auto &u : G[v].list(IP::all(), IT::n2())) {
			if (isomorphism.count(u) == 0) continue;
			if (result.count(u) == 0)
				result[u] = 1;
			else
				result[u]++;
		}
		return result;
	}

	bool checkMultiplicities(std::map<Number, int> mG, std::map<Number, int> mH)
	{
		if (mG.size() != mH.size())
			return false;
		for (auto &p : mG) {
			if (mH[isomorphism[p.first]] != p.second)
				return false;
		}
		return true;
	}

public:
	IsomorphismFinder(const Graph &G, const Graph &H) : G(G), H(H) {}

	bool are_isomorphic()
	{
		return getIsomorphism().size() == static_cast<unsigned>(G.order());
	}

	std::map<Number, Number> getIsomorphism()
	{
		if (isomorphism.size() != static_cast<unsigned>(G.order())) {
			if (G.order() != H.order()) return {};
			if (G.size() != H.size()) return {};
			computeHashes();
			int assignedCount = computeOrdering();
			if (assignedCount == -1) return {};

			r(assignedCount);

			if (isomorphism.size() != static_cast<unsigned>(G.order()))
				clearI();
		}
		return isomorphism;
	}
};

inline bool are_isomorphic(const Graph &G, const Graph &H)
{
	IsomorphismFinder myIF(G, H);
	return myIF.are_isomorphic();
}

} // namespace ba_graph

#endif
