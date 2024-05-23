#ifndef BA_GRAPH_ALGORITHM_MATCHINGS_HPP
#define BA_GRAPH_ALGORITHM_MATCHINGS_HPP

#include <cassert>
#include <set>

namespace ba_graph
{

typedef std::set<Edge> Matching;

inline std::set<Matching> all_matchings(const Graph &G, int size)
{
#ifdef BA_GRAPH_DEBUG
    assert(0 <= size);
#endif
    std::set<Matching> result;
	if (size == 0) {
		result.insert(Matching({}));
		return result;
	}
    auto smaller = all_matchings(G, size - 1);
    for (auto m : smaller) {
		std::set<Vertex> endpoints;
		for (auto &e : m) {
			endpoints.insert(e.v1());
			endpoints.insert(e.v2());
		}
		for (auto &ii : G.list(RP::all(), IP::primary())) {
			if (endpoints.count(ii->v1()) > 0) continue;
			if (endpoints.count(ii->v2()) > 0) continue;
			auto newM = m;
			newM.insert(ii->e());
			result.insert(newM);		
		}
    }
    return result;
}

inline std::set<Matching> all_matchings(const Graph &G)
{
	std::set<Matching> result;
	for (int i = 0; i <= std::min(G.order() / 2, G.size()); ++i) {
		auto mi = all_matchings(G, i);
		result.insert(mi.begin(), mi.end());
	}
	return result;
}

} // namespace ba_graph

#endif
