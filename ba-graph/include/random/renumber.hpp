#ifndef BA_GRAPH_RANDOM_RENUMBER_HPP
#define BA_GRAPH_RANDOM_RENUMBER_HPP

#include <cstdlib>
#include <random>

namespace ba_graph {

inline Mapping<Number> randomize_vertices(Graph &G, Factory &f = static_factory)
{
    auto m1 = renumber_consecutive(G, f);
    std::vector<Number> vertices;
    vertices.reserve(G.order());
    for (auto &r : G)
        vertices.push_back(r.n());
    auto rng = std::default_random_engine {};
    std::shuffle(vertices.begin(), vertices.end(), rng);
    Mapping<Number> m2;
    for (int n = 0; n < G.order(); ++n) {
        m2.set(n, vertices[n]);
    }
    renumber_vertices(G, m2, f);
    m1.compose_with(m2);
    return m1;
}

} // namespace ba_graph

#endif
