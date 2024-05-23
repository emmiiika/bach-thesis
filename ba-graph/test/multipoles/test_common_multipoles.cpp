#include "implementation.h"

#include <invariants/degree.hpp>
#include <invariants/girth.hpp>
#include <graphs.hpp>
#include <multipoles/common_multipoles.hpp>

using namespace ba_graph;

void test_multipole_by_splitting()
{
	Graph G(create_petersen());
	Multipole m = multipole_by_splitting(G, {0, 1}, {G[3].find(Number(4))->e()}, 10, static_factory);
	assert(m.size() == 3);
	assert(m.check_in(G));
	assert(G.order() == 16);
	assert(G.size() == 16);
	assert(girth(G) == 5);
}

void test_multipole_by_removing()
{
	Graph G(create_petersen());
	Multipole m = multipole_by_removing(G, {0, 1}, 10, static_factory);
	assert(m.size() == 1);
	assert(m.check_in(G));
	assert(G.order() == 12);
	assert(G.size() == 14);
	assert(girth(G) == 5);
}

void test_negator()
{
	Graph G(create_petersen());
	Multipole m = multipole_negator(G, 0, 1, 2, 10, static_factory);
	assert(m.size() == 3);
	assert(m.check_in(G));
	assert(G.order() == 12);
	assert(G.size() == 13);
	assert(girth(G) == 5);
	auto [g1, m1] = multipole_negator_petersen();
	assert(m1.size() == 3);
	assert(m1.check_in(g1));
	assert(g1.order() == 12);
	assert(g1.size() == 13);
	assert(girth(g1) == 5);
}

void test_multipole_23()
{
	Graph G(create_petersen());
	Multipole m = multipole_23(G, Location(0, 1), 8, 10, static_factory);
	assert(m.size() == 2);
	assert(m.check_in(G));
	assert(G.order() == 14);
	assert(G.size() == 16);
	assert(girth(G) == 5);

	G = std::move(create_petersen());
	m = multipole_23(G, Location(0, 1), 0, 10, static_factory);
	assert(m.size() == 2);
	assert(m.check_in(G));
	assert(G.order() == 14);
	assert(G.size() == 16);
	assert(girth(G) == 5);

	auto [g1, m1] = multipole_23_petersen(10);
	assert(m1.size() == 2);
	assert(m1.check_in(g1));
	assert(g1.order() == 14);
	assert(g1.size() == 16);
	assert(girth(g1) == 5);
}

void test_path()
{
	auto [g1, m1] = multipole_path(6, 7);
	assert(g1.order() == 16);
	assert(m1.check_in(g1));
	assert(m1.size() == 7);
	assert(m1[0].size() == 2);
	assert(m1[1].size() == 1);
	assert(m1[2].size() == 1);
	assert(m1[3].size() == 1);
	assert(m1[4].size() == 1);
	assert(m1[5].size() == 1);
	assert(m1[6].size() == 2);
	assert(girth(g1) == -1);
}

void test_path_complement()
{
	Graph G(create_petersen());
	Multipole m = multipole_circuit_complement(G, {0, 1, 2, 3, 4}, static_factory);
	assert(m.size() == 1);
	assert(m.check_in(G));
	assert(G.order() == 10);
	assert(G.size() == 10);
	assert(girth(G) == 5);
	assert(degree_sequence(G) == std::vector({3, 3, 3, 3, 3, 1, 1, 1, 1, 1}));
}

void test_circuit()
{
	auto [g1, m1] = multipole_circuit(6, 6);
	assert(g1.order() == 12);
	assert(m1.check_in(g1));
	assert(m1.size() == 1);
	assert(m1[0].size() == 6);
	assert(girth(g1) == 6);
}

void test_circuit_complement()
{
	Graph G(create_petersen());
	Multipole m = multipole_circuit_complement(G, {0, 1, 2, 3, 4}, static_factory);
	assert(m.check_in(G));
	assert(m.size() == 1);
	assert(m.check_in(G));
	assert(G.order() == 10);
	assert(G.size() == 10);
	assert(girth(G) == 5);
	assert(degree_sequence(G) == std::vector({3, 3, 3, 3, 3, 1, 1, 1, 1, 1}));
}

void test_tree()
{
	Factory f;
	auto [G, m] = multipole_full_tree(1, 100, f);
	assert(G.order() == 10);
	assert(m.check_in(G));
	assert(m.size() == 3);
	assert(m[0].size() == 2);
	assert(m[1].size() == 2);
	assert(m[2].size() == 2);
	assert(girth(G) == -1);
}

void test_tree_complement()
{
	Factory f;
	Graph G(create_petersen(f));
	Multipole m = multipole_full_tree_complement(G, 0, 1, 10, f);
	assert(m.size() == 3);
	assert(m.check_in(G));
	assert(G.order() == 12);
	assert(G.size() == 12);
	assert(girth(G) == 6);
	assert(degree_sequence(G) == std::vector({3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1}));
}

void test_odd_222()
{
	Graph G(create_petersen());
	Multipole m = multipole_odd_222(G, 0, 10);
	assert(m.size() == 3);
	assert(m.check_in(G));
	assert(G.order() == 12);
	assert(G.size() == 12);
	assert(girth(G) == 6);
	assert(degree_sequence(G) == std::vector({3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1}));
}

int main()
{
	test_multipole_by_splitting();
	test_multipole_by_removing();
	test_negator();
	test_multipole_23();
	test_path();
	test_path_complement();
	test_circuit();
	test_circuit_complement();
	test_tree();
	test_tree_complement();
	test_odd_222();
}
