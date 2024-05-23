#include "implementation.h"

#include <graphs/snarks.hpp>
#include <invariants.hpp>
#include <snarks/products.hpp>

#include <cassert>

using namespace ba_graph;

void two_product()
{
	Factory f;
	Graph H1(create_petersen(f));
	Graph H2(create_petersen(f));
	two_product(H1, Location(0, 1), H2, Location(2, 3), 12, f);
	assert(H1.order() == 20);
	for (auto &r : H1)
		assert(r.degree() == 3);
	assert(is_connected(H1));
	assert(girth(H1) == 5);
}

void three_product()
{
	Factory f;
	Graph H1(create_petersen(f));
	Graph H2(create_petersen(f));
	std::vector<Number> n1 = {H1[3][0].n2(), H1[3][1].n2(), H1[3][2].n2()};
	std::vector<Number> n2 = {H2[7][0].n2(), H2[7][1].n2(), H2[7][2].n2()};
	three_product(H1, 3, n1, H2, 7, n2, 12, f);
	assert(H1.order() == 18);
	for (auto &r : H1)
		assert(r.degree() == 3);
	assert(is_connected(H1));
	assert(girth(H1) == 5);
}

void dot_product()
{
	Graph H1(create_petersen());
	Graph H2(create_petersen());
	dot_product(H1, Location(0, 1), Location(2, 3), H2, Location(0, 1), 13);
	assert(H1.order() == 18);
	for (auto &r : H1)
		assert(r.degree() == 3);
}

void star_product()
{
	Graph H1(create_petersen());
	Graph H2(create_petersen());
	star_product(H1, {0, 1, 2, 3, 4}, H2, {0, 1, 2, 3, 4}, 13);
	assert(H1.order() == 10);
	for (auto &r : H1)
		assert(r.degree() == 3);
}

int main()
{
	two_product();
	three_product();
	dot_product();
	star_product();
}
