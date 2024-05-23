#include "implementation.h"

#include <graphs/snarks.hpp>
#include <multipoles/join.hpp>
#include <operations/basic.hpp>

using namespace ba_graph;

int main()
{
    Graph g(create_petersen());
    Graph h(create_petersen());
    split_vertex(g, 0, 10);
    split_vertex(g, 2, 13);
    Multipole m({Connector({10, 11, 12}), Connector({13, 14, 15})});
    Number nei1 = g[14].begin()->n2();
    split_vertex(h, 5, 10);
    split_vertex(h, 7, 14);
    Multipole m2({Connector({10, 12, 11}), Connector({14, 15, 16})});
    join(g, m, 1, h, m2, 0, 17);

    Number nei2 = h[12].begin()->n2() + 17;
    assert(g[nei1].contains(nei2));
    assert(g.contains(Number(10)));
    assert(!g.contains(Number(0)));
    assert(!g.contains(Number(13)));
    assert(!g.contains(Number(12 + 17)));
    assert(g.contains(Number(6 + 17)));
    assert(!g.contains(Number(7 + 17)));
    assert(g.contains(Number(16 + 17)));
    assert(g.order() == 22);

    assert(m.size() == 2);
    assert(m[0][1] == 11);
    assert(m[1][2] == 16 + 17);

    join(g, m, 0, 1);

    assert(m.size() == 0);
    assert(g.order() == 16);
    for (auto &rot : g)
        assert(rot.degree() == 3);
}
