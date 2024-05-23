#include "implementation.h"
#include <operations/line_graph.hpp>

#include <cassert>
#include <iostream>

using namespace ba_graph;


int main() {
    Graph G(createG());
    addV(G, 0);
    addV(G, 1);
    addV(G, 2);
    Edge e0 = addE(G, Location(0,0)).e();
    Edge e1 = addE(G, Location(0,1)).e();
    Edge e2 = addE(G, Location(0,1)).e();
    Edge e3 = addE(G, Location(0,2)).e();
    Edge e4 = addE(G, Location(1,2)).e();

    auto [H, m] = line_graph_with_map(G);

    assert(H.order() == G.size());
    assert(H.size() == 14);

    std::vector<int> correct = {3, 5, 6, 6, 8};
    std::vector<int> answer;
    for(const auto &r: H) answer.push_back(r.degree());
    std::sort(answer.begin(),answer.end());
    assert(correct == answer);

    assert(H[m[e0]].contains(m[e1]));
    assert(H[m[e0]].contains(m[e2]));
    assert(H[m[e0]].contains(m[e3]));
    assert(!H[m[e0]].contains(m[e4]));
}
