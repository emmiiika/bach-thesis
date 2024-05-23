#ifndef BA_GRAPH_SNARKS_REDUCTIONS_HPP
#define BA_GRAPH_SNARKS_REDUCTIONS_HPP

#include "../invariants/degree.hpp"
#include "../operations/add_graph.hpp"
#include "../operations/basic.hpp"
#include "../operations/copies.hpp"
#include "../operations/simplification.hpp"

#include "../io/print_nice.hpp"

namespace ba_graph
{

inline void reduce_digons(Graph &G, Factory &f)
{
#ifdef BA_GRAPH_DEBUG
    assert(check(G, f));
    assert(max_deg(G) <= 3);
#endif
    bool done = false;
    while (!done) {
        if (G.order() == 2)
            break; // we are not reducing the triple edge
        done = true;
        for (auto &r : G) {
            std::set<Number> neighbours;
            for (auto &ii : r.list(!IP::loop())) {
                if (neighbours.find(ii->n2()) != neighbours.end()) {
                    done = false;
                    // ii is part of a digon
                    Number n1 = ii->n1();
                    identify_vertices(G, n1, ii->n2(), f);
                    remove_loops(G, n1, f);
                    suppress_vertex(G, n1, f);
                    break;
                } else {
                    neighbours.insert(ii->n2());
                }
            }
            if (!done) break;
        }        
    }
}

} // namespace ba_graph

#endif
