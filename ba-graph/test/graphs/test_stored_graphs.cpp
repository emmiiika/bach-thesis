#include "implementation.h"

#include <algorithms/cyclic_connectivity.hpp>
#include <graphs/stored_graphs.hpp>
#include <invariants.hpp>
#include <io.hpp>

#include <cassert>

using namespace ba_graph;

int main()
{
    assert(internal::stored_cubic_path(2, 3, 4) == "cubic/cubic_cc02_g03/cubic_cc02_g03.04");
    assert(internal::stored_snark_path(2, 3, 4) == "snarks/cubic_cc02_g03.snark/cubic_cc02_g03.04.snark");

    Configuration cfg;
    cfg.load_from_string("{\"storage\": {\"dir\": \"../../resources/graphs\"}}");

    auto sg1 = StoredGraphs::create<SnarkStorageDataC4G4>(cfg);
    Graph g(sg1->get_graph(10, 0));
    assert(g.order() == 10);
    assert(max_deg(g) == 3);
    assert(min_deg(g) == 3);
    assert(girth(g) >= 4);
    assert(cyclic_connectivity(g) >= 4);

    auto sg2 = StoredGraphs::create<SnarkStorageDataC4G5>(cfg);
    g = std::move(sg2->get_graph(20, 0));
    assert(g.order() == 20);
    assert(max_deg(g) == 3);
    assert(min_deg(g) == 3);
    assert(girth(g) >= 5);
    assert(cyclic_connectivity(g) >= 4);

    auto sg3 = StoredGraphs::create<SnarkStorageDataC5G5>(cfg);
    g = std::move(sg3->get_graph(22, 0));
    assert(g.order() == 22);
    assert(max_deg(g) == 3);
    assert(min_deg(g) == 3);
    assert(girth(g) >= 5);
    assert(cyclic_connectivity(g) >= 5);

    // TODO add tests for other storage data

    auto sg = StoredGraphs::create<CubicStorageDataC1G3>(cfg);
    auto minimizedGraphs = sg->get_graphs(6);
    assert(minimizedGraphs.size() == 2);

    Factory f;
    MinimizedGraphs graphs(cfg.storage_dir() + "/" + internal::stored_cubic_path(1, 3, 6), f);
    assert(graphs.size() == 2);
    assert(graphs[0].order() == 6);
    assert(girth(graphs[0]) == 3);
    assert(graphs[1].order() == 6);
    assert(girth(graphs[1]) == 4);

    return 0;
}
