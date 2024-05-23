#include "implementation.h"

#include <multipoles/multipoles.hpp>

using namespace ba_graph;

int main()
{
    Graph g(createG());
    addV(g, 1);
    addV(g, 2);
    addE(g, Location(1, 2));
    assert(Connector({1, 2}).check_in(g));
    assert(!Connector({1, 1}).check_in(g));
    assert(!Connector({1, 3}).check_in(g));
    assert(Multipole({Connector({1}), Connector({2})}).check_in(g));
    assert(!Multipole({Connector({1}), Connector({1})}).check_in(g));
    Multipole m({Connector({1}), Connector({2})});
    m.flatten();
    assert(m.size() == 1);
    assert(m[0].size() == 2);
}
