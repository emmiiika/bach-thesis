#include "implementation.h"

#include <graphs/snarks.hpp>
#include <invariants.hpp>
#include <io/graph6.hpp>
#include <snarks/reductions.hpp>

#include <cassert>

using namespace ba_graph;

void digons()
{
	Factory f;
	Graph H1(create_petersen(f));
	reduce_digons(H1, f);
	assert(H1.order() == 10);
	for (auto &r : H1)
		assert(r.degree() == 3);
	assert(is_connected(H1));
	assert(girth(H1) == 5);

	H1 = read_graph6_line(":K`EGwAgUTjvOaDdI", f);
	reduce_digons(H1, f);
	assert(H1.order() == 8);
	for (auto &r : H1)
		assert(r.degree() == 3);
	assert(is_connected(H1));

	H1 = read_graph6_line(":K`EOi`GQDbOg`CziT", f);
	reduce_digons(H1, f);
	assert(H1.order() == 2);
	for (auto &r : H1)
		assert(r.degree() == 3);
	assert(is_connected(H1));
}

int main()
{
	digons();
}
