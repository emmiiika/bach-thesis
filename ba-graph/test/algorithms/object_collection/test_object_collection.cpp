#include <cassert>

#include <impl/basic/include.hpp>
#include <algorithms/object_collection/object_collection.hpp>
#include <algorithms/object_collection/generation.hpp>
#include <graphs.hpp>
#include <io/graph6.hpp>

using namespace ba_graph;

typedef ObjectCollection<CanonicalGraph, int> Collection;

int main() {
	Graph p1(create_petersen());
	CanonicalGraph gh1(p1, true);
	gh1.minimise();
	gh1.inflate();
	assert(are_isomorphic(gh1.graph(), p1));

	Collection collection("",
			Collection::ContainsAlgorithm::HASH_EQUALS,
			false, 100, false);

	Graph g1 = complete_graph(5);
	auto g1h = std::make_unique<CanonicalGraph>(g1, false);
	collection.add(std::move(g1h), true);
	assert(collection.size() == 1);
	g1h = std::make_unique<CanonicalGraph>(g1, false);
	assert(!collection.is_failed_candidate(g1h.get()));

	g1h = std::make_unique<CanonicalGraph>(g1, false);
	collection.add(std::move(g1h), true);
	assert(collection.size() == 1);
	g1h = std::make_unique<CanonicalGraph>(g1, true);
	assert(collection.is_failed_candidate(g1h.get()));
	collection.add(std::move(g1h), false);
	assert(collection.size() == 1);

	Graph g2 = complete_graph(5);
	auto g2h = std::make_unique<CanonicalGraph>(g2, false);
	g2h->minimise();
	collection.add(std::move(g2h), true);
	assert(collection.size() == 1);

	Graph g3 = complete_graph(7);
	auto g3h = std::make_unique<CanonicalGraph>(g3, true);
	g3h->minimise();
	collection.add(std::move(g3h), true);
	assert(collection.size() == 2);

    std::string baseDir = "../../../resources/graphs/";
    std::vector<std::string> files = {
			internal::stored_cubic_path(1, 3, 4), internal::stored_cubic_path(1, 3, 6),
			internal::stored_cubic_path(1, 3, 8), internal::stored_cubic_path(1, 3, 10),
			internal::stored_cubic_path(1, 3, 12), internal::stored_cubic_path(1, 3, 14),
			internal::stored_cubic_path(1, 3, 16)};
	for (auto &file : files) {
		collection.clear();
		auto graphs = read_graph6_file(baseDir + file).graphs();
		for (auto &G : graphs) {
			auto g = std::make_unique<CanonicalGraph>(G, true);
			collection.add(std::move(g), true);
		}
		assert(collection.size() == read_graph6_file_count(baseDir + file));
	}
}
